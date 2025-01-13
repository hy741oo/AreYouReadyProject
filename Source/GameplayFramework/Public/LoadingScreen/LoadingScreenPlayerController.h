// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AYRPlayerController.h"
#include "LoadingScreenPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API ALoadingScreenPlayerController : public AAYRPlayerController
{
	GENERATED_BODY()
	
public:
	ALoadingScreenPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
};
