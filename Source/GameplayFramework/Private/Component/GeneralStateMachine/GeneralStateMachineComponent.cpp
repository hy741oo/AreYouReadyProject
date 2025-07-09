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

TSharedPtr<FGeneralStateMachineNode> UGeneralStateMachineComponent::CreateStateMachineNode(const FName& InNodeName)
{
	TSharedPtr<FGeneralStateMachineNode> Node = MakeShared<FGeneralStateMachineNode>();
	Node->NodeName = InNodeName;

	return Node;
}

void UGeneralStateMachineComponent::InitGeneralStateMachine(const TSharedPtr<FGeneralStateMachineNode>& InInitState)
{
	this->CurrentState = InInitState;
	if (this->CurrentState.IsValid())
	{
		this->CurrentState->OnActivatedNode.ExecuteIfBound();
	}
}

bool UGeneralStateMachineComponent::ChangeStateTo(const TSharedPtr<FGeneralStateMachineNode>& InStateChangeTo)
{
	UE_LOG(LogGeneralStateMachineComponent, Verbose, TEXT("Changing State...current state:%s, next state:%s"),*this->CurrentState->NodeName.ToString(), *InStateChangeTo->NodeName.ToString());

	bool bSuccessful = false;

	if (this->CurrentState->NextStates.Contains(InStateChangeTo))
	{
		FGeneralStateMachineCondition& NextState = this->CurrentState->NextStates[InStateChangeTo];
		if (NextState.Condition.IsBound())
		{
			bSuccessful = NextState.Condition.Execute();
		}

		if (bSuccessful)
		{
			// 当“现态”和“次态”不同（例如从“行走”状态转换到“奔跑”状态）时，需要执行“次态”的激活事件，同时将“次态”替换为“现态”。
			if (this->CurrentState != InStateChangeTo)
			{
				NextState.Action.ExecuteIfBound();
				this->CurrentState = InStateChangeTo;
			}

			UE_LOG(LogGeneralStateMachineComponent, Verbose, TEXT("Changing success, now current state become:%s"), *this->CurrentState->NodeName.ToString());
		}
		else
		{
			UE_LOG(LogGeneralStateMachineComponent, Verbose, TEXT("Changing failed, the condition of next state:%s did not pass."), *this->CurrentState->NodeName.ToString(), *InStateChangeTo->NodeName.ToString());
		}
	}
	else
	{
		UE_LOG(LogGeneralStateMachineComponent, Verbose, TEXT("Changing failed, current state:%s has not next state:%s."), *this->CurrentState->NodeName.ToString(), *InStateChangeTo->NodeName.ToString());
	}


	return bSuccessful;
}

bool UGeneralStateMachineComponent::IsSameState(const TSharedPtr<FGeneralStateMachineNode>& InState) const
{
	return this->CurrentState == InState;
}

