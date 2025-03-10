// Fill out your copyright notice in the Description page of Project Settings.


#include "UISubsystem.h"

#include "Blueprint/UserWidget.h"
#include "UI/AYRUserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/GameInstanceSubsystem.h"

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
			UE_LOG(LogUISubsystem, Display, TEXT("Preloading new map, map path: %s, clearning UI subsystem."), *InMapPath);
			this->Clear();
		}
	);
}

void UUISubsystem::Deinitialize()
{
	FCoreUObjectDelegates::PreLoadMap.Remove(this->CleanDelegateHandle);
	this->CleanDelegateHandle.Reset();
	this->Clear();
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
					StackTop.UserWidget->OnLeaveThisWidget(EUIStateChangedReason::NewWidgetEntered);
				}
			}
			CreatedWidget->OnEnterThisWidget(PlayerController, &UIStackInfo, EUIStateChangedReason::NewWidgetEntered);
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
			OldStackTop.UserWidget->OnLeaveThisWidget(EUIStateChangedReason::BePopped);
			OldStackTop.UserWidget->RemoveFromParent();
		} 
		while (InSpecifiedUI->StackIndex < CurrentStackIndex);

		// 获取当前栈顶UI并执行进入状态操作。
		if (this->UIStack.IsValidIndex(this->UIStack.Num() - 1))
		{
			FUIStackInfo& CurrentStackTop = this->UIStack[this->UIStack.Num() - 1];
			CurrentStackTop.UserWidget->OnEnterThisWidget(UGameplayStatics::GetPlayerController(this, 0), &CurrentStackTop, EUIStateChangedReason::NewWidgetEntered);
		}
	}
	else 
	{
		// 如果未指定要移除的UI，则从栈顶开始移除。
		if (this->UIStack.IsValidIndex(0))
		{
			FUIStackInfo OldStackTop = this->UIStack.Pop();
			OldStackTop.UserWidget->OnLeaveThisWidget(EUIStateChangedReason::BePopped);
			OldStackTop.UserWidget->RemoveFromParent();

			if (this->UIStack.IsValidIndex(this->UIStack.Num() - 1))
			{
				FUIStackInfo& CurrentStackTop = this->UIStack[this->UIStack.Num() - 1];
				CurrentStackTop.UserWidget->OnEnterThisWidget(UGameplayStatics::GetPlayerController(this, 0), &CurrentStackTop, EUIStateChangedReason::BePopped);
			}
		}
	}
}

void UUISubsystem::ApplyUIInfo(APlayerController* InPlayerController, const FUIStackInfo* InUIStackInfo)
{
	if (!InPlayerController || !InUIStackInfo || !InUIStackInfo->UserWidget) return;

	FUIStateInfoTableRow UIStateInfo = InUIStackInfo->UIStateInfo;
	UAYRUserWidget* UI = InUIStackInfo->UserWidget;

	// 设置输入模式。
	switch (InUIStackInfo->UIStateInfo.InputMode)
	{
	case EUIInputMode::IM_GameAndUI:
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(InPlayerController, UI, UIStateInfo.MouseLockMode, UIStateInfo.bHideCursorDuringCapture);
		break;
	case EUIInputMode::IM_GameOnly:
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(InPlayerController);
		break;
	case EUIInputMode::IM_UIOnly:
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(InPlayerController, UI, UIStateInfo.MouseLockMode);
		break;
	default:
		ensureAlways(false);
	}

	// 设置输入优先级。
	UI->SetInputPriority(UIStateInfo.Priority);

	// 设置是否阻挡Action。
	UI->SetIsStopAction(UIStateInfo.bStopAction);

	// 是否需要显示鼠标。
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
	{
		PlayerController->SetShowMouseCursor(UIStateInfo.bShowMouseCursor);
	}
}

void UUISubsystem::Clear()
{
	// 销毁所有的UI。
	while (this->UIStack.Num() > 0)
	{
		this->PopUI();
	}

	// 初始化界面的输入设置。
	if (UWorld* World = this->GetWorld())
	{
		if (UGameViewportClient* GameViewportClient = World->GetGameViewport())
		{
			GameViewportClient->SetIgnoreInput(false);
			GameViewportClient->SetMouseCaptureMode(EMouseCaptureMode::CapturePermanently);
			GameViewportClient->SetHideCursorDuringCapture(false);
			GameViewportClient->SetMouseLockMode(EMouseLockMode::LockOnCapture);
		}
	}
}

