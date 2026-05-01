// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Gameplay/OSGameModeBase.h"

#include "MainMenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API AMainMenuGameMode : public AOSGameModeBase
{
	GENERATED_BODY()
	
public:
	AMainMenuGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

};
