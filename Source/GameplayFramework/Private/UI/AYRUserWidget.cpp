// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AYRUserWidget.h"

void UAYRUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	verify(this->UISubsystem = ULocalPlayer::GetSubsystem<UUISubsystem>(this->GetOwningLocalPlayer()));

}

void UAYRUserWidget::OnEnterThisWidget(APlayerController* InPlayerController, const FUIStackInfo* InUIStackInfo, EUIStateChangedReason::Type InReason)
{
	// 应用UI状态信息。
	this->UISubsystem->ApplyUIInfo(InPlayerController, InUIStackInfo);

	// 调用蓝图版本。
	this->BP_OnEnterThisWidget(InPlayerController, InUIStackInfo ? *InUIStackInfo : FUIStackInfo(), InReason);
}

void UAYRUserWidget::OnLeaveThisWidget(EUIStateChangedReason::Type InReason)
{
	// 调用蓝图版本。
	this->BP_OnLeaveThisWidget(InReason);
}
