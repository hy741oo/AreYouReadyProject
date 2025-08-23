// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AYRPlayerCharacter.h"

#include "MainLevelCharacter.generated.h"

struct FInputActionInstance;
class UAYRCameraComponent;
class IInteractableObjectInterface;
class UGeneralStateMachineComponent;
struct FGeneralStateMachineNode;
class UPlayerHUD;

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API AMainLevelCharacter : public AAYRPlayerCharacter
{
	GENERATED_BODY()

private:
	// HUD
	UPlayerHUD* PlayerHUD = nullptr;
	
	// Camera更新回调句柄。
	FDelegateHandle OnPlayerCameraManagerUpdatedHandle;

private:
	// 当PlayerCameraManager更新时调用。用于处理依赖于Camera的业务逻辑，防止因为业务逻辑更新在Camera更新之前造成的不匹配问题。
	void OnPlayerCameraManagerUpdated();

protected:
	// 玩家摄像机。
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UAYRCameraComponent* PlayerCamera = nullptr;

	// 球形射线追踪距离。
	float SphereTraceDistance = 200.f;

	// 球形射线半径。
	float SphereTraceRadius = 10.f;

	// 当前可交互的对象。
	TWeakObjectPtr<AActor> InteractableActor;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////状态机相关Begin
protected:
	// 状态机初始化。
	void InitializeGeneralStateMachine();

	void OnEnterNormalSpeedState();

	void OnEnterHighSpeedState();

	void OnEnterJumpState();

	void OnEnterIdleState();
	
public:
	// 运动状态机。
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UGeneralStateMachineComponent* MovementStateMachineComponent = nullptr;

	// 移动数据状态机。
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UGeneralStateMachineComponent* MovementDataStateMachineComponent = nullptr;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////状态机相关End

public:
	// 构造函数
	AMainLevelCharacter(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	// 执行绑定运动等操作。
	virtual void BeginPlay() override;

	// 销毁时的清理操作。
	virtual void Destroyed() override;

	// Tick函数。包含交互物检测等逻辑。
	virtual void Tick(float InDeltaTime) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////状态机相关Begin
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
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////状态机相关End

	// 玩家落地。
	virtual void Landed(const FHitResult& Hit) override;

	// 交互逻辑。
	virtual void Interact(const FInputActionInstance& InValue);
};
