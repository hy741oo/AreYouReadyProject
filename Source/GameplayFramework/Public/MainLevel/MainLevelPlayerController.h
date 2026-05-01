// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/OSPlayerController.h"
#include "MainLevelPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API AMainLevelPlayerController : public AOSPlayerController
{
	GENERATED_BODY()
	
public:
	AMainLevelPlayerController(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
};
