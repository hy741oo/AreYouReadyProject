// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameConfigSubsystem.h"
#include "AYRInputProcessor.h"

#include "AYRPlayerController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAYRPlayerController, Log, All);

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API AAYRPlayerController : public APlayerController
{
	GENERATED_BODY()
	
private:
	// 控制器需要用到的结构体。
	FPlayerControllerInfoTableRow* PlayerControllerInfoTableRow = nullptr;

public:
	// 玩家控制器唯一ID。
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FName ID;

public:
	AAYRPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

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
	AAYRPlayerCameraManager* GetPlayerCameraManager() const;
};
