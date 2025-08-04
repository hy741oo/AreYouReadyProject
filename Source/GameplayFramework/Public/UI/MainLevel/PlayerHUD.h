// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/AYRUserWidget.h"
#include "PlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UPlayerHUD : public UAYRUserWidget
{
	GENERATED_BODY()
	
protected:
	// 准星Widget。
	UPROPERTY(Meta = (BindWidget))
	UUserWidget* Crosshair = nullptr;

public:
	void SetCrosshairPositionInScreen(const FVector2D& InPosition);
};
