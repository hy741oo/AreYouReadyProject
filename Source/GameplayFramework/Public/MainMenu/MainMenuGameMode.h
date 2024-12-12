// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AYRGameModeBase.h"

#include "MainMenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API AMainMenuGameMode : public AAYRGameModeBase
{
	GENERATED_BODY()
	
public:
	AMainMenuGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

};
