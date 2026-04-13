// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGeneralStateMachine, Log, All);

DECLARE_DELEGATE_RetVal(bool, FGeneralStateMachineConditionCheckDelegate);

DECLARE_DELEGATE_TwoParams(FOnStateTickedDelegate, const float& /* InDeltaTime */, const float& /* InTickingElapsedTime*/);

struct FGeneralStateMachineNode;

/**
 * 状态机条件节点。即现态转换到次态所需的条件（Condition）。
 */
struct FGeneralStateMachineCondition
{
	// 记录着可以转换的次态。
	FName NextStateName;

	// 转换次态时需要判断的条件，条件满足即可转换状态，不满足的话状态机不会做任何变动。如果用户没有绑定对应的事件，则认为该此转换无条件通过。
	FGeneralStateMachineConditionCheckDelegate Condition;

	// 条件判断成功之后，或者是无条件进行的状态切换之后执行的动作，可为空。
	FSimpleDelegate PassAction;

	// 如果设置了Condition，且Condition判断没有通过，则执行该动作。
	FSimpleDelegate NoPassAction;
};

/**
 * 状态机节点。每一个节点代表了一种状态（例如行走、跳跃、射击等）。
 * 节点包含了现态可以转换的次态。
 */
struct FGeneralStateMachineNode
{
	// 可以转换的次态。
	TMap<FName, FGeneralStateMachineCondition> NextStates;

	// 进入到该状态时执行的逻辑。
	FSimpleDelegate OnEnterState;

	// 离开该状态时执行的逻辑。
	FSimpleDelegate OnLeaveState;

	// 如果状态转换是在自发生时（即“奔跑状态”切换到“奔跑状态”），不执行OnEnterState逻辑，而是执行更新逻辑。
	FSimpleDelegate OnUpdateState;

	// 状态的Tick，只要进入过该状态，哪怕不是自发生状态也依然会每帧调用。
	FOnStateTickedDelegate OnTickState;

	// 节点名称。
	FName NodeName;
};

/**
 通用状态机类。封装了通用状态机最核心的功能。
 */
class GAMEPLAYFRAMEWORK_API OSGeneralStateMachine
{

private:
	// 当前状态。即“现态”。
	FName CurrentState;

	// 已经生成的全部状态映射。P
	TMap<FName, FGeneralStateMachineNode> CreatedStates;

	// 用于记录当前状态已经持续了多长时间。
	float TickingElapsedTime = .0f;

	// 注册Tick函数后的句柄。
	FDelegateHandle TickDelegateHandle;

public:	
	/*
	只保留默认构造函数。
	*/
	// 默认构造函数。
	OSGeneralStateMachine() = default;
	// 析构函数。
	virtual ~OSGeneralStateMachine();
	// 拷贝构造函数。
	OSGeneralStateMachine(const OSGeneralStateMachine&) = delete;
	// 移动构造函数。
	OSGeneralStateMachine(OSGeneralStateMachine&&) = delete;
	// 拷贝赋值运算符。
	OSGeneralStateMachine& operator=(const OSGeneralStateMachine&) = delete;
	// 移动复制运算符。
	OSGeneralStateMachine& operator=(OSGeneralStateMachine&&) = delete;

	// 自身的Tick函数。
	bool Tick(float InDeltaTime);

	// 创建状态机节点。
	void CreateStateMachineNode(const FName& InNodeName);

	/*
	设置状态节点执行不同步骤时要调用的委托。例如进入某个状态时要调用某个函数，离开某个状态时又要调用另一个函数。
	*/
	// 设置进入状态节点时调用的委托。
	void SetStateMachineNodeOnEnterStateDelegate(const FName& InNodeName, FSimpleDelegate InOnEnterStateDelegate);
	// 设置离开状态节点时调用的委托。
	void SetStateMachineNodeOnLeaveStateDelegate(const FName& InNodeName, FSimpleDelegate InOnLeaveStateDelegate);
	// 设置自更新状态节点时调用的委托。
	void SetStateMachineNodeOnUpdateStateDelegate(const FName& InNodeName, FSimpleDelegate InOnUpdateStateDelegate);
	// 设置Tick更新状态节点时调用的委托。
	void SetStateMachineNodeOnTickStateDelegate(const FName& InNodeName, FOnStateTickedDelegate InOnTickStateDelegate);

	/*
	设置切换状态时的条件节点。用于设置状态可以切换到哪些状态，还用于判断状态切换是否需要执行通过判定，以及设置判定通过和判定不通过时分别要执行哪些操作。
	*/
	// 设置状态InCurrentStateName可以切换到状态InNextStateName。
	// InConditionCheck：对状态切换进行判定，只有当委托返回的bool值为true才进行状态切换，返回false时不进行切换。
	// InPassAction：状态切换进行判定时如果通过判定则调用该委托。
	// InNoPassAction：状态切换进行判定时如果没有通过则调用该委托。
	void SetStateMachineCondition(const FName& InCurrentStateName, const FName& InNextStateName, FGeneralStateMachineConditionCheckDelegate InConditionCheck = FGeneralStateMachineConditionCheckDelegate(), FSimpleDelegate InPassAction = FSimpleDelegate(), FSimpleDelegate InNoPassAction = FSimpleDelegate());
	// 设置状态InCurrentStateName可以切换到状态InNextStateName，不需要任何判定，如何传入了InAction则会在切换状态时进行调用。可以实现例如玩家从空手状态切换到持枪状态时播放一个切换的音效。
	void SetStateMachineCondition(const FName& InCurrentStateName, const FName& InNextStateName, FSimpleDelegate InAction = FSimpleDelegate());

	// 初始化状态机，状态机在启用之前必须指定一个状态用于“最初的状态”。
	void InitGeneralStateMachine(const FName& InInitState);

	// 切换状态。
	bool ChangeStateTo(const FName& InStateChangeTo);

	// 检测传入的状态是否是现态。
	bool IsSameState(const FName& InState) const;

	// 检测是否可以切换状态。
	bool CanChangeToState(const FName& InNewState) const;
};
