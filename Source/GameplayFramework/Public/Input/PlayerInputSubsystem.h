// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PlayerInputSubsystem.generated.h"

class UInputAction;
class UInputMappingContext;

/**
 * 结合增强输入（Enhanced Input）系统实现Game和UI层级的输入管理。
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UPlayerInputSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void BindActionTest(APlayerController* InPlayerController, UInputAction* InInputAction, UInputMappingContext* InInputMappingContext);

	void TestFunc();
};
