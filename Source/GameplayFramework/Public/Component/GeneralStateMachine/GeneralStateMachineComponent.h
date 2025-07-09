// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GeneralStateMachineComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGeneralStateMachineComponent, Log, All);

DECLARE_DELEGATE_RetVal(bool, FGeneralStateMachineConditionCheck);

struct FGeneralStateMachineNode;

/**
 * 状态机条件节点。即现态转换到次态所需的条件（Condition）。
 */
struct FGeneralStateMachineCondition
{
	// 记录着可以转换的次态。
	TSharedPtr<FGeneralStateMachineNode> NextState;

	// 转换次态时需要判断的条件，条件满足即可转换状态，不满足的话状态机不会做任何变动。
	FGeneralStateMachineConditionCheck Condition;

	// 条件判断成功之后执行的动作，可为空。
	FSimpleDelegate Action;
};

/**
 * 状态机节点。每一个节点代表了一种状态（例如行走、跳跃、射击等）。
 * 节点包含了现态可以转换的次态。
 */
struct FGeneralStateMachineNode
{
	// 可以转换的次态。
	TMap<TSharedPtr<FGeneralStateMachineNode>, FGeneralStateMachineCondition> NextStates;

	// 当进入到该状态时执行的逻辑。
	FSimpleDelegate OnActivatedNode;

	// 节点名称。
	FName NodeName;

	FGeneralStateMachineNode() = default;

	FGeneralStateMachineNode(const FGeneralStateMachineNode&) = delete;
	FGeneralStateMachineNode& operator=(const FGeneralStateMachineNode&) = delete;

};

/**
 * 状态机组件。用于实现一个通用状态机。 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMEPLAYFRAMEWORK_API UGeneralStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	// 当前状态。即“现态”。
	TSharedPtr<FGeneralStateMachineNode> CurrentState;

public:	
	// Sets default values for this component's properties
	UGeneralStateMachineComponent(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

public:	
	// Called every frame
	virtual void TickComponent(float InDeltaTime, ELevelTick InTickType, FActorComponentTickFunction* InThisTickFunction) override;

	// 创建状态机节点。
	TSharedPtr<FGeneralStateMachineNode> CreateStateMachineNode(const FName& InNodeName);

	// 初始化状态机，状态机在启用之前必须指定一个状态用于“最初的状态”。
	void InitGeneralStateMachine(const TSharedPtr<FGeneralStateMachineNode>& InInitState);

	// 切换状态。
	bool ChangeStateTo(const TSharedPtr<FGeneralStateMachineNode>& InStateChangeTo);

	// 检测传入的状态是否是现态。
	bool IsSameState(const TSharedPtr<FGeneralStateMachineNode>& InState) const;
};
