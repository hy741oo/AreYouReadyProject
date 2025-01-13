// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AYRGameModeBase.h"
#include "LoadingScreenGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API ALoadingScreenGameMode : public AAYRGameModeBase
{
	GENERATED_BODY()

public:
	ALoadingScreenGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
};
