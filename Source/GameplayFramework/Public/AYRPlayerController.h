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
	// ��������Ҫ�õ��Ľṹ�塣
	FPlayerControllerInfoTableRow* PlayerControllerInfoTableRow;

public:
	// ��ҿ�����ΨһID��
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
