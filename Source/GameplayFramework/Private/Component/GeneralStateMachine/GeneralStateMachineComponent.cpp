// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/GeneralStateMachine/GeneralStateMachineComponent.h"

DEFINE_LOG_CATEGORY(LogGeneralStateMachineComponent);

// Sets default values for this component's properties
UGeneralStateMachineComponent::UGeneralStateMachineComponent(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called every frame
void UGeneralStateMachineComponent::TickComponent(float InDeltaTime, ELevelTick InTickType, FActorComponentTickFunction* InThisTickFunction)
{
	Super::TickComponent(InDeltaTime, InTickType, InThisTickFunction);

	if (FGeneralStateMachineNode* Node = this->CreatedStates.Find(this->CurrentState))
	{
		this->TickingElapsedTime += InDeltaTime;
		Node->OnTickState.ExecuteIfBound(InDeltaTime, TickingElapsedTime);
	}
}

FGeneralStateMachineNode& UGeneralStateMachineComponent::CreateStateMachineNode(const FName InNodeName)
{
	// 如果传进的NodeName已经被生成了，则返回之前已经生成的节点。
	FGeneralStateMachineNode& Node = this->CreatedStates.FindOrAdd(InNodeName);
	Node.NodeName = InNodeName;

	return Node;
}

void UGeneralStateMachineComponent::InitGeneralStateMachine(const FName InInitState)
{
	// 如果状态已初始化（即CurrentState不为NAME_None，则不执行任何操作。
	if (!this->CurrentState.IsNone())
	{
		return;
	}

	if (FGeneralStateMachineNode* Node = this->CreatedStates.Find(InInitState))
	{
		this->CurrentState = InInitState;
		Node->OnEnterState.ExecuteIfBound();
	}
	else
	{
		UE_LOG(LogGeneralStateMachineComponent, Warning, TEXT("\"%s\" Init general state machine failed, InitState:\"%s\" is no valid."), *this->GetFName().ToString(), *InInitState.ToString());
	}
}

bool UGeneralStateMachineComponent::ChangeStateTo(const FName InStateChangeTo)
{
	UE_LOG(LogGeneralStateMachineComponent, Verbose, TEXT("\"%s\" Changing State...current state:\"%s\", next state:\"%s\"."), *this->GetFName().ToString(), *this->CurrentState.ToString(), *InStateChangeTo.ToString());

	bool bSuccessful = this->CanChangeToState(InStateChangeTo);

	if (bSuccessful)
	{
		FGeneralStateMachineNode& OldNode = this->CreatedStates[this->CurrentState];

		// 转换成功代表着ConditionCheck通过，需要执行一次Action。
		const FGeneralStateMachineCondition& NextStateCondition = OldNode.NextStates.FindRef(InStateChangeTo);
		if (NextStateCondition.Action.IsBound())
		{
			NextStateCondition.Action.Execute();
		}

		// 如果状态变更为自更新状态，则执行Update委托，否则进入常规流程。
		if (this->CurrentState == InStateChangeTo)
		{
			OldNode.OnUpdateState.ExecuteIfBound();
		}
		else
		{
			FGeneralStateMachineNode& NewNode = this->CreatedStates[InStateChangeTo];
			this->CurrentState = InStateChangeTo;
			this->TickingElapsedTime = .0f;

			OldNode.OnLeaveState.ExecuteIfBound();
			NewNode.OnEnterState.ExecuteIfBound();
		}

	}

	return bSuccessful;
}

bool UGeneralStateMachineComponent::IsSameState(const FName InState) const
{
	return this->CurrentState == InState;
}

bool UGeneralStateMachineComponent::CanChangeToState(const FName InNewState) const
{
	bool bSuccessful = false;

	// 检测新状态是否已经创建。
	if (const FGeneralStateMachineNode* NewStateNode = this->CreatedStates.Find(InNewState))
	{
		// 检测现态可以转换的次态是否包含新状态。
		if (const FGeneralStateMachineNode* CurrentStateNode = this->CreatedStates.Find(this->CurrentState))
		{
			if (const FGeneralStateMachineCondition* NextStateCondition = CurrentStateNode->NextStates.Find(InNewState))
			{
				if (NextStateCondition->Condition.IsBound())
				{
					bSuccessful = NextStateCondition->Condition.Execute();
				}
				else
				{
					// 如果Condition委托没有被绑定，默认此状态切换无条件成功。
					bSuccessful = true;
				}

				if (bSuccessful)
				{
					UE_LOG(LogGeneralStateMachineComponent, Verbose, TEXT("\"%s\" Changing success, now current state become:\"%s\"."), *this->GetFName().ToString(), *InNewState.ToString());
				}
				else
				{
					UE_LOG(LogGeneralStateMachineComponent, Verbose, TEXT("\"%s\" Changing failed, the condition of state \"%s\" to \"%s\" did not pass."), *this->GetFName().ToString(), *this->CurrentState.ToString(), *InNewState.ToString());
				}
			}
			else
			{
				UE_LOG(LogGeneralStateMachineComponent, Verbose, TEXT("\"%s\" Changing state failed, current state:\"%s\" has no next state:\"%s\"."), *this->GetFName().ToString(), *this->CurrentState.ToString(), *InNewState.ToString());
			}
		}
	}
	else
	{
		UE_LOG(LogGeneralStateMachineComponent, Verbose, TEXT("\"%s\" Changing state failed, new state:\"%s\" has not exist."), *this->GetFName().ToString(), *InNewState.ToString());
	}

	return bSuccessful;
}

void UGeneralStateMachineComponent::CreateStateMachineNodeBP(const FName InNodeName)
{
	this->CreateStateMachineNode(InNodeName);
}

void UGeneralStateMachineComponent::SetStateMachineNodeOnEnterStateDelegate(const FName InNodeName, FSimpleDynamicDelegate InOnEnterStateDelegate)
{
	if (FGeneralStateMachineNode* Node = this->CreatedStates.Find(InNodeName))
	{
		Node->OnEnterState.BindUFunction(InOnEnterStateDelegate.GetUObject(), InOnEnterStateDelegate.GetFunctionName());
	}
	else
	{
		UE_LOG(LogGeneralStateMachineComponent, Warning, TEXT("InNodeName is invalid on set delegate, abort setting."));
	}
}

void UGeneralStateMachineComponent::SetStateMachineNodeOnLeaveStateDelegate(const FName InNodeName, FSimpleDynamicDelegate InOnLeaveStateDelegate)
{
	if (FGeneralStateMachineNode* Node = this->CreatedStates.Find(InNodeName))
	{
		Node->OnLeaveState.BindUFunction(InOnLeaveStateDelegate.GetUObject(), InOnLeaveStateDelegate.GetFunctionName());
	}
	else
	{
		UE_LOG(LogGeneralStateMachineComponent, Warning, TEXT("InNodeName is invalid on set delegate, abort setting."));
	}
}

void UGeneralStateMachineComponent::SetStateMachineNodeOnUpdateStateDelegate(const FName InNodeName, FSimpleDynamicDelegate InOnUpdateStateDelegate)
{
	if (FGeneralStateMachineNode* Node = this->CreatedStates.Find(InNodeName))
	{
		Node->OnUpdateState.BindUFunction(InOnUpdateStateDelegate.GetUObject(), InOnUpdateStateDelegate.GetFunctionName());
	}
	else
	{
		UE_LOG(LogGeneralStateMachineComponent, Warning, TEXT("InNodeName is invalid on set delegate, abort setting."));
	}
}

void UGeneralStateMachineComponent::SetStateMachineNodeOnTickStateDelegate(const FName InNodeName, FOnStateTickedBP InOnTickStateDelegate)
{
	if (FGeneralStateMachineNode* Node = this->CreatedStates.Find(InNodeName))
	{
		Node->OnTickState.BindUFunction(InOnTickStateDelegate.GetUObject(), InOnTickStateDelegate.GetFunctionName());
	}
	else
	{
		UE_LOG(LogGeneralStateMachineComponent, Warning, TEXT("InNodeName is invalid on set delegate, abort setting."));
	}
}

void UGeneralStateMachineComponent::SetStateMachineConditionWithCheckAndAction(const FName InCurrentStateName, const FName InNextStateName, FGeneralStateMachineConditionCheckBP InConditionCheck, FSimpleDynamicDelegate InAction)
{
	if (FGeneralStateMachineNode* Node = this->CreatedStates.Find(InCurrentStateName))
	{
		FGeneralStateMachineCondition Condition;
		Condition.NextStateName = InNextStateName;
		if (InConditionCheck.IsBound())
		{
			Condition.Condition.BindUFunction(InConditionCheck.GetUObject(), InConditionCheck.GetFunctionName());
		}
		if (InAction.IsBound())
		{
			Condition.Action.BindUFunction(InAction.GetUObject(), InAction.GetFunctionName());
		}

		FGeneralStateMachineCondition& CurrentCondition = Node->NextStates.FindOrAdd(InNextStateName);
		CurrentCondition = Condition;
	}
	else
	{
		UE_LOG(LogGeneralStateMachineComponent, Warning, TEXT("InCurrentStateName is invalid on set condition, abort setting."));
	}
}

void UGeneralStateMachineComponent::SetStateMachineConditionWithCheck(const FName InCurrentStateName, const FName InNextStateName, FGeneralStateMachineConditionCheckBP InConditionCheck)
{
	this->SetStateMachineConditionWithCheckAndAction(InCurrentStateName, InNextStateName, InConditionCheck, FSimpleDynamicDelegate());
}

void UGeneralStateMachineComponent::SetStateMachineConditionWithAction(const FName InCurrentStateName, const FName InNextStateName, FSimpleDynamicDelegate InAction)
{
	this->SetStateMachineConditionWithCheckAndAction(InCurrentStateName, InNextStateName, FGeneralStateMachineConditionCheckBP(), InAction);
}

void UGeneralStateMachineComponent::SetStateMachineCondition(const FName InCurrentStateName, const FName InNextStateName)
{
	this->SetStateMachineConditionWithCheckAndAction(InCurrentStateName, InNextStateName, FGeneralStateMachineConditionCheckBP(), FSimpleDynamicDelegate());
}

