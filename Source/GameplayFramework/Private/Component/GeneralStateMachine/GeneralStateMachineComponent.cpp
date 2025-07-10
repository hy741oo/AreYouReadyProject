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

	// ...
}

FGeneralStateMachineNode& UGeneralStateMachineComponent::CreateStateMachineNode(const FName& InNodeName)
{
	// 如果传进的NodeName已经被生成了，则返回之前已经生成的节点。
	FGeneralStateMachineNode& Node = this->CreatedStates.FindOrAdd(InNodeName);
	Node.NodeName = InNodeName;

	return Node;
}

void UGeneralStateMachineComponent::InitGeneralStateMachine(const FName& InInitState)
{
	this->CurrentState = InInitState;
	if (FGeneralStateMachineNode* Node = this->CreatedStates.Find(InInitState))
	{
		Node->OnEnterState.ExecuteIfBound();
	}
	else
	{
		UE_LOG(LogGeneralStateMachineComponent, Warning, TEXT("Init general state machine failed, InitState:\"%s\" is no valid."), *InInitState.ToString());
	}
}

bool UGeneralStateMachineComponent::ChangeStateTo(const FName& InStateChangeTo)
{
	UE_LOG(LogGeneralStateMachineComponent, Verbose, TEXT("Changing State...current state:%s, next state:%s"), *this->CurrentState.ToString(), *InStateChangeTo.ToString());

	bool bSuccessful = this->CanChangeToState(InStateChangeTo);

	if (bSuccessful)
	{
		FGeneralStateMachineNode& OldNode = this->CreatedStates[this->CurrentState];
		if (this->CurrentState == InStateChangeTo)
		{
			OldNode.OnUpdateState.ExecuteIfBound();
		}
		else
		{
			FGeneralStateMachineNode& NewNode = this->CreatedStates[InStateChangeTo];
			OldNode.OnLeaveState.ExecuteIfBound();
			NewNode.OnEnterState.ExecuteIfBound();
			this->CurrentState = InStateChangeTo;
		}
	}

	return bSuccessful;
}

bool UGeneralStateMachineComponent::IsSameState(const FName& InState) const
{
	return this->CurrentState == InState;
}

bool UGeneralStateMachineComponent::CanChangeToState(const FName& InNewState) const
{
	bool bSuccessful = false;

	// 检测新状态是否已经生成。
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
					bSuccessful = true;
				}

				if (bSuccessful)
				{
					UE_LOG(LogGeneralStateMachineComponent, Verbose, TEXT("Changing success, now current state become:%s"), *this->CurrentState.ToString());
				}
				else
				{
					UE_LOG(LogGeneralStateMachineComponent, Verbose, TEXT("Changing failed, the condition of next state:%s did not pass."), *this->CurrentState.ToString(), *InNewState.ToString());
				}
			}
			else
			{
				UE_LOG(LogGeneralStateMachineComponent, Verbose, TEXT("Changing state failed, current state:%s has not next state:%s."), *this->CurrentState.ToString(), *InNewState.ToString());
			}
		}
	}
	else
	{
		UE_LOG(LogGeneralStateMachineComponent, Verbose, TEXT("Changing state failed, current state:%s has not next state:%s."), *this->CurrentState.ToString(), *InNewState.ToString());
	}

	return bSuccessful;
}

