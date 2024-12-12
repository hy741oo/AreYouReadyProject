// Fill out your copyright notice in the Description page of Project Settings.


#include "UISubsystem.h"

#include "Blueprint/UserWidget.h"
#include "UI/AYRUserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

UAYRUserWidget* UUISubsystem::PushUI(FName InUIID)
{
	UAYRUserWidget* CreatedWidget = nullptr;

	UConfigSubsystem* ConfigSubsystem = this->GetGameInstance()->GetSubsystem<UConfigSubsystem>();
	if (ensure(ConfigSubsystem))
	{
		FUIInfoTableRow* UIInfoTableRow = nullptr;
		if (ensureAlways(ConfigSubsystem->GetDataTableRowFromID<FUIInfoTableRow>(InUIID, UIInfoTableRow)))
		{
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(ConfigSubsystem, 0);

			// ����UI��
			CreatedWidget = CreateWidget<UAYRUserWidget>(PlayerController, UIInfoTableRow->UIClass, UIInfoTableRow->UIName);
			if (ensureAlways(CreatedWidget))
			{
				FUIStackInfo UIStackInfo;
				CreatedWidget->AddToViewport(UIInfoTableRow->UIStateInfo.ZOrder);
				UIStackInfo.UserWidget = CreatedWidget;
				UIStackInfo.UIStateInfo = UIInfoTableRow->UIStateInfo;

				// ѹջ��
				int32 StackIndex = this->UIStack.Add(UIStackInfo);
				CreatedWidget->StackIndex = StackIndex;
				if (StackIndex > 0)
				{
					// ���ջ��Ϊ�գ�����ҪΪ֮ǰ��ջ��Ԫ��ִ����ջ������
					FUIStackInfo& StackTop = this->UIStack[StackIndex - 1];
					if (StackTop.UserWidget)
					{
						StackTop.UserWidget->OnLeaveThisWidget(ELeaveReason::LR_NewWidgetEntered);
					}
				}
				CreatedWidget->OnEnterThisWidget(PlayerController, &UIStackInfo);
			}
		}
	}

	return CreatedWidget;
}

void UUISubsystem::PopUI(const UAYRUserWidget* InSpecifiedUI)
{
	if (this->UIStack.Num() == 0) return;

	if (InSpecifiedUI)
	{
		// ��ָ��UIһֱ��ջ����ȫ��UI������
		FUIStackInfo OldStackTop;
		int32 CurrentStackIndex = -1;
		do 
		{
			OldStackTop = this->UIStack.Pop();
			CurrentStackIndex = OldStackTop.UserWidget->StackIndex;
			OldStackTop.UserWidget->OnLeaveThisWidget(ELeaveReason::LR_BePopped);
			OldStackTop.UserWidget->RemoveFromParent();
		} 
		while (InSpecifiedUI->StackIndex < CurrentStackIndex);

		// ��ȡ��ǰջ��UI��ִ�н���״̬������
		if (this->UIStack.IsValidIndex(this->UIStack.Num() - 1))
		{
			FUIStackInfo& CurrentStackTop = this->UIStack[this->UIStack.Num() - 1];
			CurrentStackTop.UserWidget->OnEnterThisWidget(UGameplayStatics::GetPlayerController(this, 0), &CurrentStackTop);
		}
	}
	else 
	{
		// ���δָ��Ҫ�Ƴ���UI�����ջ����ʼ�Ƴ���
		if (this->UIStack.IsValidIndex(0))
		{
			FUIStackInfo OldStackTop = this->UIStack.Pop();
			OldStackTop.UserWidget->OnLeaveThisWidget(ELeaveReason::LR_BePopped);
			OldStackTop.UserWidget->RemoveFromParent();

			if (this->UIStack.IsValidIndex(this->UIStack.Num() - 1))
			{
				FUIStackInfo& CurrentStackTop = this->UIStack[this->UIStack.Num() - 1];
				CurrentStackTop.UserWidget->OnEnterThisWidget(UGameplayStatics::GetPlayerController(this, 0), &CurrentStackTop);
			}
		}
	}
}

void UUISubsystem::ApplyUIInfo(APlayerController* InPlayerController, const FUIStackInfo* InUIStackInfo)
{
	if (!InPlayerController && !InUIStackInfo && !InUIStackInfo->UserWidget) return;

	FUIStateInfoTableRow UIStateInfo = InUIStackInfo->UIStateInfo;
	UAYRUserWidget* UI = InUIStackInfo->UserWidget;

	// ��������ģʽ��
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

	// �����������ȼ���
	UI->SetInputPriority(UIStateInfo.Priority);

	// �����Ƿ��赲Action��
	UI->SetIsStopAction(UIStateInfo.bStopAction);
}

