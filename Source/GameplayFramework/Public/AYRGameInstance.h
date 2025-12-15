// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AYRInputProcessor.h"

#include "AYRGameInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAYRGameInstance, Log, All);

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UAYRGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
private:
	TSharedPtr<class FAYRInputProcessor> InputProcessor;

protected:
	virtual void Init() override;

	virtual void Shutdown() override;

public:
	EInputDeviceType::Type GetCurrentInputDeviceType() const;
};
