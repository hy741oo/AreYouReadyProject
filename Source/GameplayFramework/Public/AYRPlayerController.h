// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameConfigSubsystem.h"
#include "AYRInputProcessor.h"

#include "AYRPlayerController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAYRPlayerController, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerControllerInputDeviceChangedDelegate, EInputDeviceType::Type, CurrentInputDeviceType);

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

	// 玩家输入设备更改时的委托。
	UPROPERTY(BlueprintAssignable)
	FOnPlayerControllerInputDeviceChangedDelegate OnPlayerControllerInputDeviceChangedDelegate;

public:
	AAYRPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FPlayerControllerInfoTableRow GetPlayerControllerInfo()
	{
		ensureAlways(this->PlayerControllerInfoTableRow);
		return this->PlayerControllerInfoTableRow ? *this->PlayerControllerInfoTableRow : FPlayerControllerInfoTableRow();
	}
};
