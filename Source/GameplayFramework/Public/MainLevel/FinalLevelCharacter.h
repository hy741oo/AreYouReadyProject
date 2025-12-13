// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainLevel/MainLevelCharacter.h"
#include "FinalLevelCharacter.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API AFinalLevelCharacter : public AMainLevelCharacter
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
};
