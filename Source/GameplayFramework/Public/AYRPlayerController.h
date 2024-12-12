// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ConfigSubsystem.h"

#include "AYRPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API AAYRPlayerController : public APlayerController
{
	GENERATED_BODY()
	
private:
	// 控制器需要用到的结构体。
	FPlayerControllerInfoTableRow* PlayerControllerInfoTableRow;

public:
	// 玩家控制器唯一ID。
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FName ID;

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
