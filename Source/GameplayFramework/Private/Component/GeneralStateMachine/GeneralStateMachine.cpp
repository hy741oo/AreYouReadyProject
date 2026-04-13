// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/GeneralStateMachine/GeneralStateMachine.h"
#include "Containers/Ticker.h"

DEFINE_LOG_CATEGORY(LogGeneralStateMachine);

OSGeneralStateMachine::~OSGeneralStateMachine()
{
	if (this->TickDelegateHandle.IsValid())
	{
		FTicker::GetCoreTicker().RemoveTicker(this->TickDelegateHandle);
	}
}

bool OSGeneralStateMachine::Tick(const float InDeltaTime)
{
	// 如果现态有绑定TickState事件，则返回true继续Tick，否则返回false结束tick，以此来提高性能。

	if (FGeneralStateMachineNode* Node = this->CreatedStates.Find(this->CurrentState))
	{
		if (Node->OnTickState.IsBound())
		{
			this->TickingElapsedTime += InDeltaTime;
			Node->OnTickState.Execute(InDeltaTime, TickingElapsedTime);
			return true;
		}
	}

	return false;
}

void OSGeneralStateMachine::CreateStateMachineNode(const FName& InNodeName)
{
	// 如果传进的NodeName已经被生成了，则不执行任何操作并报警告，否则生成状态节点。
	if (this->CreatedStates.Contains(InNodeName))
	{
		UE_LOG(LogGeneralStateMachine, Warning, TEXT("The state does already exist, state name:%s"), *InNodeName.ToString());
	}
	else
	{
		FGeneralStateMachineNode& Node = this->CreatedStates.FindOrAdd(InNodeName);
		Node.NodeName = InNodeName;
	}
}

void OSGeneralStateMachine::InitGeneralStateMachine(const FName& InInitState)
{
	// 如果状态已初始化（即CurrentState不为NAME_None）则不执行任何操作。
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
		UE_LOG(LogGeneralStateMachine, Warning, TEXT("Init general state machine failed, InitState:is no valid."), *InInitState.ToString());
	}
}

bool OSGeneralStateMachine::ChangeStateTo(const FName& InStateChangeTo)
{
	UE_LOG(LogGeneralStateMachine, Verbose, TEXT("Changing State...current state:\"%s\", next state:\"%s\"."), *this->CurrentState.ToString(), *InStateChangeTo.ToString());

	bool bSuccessful = this->CanChangeToState(InStateChangeTo);

	if (bSuccessful)
	{
		FGeneralStateMachineNode& OldNode = this->CreatedStates[this->CurrentState];

		// 转换成功代表着ConditionCheck通过，需要执行一次Action。
		const FGeneralStateMachineCondition& NextStateCondition = OldNode.NextStates.FindRef(InStateChangeTo);
		if (NextStateCondition.PassAction.IsBound())
		{
			NextStateCondition.PassAction.Execute();
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

bool OSGeneralStateMachine::IsSameState(const FName& InState) const
{
	return this->CurrentState == InState;
}

bool OSGeneralStateMachine::CanChangeToState(const FName& InNewState) const
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
					UE_LOG(LogGeneralStateMachine, Verbose, TEXT("Changing success, now current state become:\"%s\"."), *InNewState.ToString());
				}
				else
				{
					UE_LOG(LogGeneralStateMachine, Verbose, TEXT("Changing failed, the condition of state to did not pass."), *this->CurrentState.ToString(), *InNewState.ToString());
				}
			}
			else
			{
				UE_LOG(LogGeneralStateMachine, Verbose, TEXT("Changing state failed, current state:has no next state:\"%s\"."), *this->CurrentState.ToString(), *InNewState.ToString());
			}
		}
	}
	else
	{
		UE_LOG(LogGeneralStateMachine, Verbose, TEXT("Changing state failed, new state:has not exist."), *InNewState.ToString());
	}

	return bSuccessful;
}

void OSGeneralStateMachine::SetStateMachineNodeOnEnterStateDelegate(const FName& InNodeName, FSimpleDelegate InOnEnterStateDelegate)
{
	if (FGeneralStateMachineNode* Node = this->CreatedStates.Find(InNodeName))
	{
		Node->OnEnterState = InOnEnterStateDelegate;
	}
	else
	{
		UE_LOG(LogGeneralStateMachine, Warning, TEXT("InNodeName \"%s\" is invalid on set enter delegate, abort setting."), *InNodeName.ToString());
	}
}

void OSGeneralStateMachine::SetStateMachineNodeOnLeaveStateDelegate(const FName& InNodeName, FSimpleDelegate InOnLeaveStateDelegate)
{
	if (FGeneralStateMachineNode* Node = this->CreatedStates.Find(InNodeName))
	{
		Node->OnLeaveState = InOnLeaveStateDelegate;
	}
	else
	{
		UE_LOG(LogGeneralStateMachine, Warning, TEXT("InNodeName \"%s\" is invalid on set leave delegate, abort setting."), *InNodeName.ToString());
	}
}

void OSGeneralStateMachine::SetStateMachineNodeOnUpdateStateDelegate(const FName& InNodeName, FSimpleDelegate InOnUpdateStateDelegate)
{
	if (FGeneralStateMachineNode* Node = this->CreatedStates.Find(InNodeName))
	{
		Node->OnUpdateState = InOnUpdateStateDelegate;
	}
	else
	{
		UE_LOG(LogGeneralStateMachine, Warning, TEXT("InNodeName \"%s\" is invalid on set update delegate, abort setting."), *InNodeName.ToString());
	}
}

void OSGeneralStateMachine::SetStateMachineNodeOnTickStateDelegate(const FName& InNodeName, FOnStateTickedDelegate InOnTickStateDelegate)
{
	if (FGeneralStateMachineNode* Node = this->CreatedStates.Find(InNodeName))
	{
		Node->OnTickState = InOnTickStateDelegate;
	}
	else
	{
		UE_LOG(LogGeneralStateMachine, Warning, TEXT("InNodeName \"%s\" is invalid on set tick delegate, abort setting."), *InNodeName.ToString());
	}

	// 如果还没有注册到引擎的Tick调用里，则执行注册步骤，该步骤目的是只有在需要Tick时才注册Tick函数，从而减少一些性能开销。
	if (!this->TickDelegateHandle.IsValid())
	{
		FTickerDelegate TickerDelegate;
		TickerDelegate.BindRaw(this, &OSGeneralStateMachine::Tick);
		FTicker::GetCoreTicker().AddTicker(TickerDelegate);
	}
}

void OSGeneralStateMachine::SetStateMachineCondition(const FName& InCurrentStateName, const FName& InNextStateName, FGeneralStateMachineConditionCheckDelegate InConditionCheck, FSimpleDelegate InPassAction, FSimpleDelegate InNoPassAction)
{
	if (FGeneralStateMachineNode* Node = this->CreatedStates.Find(InCurrentStateName))
	{
		FGeneralStateMachineCondition Condition;
		Condition.NextStateName = InNextStateName;
		Condition.Condition = InConditionCheck;
		Condition.PassAction = InPassAction;
		Condition.NoPassAction = InNoPassAction;

		FGeneralStateMachineCondition& CurrentCondition = Node->NextStates.FindOrAdd(InNextStateName);
		CurrentCondition = Condition;
	}
	else
	{
		UE_LOG(LogGeneralStateMachine, Warning, TEXT("InCurrentStateName is invalid on set condition, abort setting."), *InCurrentStateName.ToString());
	}
}

void OSGeneralStateMachine::SetStateMachineCondition(const FName& InCurrentStateName, const FName& InNextStateName, FSimpleDelegate InPassAction)
{
	this->SetStateMachineCondition(InCurrentStateName, InNextStateName, FGeneralStateMachineConditionCheckDelegate(), InPassAction);
}

