// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "OSPlayerCameraManager.generated.h"

USTRUCT(BlueprintType)
struct FCameraShakeInfoTableRow : public FAYRTableRowBase
{
	GENERATED_BODY()

	// 摄相机抖动类。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<UCameraShakeBase> CameraShakeClass = nullptr;

	// 抖动系数。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float Scale = 1.f;

	// 是否立即停止。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bStopImmediately = false;
};

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API AOSPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
protected:
	// 获取CameraShakeID指定的抖动Info蓝图版本。
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "OSPlayerCameraManager")
	bool GetCameraShakeInfo(const FName& InCameraShakeID, FCameraShakeInfoTableRow& OutCameraShakeInfo);

public:
	// 启用摄像机抖动。
	UFUNCTION(BlueprintCallable, Category = "OSPlayerCameraManager")
	void StartCameraShakeByID(const FName& InCameraShakeID, ECameraShakePlaySpace InCameraShakePlaySpace = ECameraShakePlaySpace::CameraLocal, FRotator InUserPlaySpaceRot = FRotator::ZeroRotator);

	// 停止摄像机抖动。
	UFUNCTION(BlueprintCallable, Category = "OSPlayerCameraManager")
	void StopCameraShakeByID(const FName& InCameraShakeID);

	// 获取CameraShakeID指定的抖动Info。
	bool GetCameraShakeInfo(const FName& InCameraShakeID, FCameraShakeInfoTableRow*& OutCameraShakeInfo);
};
