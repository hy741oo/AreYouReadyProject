// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AYRCharacter.h"

#include "MainLevelCharacter.generated.h"

struct FInputActionInstance;
class UCameraComponent;

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API AMainLevelCharacter : public AAYRCharacter
{
	GENERATED_BODY()
	
protected:
	// 玩家摄像机。
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UCameraComponent* PlayerCamera = nullptr;

	// 射线追踪距离。
	float LineTraceDistance = 150.f;

public:
	// 构造函数
	AMainLevelCharacter(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	// 执行绑定运动等操作。
	virtual void BeginPlay() override;

	// 前后移动逻辑。
	virtual void MoveForward(const FInputActionInstance& InValue);

	// 左右移动逻辑。
	virtual void MoveRight(const FInputActionInstance& InValue);

	// 左右移动摄像机逻辑。
	virtual void LookAround(const FInputActionInstance& InValue);

	// 上下移动摄像机逻辑。
	virtual void LookUp(const FInputActionInstance& InValue);

	// Tick函数。包含交互物检测等逻辑。
	virtual void Tick(float InDeltaTime) override;
};
