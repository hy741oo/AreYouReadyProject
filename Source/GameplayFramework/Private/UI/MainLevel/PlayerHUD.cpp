// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainLevel/PlayerHUD.h"

#include "Components/CanvasPanelSlot.h"
#include "UI/Common/InputIcon.h"


void UPlayerHUD::SetCrosshairPositionInScreen(const FVector2D& InPosition)
{
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(this->Crosshair->Slot);
	CanvasSlot->SetPosition(InPosition);
}

void UPlayerHUD::OnEnterInteractableState()
{
	this->InteractionInputIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UPlayerHUD::OnLeaveInteractableState()
{
	this->InteractionInputIcon->SetVisibility(ESlateVisibility::Collapsed);
}

void UPlayerHUD::NativeDestruct()
{
	this->Crosshair->RemoveFromParent();
	this->Crosshair = nullptr;;
	this->InteractionInputIcon->RemoveFromParent();
	this->InteractionInputIcon = nullptr;

	Super::NativeDestruct();
}


