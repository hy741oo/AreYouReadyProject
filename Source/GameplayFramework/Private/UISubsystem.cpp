// Fill out your copyright notice in the Description page of Project Settings.


#include "UISubsystem.h"

#include "Blueprint/UserWidget.h"
#include "UI/AYRUserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Widgets/SViewport.h"

DEFINE_LOG_CATEGORY(LogUISubsystem);

void UUISubsystem::Initialize(FSubsystemCollectionBase& InCollection)
{
	if (this->CleanDelegateHandle.IsValid())
	{
		FCoreUObjectDelegates::PreLoadMap.Remove(this->CleanDelegateHandle);
	}

	this->CleanDelegateHandle = FCoreUObjectDelegates::PreLoadMap.AddWeakLambda(this,
		[this](const FString& InMapPath)
		{
			UE_LOG(LogUISubsystem, Log, TEXT("Preloading new map, map path: %s, clearning UI subsystem."), *InMapPath);
			this->ClearUIStack();
		}
	);

	// 注册输入设备切换事件。用于当玩家从键鼠切换到手柄时可以让当前栈顶UI接收到事件，并设置对应的Focus。
	if (UGameplayMessageSystem* GMS = UGameInstance::GetSubsystem<UGameplayMessageSystem>(this->GetGameInstance()))
	{
		FGameplayTag Tag = UGameplayTagsManager::Get().RequestGameplayTag(TEXT("GMSMessage.System.Input.DeviceType"));
		FOnMessageReceived Callback;
		Callback.BindUObject(this, &UUISubsystem::OnInputDeviceChanged);
		this->InputDeviceMessageHandle = GMS->Register(Tag, Callback);
	}
}

void UUISubsystem::Deinitialize()
{
	FCoreUObjectDelegates::PreLoadMap.Remove(this->CleanDelegateHandle);
	this->CleanDelegateHandle.Reset();
	this->ClearUIStack();
	UGameInstance::GetSubsystem<UGameplayMessageSystem>(this->GetGameInstance())->Unregister(this->InputDeviceMessageHandle);
}

UAYRUserWidget* UUISubsystem::PushUI(FName InUIID)
{
	UAYRUserWidget* CreatedWidget = nullptr;

	UGameConfigSubsystem* ConfigSubsystem = UGameInstance::GetSubsystem<UGameConfigSubsystem>(this->GetWorld()->GetGameInstance());
	FUIInfoTableRow* UIInfoTableRow = nullptr;
	if (ConfigSubsystem->GetDataTableRowFromID(InUIID, UIInfoTableRow))
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);

		// 生成UI。
		CreatedWidget = CreateWidget<UAYRUserWidget>(PlayerController, UIInfoTableRow->UIClass, UIInfoTableRow->UIName);
		if (ensureAlways(CreatedWidget))
		{
			FUIStackInfo UIStackInfo;
			CreatedWidget->AddToViewport(UIInfoTableRow->UIStateInfo.ZOrder);
			UIStackInfo.UserWidget = CreatedWidget;
			UIStackInfo.UIStateInfo = UIInfoTableRow->UIStateInfo;

			// 压栈。
			int32 StackIndex = this->UIStack.Add(UIStackInfo);
			CreatedWidget->StackIndex = StackIndex;
			if (StackIndex > 0)
			{
				// 如果栈不为空，则需要为之前的栈顶元素执行脱栈操作。
				FUIStackInfo& StackTop = this->UIStack[StackIndex - 1];
				if (StackTop.UserWidget)
				{
					StackTop.UserWidget->OnLeaveThisWidget(EUIStateChangedReason::UISCR_NewWidgetEntered);
				}
			}
			CreatedWidget->OnEnterThisWidget(PlayerController, &UIStackInfo, EUIStateChangedReason::UISCR_NewWidgetEntered);
		}
	}
	else
	{
		UE_LOG(LogUISubsystem, Warning, TEXT("Can't find UIID: \"%s\""), *InUIID.ToString());
	}

	return CreatedWidget;
}

void UUISubsystem::PopUI(const UAYRUserWidget* InSpecifiedUI)
{
	if (this->UIStack.Num() == 0) return;

	if (InSpecifiedUI)
	{
		// 将指定UI一直到栈顶的全部UI弹出。
		FUIStackInfo OldStackTop;
		int32 CurrentStackIndex = -1;
		do 
		{
			OldStackTop = this->UIStack.Pop();
			CurrentStackIndex = OldStackTop.UserWidget->StackIndex;
			OldStackTop.UserWidget->OnLeaveThisWidget(EUIStateChangedReason::UISCR_BePopped);
			OldStackTop.UserWidget->RemoveFromParent();
		} 
		while (InSpecifiedUI->StackIndex < CurrentStackIndex);

		// 获取当前栈顶UI并执行进入状态操作。
		if (this->UIStack.IsValidIndex(this->UIStack.Num() - 1))
		{
			FUIStackInfo& CurrentStackTop = this->UIStack[this->UIStack.Num() - 1];
			CurrentStackTop.UserWidget->OnEnterThisWidget(UGameplayStatics::GetPlayerController(this, 0), &CurrentStackTop, EUIStateChangedReason::UISCR_NewWidgetEntered);
		}
	}
	else 
	{
		// 如果未指定要移除的UI，则从栈顶开始移除。
		if (this->UIStack.IsValidIndex(0))
		{
			FUIStackInfo OldStackTop = this->UIStack.Pop();
			OldStackTop.UserWidget->OnLeaveThisWidget(EUIStateChangedReason::UISCR_BePopped);
			OldStackTop.UserWidget->RemoveFromParent();

			if (this->UIStack.IsValidIndex(this->UIStack.Num() - 1))
			{
				FUIStackInfo& CurrentStackTop = this->UIStack[this->UIStack.Num() - 1];
				CurrentStackTop.UserWidget->OnEnterThisWidget(UGameplayStatics::GetPlayerController(this, 0), &CurrentStackTop, EUIStateChangedReason::UISCR_BePopped);
			}
		}
	}

	// 出栈操作后如果栈内已经没有任何UI，则执行重置输入设置的操作。
	if (this->UIStack.Num() == 0)
	{
		this->ResetInputSetting();
	}

}

void UUISubsystem::ApplyUIInfo(APlayerController* InPlayerController, const FUIStackInfo* InUIStackInfo)
{
	if (!InPlayerController || !InUIStackInfo || !InUIStackInfo->UserWidget) return;

	FUIStateInfoTableRow UIStateInfo = InUIStackInfo->UIStateInfo;
	UAYRUserWidget* UI = InUIStackInfo->UserWidget;

	// 设置输入模式。需要判断是否需要在SetInputMode时Focus我们生成的Widget，防止Widget的Construct函数里面设置好的Focus被这里的设置打乱了。
	FAYRInputModeData InputMode;
	InputMode.UIStateInfo = InUIStackInfo->UIStateInfo;
	InPlayerController->SetInputMode(InputMode);

	// 是否需要显示鼠标。
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
	{
		PlayerController->SetShowMouseCursor(UIStateInfo.bShowMouseCursor);
	}
}

void UUISubsystem::ClearUIStack()
{
	// 销毁所有的UI。
	while (this->UIStack.Num() > 0)
	{
		this->PopUI();
	}

	// 恢复默认设置。
	this->ResetInputSetting();
}

void UUISubsystem::ResetInputSetting() const
{
	if (UGameViewportClient* GameViewportClient = this->GetWorld()->GetGameViewport())
	{
		GameViewportClient->SetIgnoreInput(false);
		GameViewportClient->SetMouseCaptureMode(EMouseCaptureMode::CapturePermanently);
		GameViewportClient->SetHideCursorDuringCapture(false);
		GameViewportClient->SetMouseLockMode(EMouseLockMode::LockOnCapture);
	}
}


void UUISubsystem::OnInputDeviceChanged(UGMSMessageBase* InMessage)
{
	if (UGMSInputDeviceType* Msg = Cast<UGMSInputDeviceType>(InMessage))
	{
		// 如果切换为控制器则让栈顶UI获得响应。
		if (Msg->CurrentType == EInputDeviceType::IDT_Controller)
		{
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
			this->OnInputDeviceChangeIntoGamepad(PlayerController);
		}
	}
}

void UUISubsystem::OnInputDeviceChangeIntoGamepad(APlayerController* InPlayerController)
{
	if (this->UIStack.Num() > 0)
	{
		FUIStackInfo& LastUIStackInfo = this->UIStack.Last();
		if (UAYRUserWidget* LastWidget = LastUIStackInfo.UserWidget)
		{
			LastWidget->OnEnterThisWidget(InPlayerController, &LastUIStackInfo, EUIStateChangedReason::UISCR_InputDeviceChangedIntoGamepad);
		}
	}
}

void FAYRInputModeData::ApplyInputMode(FReply& SlateOperations, UGameViewportClient& GameViewportClient) const
{
	if (TSharedPtr<SViewport> Viewport = GameViewportClient.GetGameViewportWidget())
	{
		TSharedRef<SViewport> ViewportWidgetRef = Viewport.ToSharedRef();
		{
			SlateOperations.UseHighPrecisionMouseMovement(ViewportWidgetRef);
			SlateOperations.SetUserFocus(ViewportWidgetRef);
			SlateOperations.LockMouseToWidget(ViewportWidgetRef);
		}

		if (this->UIStateInfo.MouseCaptureMode == EMouseCaptureMode::NoCapture)
		{
			SlateOperations.ReleaseMouseCapture();
		}

		if (this->UIStateInfo.MouseLockMode == EMouseLockMode::DoNotLock)
		{
			SlateOperations.ReleaseMouseLock();
		}

		GameViewportClient.SetMouseLockMode(this->UIStateInfo.MouseLockMode);
		GameViewportClient.SetMouseCaptureMode(this->UIStateInfo.MouseCaptureMode);
		GameViewportClient.SetHideCursorDuringCapture(this->UIStateInfo.bHideCursorDuringCapture);
	}
}

