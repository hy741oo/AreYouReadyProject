// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AYRUserWidget.h"

void UAYRUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	verify(this->UISubsystem = this->GetGameInstance()->GetSubsystem<UUISubsystem>());

}

void UAYRUserWidget::OnEnterThisWidget(APlayerController* InPlayerController, const FUIStackInfo* InUIStackInfo)
{
	// Ӧ��UI״̬��Ϣ��
	this->UISubsystem->ApplyUIInfo(InPlayerController, InUIStackInfo);

	// ������ͼ�汾��
	this->BP_OnEnterThisWidget(InPlayerController, InUIStackInfo ? *InUIStackInfo : FUIStackInfo());
}

void UAYRUserWidget::OnLeaveThisWidget(ELeaveReason::Type InLeaveReason)
{

	// ������ͼ�汾��
	this->BP_OnLeaveThisWidget(InLeaveReason);
}

void UAYRUserWidget::SetInputPriority(int32 InPriority)
{
	this->SetInputActionPriority(InPriority);
}

void UAYRUserWidget::SetIsStopAction(bool bInStopAction)
{
	this->SetInputActionBlocking(bInStopAction);
}

