// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AYRCharacter.h"
#include "AYRPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API AAYRPlayerCharacter : public AAYRCharacter
{
	GENERATED_BODY()
	

public:
	UFUNCTION(BlueprintCallable, Category = "AYRCamera")
	// 启用摄相机抖动。
	virtual void StartCameraShake(const FName& InCameraShakeID);

	UFUNCTION(BlueprintCallable, Category = "AYRCamera")
	// 停止摄相机抖动。
	virtual void StopCameraShake(const FName& InCameraShakedID);
};
