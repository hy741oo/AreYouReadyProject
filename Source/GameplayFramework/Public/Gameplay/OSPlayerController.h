// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameConfigSubsystem.h"
#include "Input/OSInputProcessor.h"

#include "OSPlayerController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOSPlayerController, Log, All);

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API AOSPlayerController : public APlayerController
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE(FOnPlayerCameraManagerUpdatedDelegate);
	
private:
	// 控制器需要用到的结构体。
	FPlayerControllerInfoTableRow* PlayerControllerInfoTableRow = nullptr;

public:
	// 玩家控制器唯一ID。
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FName ID;

	// 当PlayerCameraManager更新时调用。
	FOnPlayerCameraManagerUpdatedDelegate OnPlayerCameraManagerUpdateDelegate;

public:
	AOSPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	// 清理该Controller生成的所有UI。
	virtual void CleanupGameViewport() override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FPlayerControllerInfoTableRow GetPlayerControllerInfo()
	{
		ensureAlways(this->PlayerControllerInfoTableRow);
		return this->PlayerControllerInfoTableRow ? *this->PlayerControllerInfoTableRow : FPlayerControllerInfoTableRow();
	}

	UFUNCTION(BlueprintCallable, Category = "AYRCamera")
	AOSPlayerCameraManager* GetPlayerCameraManager() const;

	virtual void UpdateCameraManager(float InDeltaTime) override;
};
