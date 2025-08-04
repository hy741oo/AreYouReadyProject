// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainLevel/PlayerHUD.h"

#include "Components/CanvasPanelSlot.h"


void UPlayerHUD::SetCrosshairPositionInScreen(const FVector2D& InPosition)
{
	if (this->Crosshair)
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(this->Crosshair->Slot);
		CanvasSlot->SetPosition(InPosition);
	}
}


