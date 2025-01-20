// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AYRGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UAYRGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
protected:
	virtual void OnStart() override;
};
