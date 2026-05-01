// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Gameplay/OSPlayerController.h"

#include "MainMenuPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API AMainMenuPlayerController : public AOSPlayerController
{
	GENERATED_BODY()
	
public:
	AMainMenuPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
