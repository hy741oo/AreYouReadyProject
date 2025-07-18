// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AYRCharacter.h"

#include "MainLevelCharacter.generated.h"

struct FInputActionInstance;
class UCameraComponent;
class IInteractableObjectInterface;
class UGeneralStateMachineComponent;
struct FGeneralStateMachineNode;

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API AMainLevelCharacter : public AAYRCharacter
{
	GENERATED_BODY()

protected:
	// 玩家摄像机。
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UCameraComponent* PlayerCamera = nullptr;

	// 球形射线追踪距离。
	float SphereTraceDistance = 150.f;

	// 球形射线半径。
	float SphereTraceRadius = 10.f;

	// 当前可交互的对象。
	TWeakObjectPtr<AActor> InteractableActor;

	/* 状态机相关--------------------Begin*/
protected:
	// 状态机初始化。
	void InitializeGeneralStateMachine();

	void OnEnterNormalSpeedState();

	void OnEnterHighSpeedState();

	void OnEnterJumpState();
	/* 状态机相关--------------------End*/
	
public:
	// 运动状态机。
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UGeneralStateMachineComponent* MovementStateMachineComponent = nullptr;

	// 移动数据状态机。
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UGeneralStateMachineComponent* MovementDataStateMachineComponent = nullptr;

public:
	// 构造函数
	AMainLevelCharacter(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	// 执行绑定运动等操作。
	virtual void BeginPlay() override;

	// 结束时的清理操作。
	virtual void EndPlay(const EEndPlayReason::Type InEndPlayReason) override;

	// Tick函数。包含交互物检测等逻辑。
	virtual void Tick(float InDeltaTime) override;

	// 移动逻辑。
	virtual void Move(const FInputActionInstance& InValue);

	// 移动停止时调用。
	virtual void OnMoveStopped(const FInputActionInstance& InValue);

	// 旋转摄像机逻辑。
	virtual void LookAround(const FInputActionInstance& InValue);

	// 奔跑。
	virtual void Run(const FInputActionInstance& InValue);

	// 停止奔跑。
	virtual void OnRunStopped(const FInputActionInstance& InValue);

	// 跳跃。
	virtual void CharacterJump(const FInputActionInstance& InValue);

	// 停止跳跃。
	virtual void OnCharacterJumpStopped(const FInputActionInstance& InValue);

	// 玩家落地。
	virtual void Landed(const FHitResult& Hit) override;
};
