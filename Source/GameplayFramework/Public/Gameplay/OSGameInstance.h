// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Input/OSInputProcessor.h"

#include "OSGameInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOSGameInstance, Log, All);

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UOSGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
private:
	TSharedPtr<class FOSInputProcessor> InputProcessor;

protected:
	virtual void OnStart() override;

	virtual void Shutdown() override;

public:
	EInputDeviceType::Type GetCurrentInputDeviceType() const;
};
