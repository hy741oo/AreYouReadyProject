// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GeneralStateMachine.h"
#include "GeneralStateMachineComponent.generated.h"


DECLARE_DYNAMIC_DELEGATE(FSimpleDynamicDelegate);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnStateTickedDynamicDelegate, const float, InDeltaTime, const float, InTickingElapsedTime);
DECLARE_DYNAMIC_DELEGATE_RetVal(bool, FGeneralStateMachineConditionCheckDynamicDelegate);


/**
 * 状态机组件。封装了状态机的Actor组件。 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMEPLAYFRAMEWORK_API UGeneralStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	// 通用状态机。
	OSGeneralStateMachine StateMachine;

public:	
	// 初始化状态机，状态机在启用之前必须指定一个状态用于“最初的状态”。
	UFUNCTION(BlueprintCallable, Category = "General State Machine")
	void InitGeneralStateMachine(const FName InInitState);

	// 切换状态。
	UFUNCTION(BlueprintCallable, Category = "General State Machine")
	bool ChangeStateTo(const FName InStateChangeTo);

	// 检测传入的状态是否是现态。
	UFUNCTION(BlueprintCallable, Category = "General State Machine")
	bool IsSameState(const FName InState) const;

	// 检测是否可以切换状态。
	UFUNCTION(BlueprintCallable, Category = "General State Machine")
	bool CanChangeToState(const FName InNewState) const;

	/**
	 * 蓝图接口。由于蓝图不能使用结构体的函数，因此需要把CPP接口封装一套供蓝图使用。
	 */

	// 创建状态机节点。
	UFUNCTION(BlueprintCallable, Category = "General State Machine")
	void CreateStateMachineNode(const FName InNodeName);

protected:
	// 绑定进入状态节点时的委托。
	UFUNCTION(BlueprintCallable, Category = "General State Machine")
	void SetStateMachineNodeOnEnterStateDelegate(const FName InNodeName, FSimpleDynamicDelegate InOnEnterStateDelegate);
	// 绑定离开状态节点时的委托。
	UFUNCTION(BlueprintCallable, Category = "General State Machine")
	void SetStateMachineNodeOnLeaveStateDelegate(const FName InNodeName, FSimpleDynamicDelegate InOnLeaveStateDelegate);
	// 绑定自更新状态节点时的委托。
	UFUNCTION(BlueprintCallable, Category = "General State Machine")
	void SetStateMachineNodeOnUpdateStateDelegate(const FName InNodeName, FSimpleDynamicDelegate InOnUpdateStateDelegate);
	// 绑定Tick更新状态节点时的委托。
	UFUNCTION(BlueprintCallable, Category = "General State Machine")
	void SetStateMachineNodeOnTickStateDelegate(const FName InNodeName, FOnStateTickedDynamicDelegate InOnTickStateDelegate);

	// 添加状态机条件节点，由于蓝图不能省略函数，因此这里分成多种类型的接口。
	UFUNCTION(BlueprintCallable, Category = "General State Machine")
	void SetStateMachineConditionWithCheckAndPassOrNoPassAction(const FName InCurrentStateName, const FName InNextStateName, FGeneralStateMachineConditionCheckDynamicDelegate InConditionCheck, FSimpleDynamicDelegate InPassAction, FSimpleDynamicDelegate InNoPassAction);
	UFUNCTION(BlueprintCallable, Category = "General State Machine")
	void SetStateMachineConditionWithCheckAndAction(const FName InCurrentStateName, const FName InNextStateName, FGeneralStateMachineConditionCheckDynamicDelegate InConditionCheck, FSimpleDynamicDelegate InAction);
	UFUNCTION(BlueprintCallable, Category = "General State Machine")
	void SetStateMachineConditionWithCheck(const FName InCurrentStateName, const FName InNextStateName, FGeneralStateMachineConditionCheckDynamicDelegate InConditionCheck);
	UFUNCTION(BlueprintCallable, Category = "General State Machine")
	void SetStateMachineConditionWithAction(const FName InCurrentStateName, const FName InNextStateName, FSimpleDynamicDelegate InAction);
	UFUNCTION(BlueprintCallable, Category = "General State Machine", Meta = (DisplayName = "Set State Machine Condition"))
	void SetStateMachineConditionDynamic(const FName InCurrentStateName, const FName InNextStateName);

public:
	/*
	C++代码可访问的接口。作用是让用户在C++代码里直接访问该版本可提高一些性能。（应该把……）。
	*/
	void SetStateMachineCondition(const FName InCurrentStateName, const FName InNextStateName, FGeneralStateMachineConditionCheckDelegate InConditionCheck = FGeneralStateMachineConditionCheckDelegate(), FSimpleDelegate InPassAction = FSimpleDelegate(), FSimpleDelegate InNoPassAction = FSimpleDelegate());

	// 绑定进入状态节点时的委托。
	void SetStateMachineNodeOnEnterStateDelegate(const FName InNodeName, FSimpleDelegate InOnEnterStateDelegate);
	// 绑定离开状态节点时的委托。
	void SetStateMachineNodeOnLeaveStateDelegate(const FName InNodeName, FSimpleDelegate InOnLeaveStateDelegate);
	// 绑定自更新状态节点时的委托。
	void SetStateMachineNodeOnUpdateStateDelegate(const FName InNodeName, FSimpleDelegate InOnUpdateStateDelegate);
	// 绑定Tick更新状态节点时的委托。
	void SetStateMachineNodeOnTickStateDelegate(const FName InNodeName, FOnStateTickedDelegate InOnTickStateDelegate);
};
