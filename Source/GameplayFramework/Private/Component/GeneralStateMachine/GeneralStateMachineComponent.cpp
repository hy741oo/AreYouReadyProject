// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/GeneralStateMachine/GeneralStateMachineComponent.h"

void UGeneralStateMachineComponent::InitGeneralStateMachine(const FName InInitState)
{
	this->StateMachine.InitGeneralStateMachine(InInitState);
}

bool UGeneralStateMachineComponent::ChangeStateTo(const FName InStateChangeTo)
{
	return this->StateMachine.ChangeStateTo(InStateChangeTo);
}

bool UGeneralStateMachineComponent::IsSameState(const FName InState) const
{
	return this->StateMachine.IsSameState(InState);
}

bool UGeneralStateMachineComponent::CanChangeToState(const FName InNewState) const
{
	return this->StateMachine.CanChangeToState(InNewState);
}

void UGeneralStateMachineComponent::CreateStateMachineNode(const FName InNodeName)
{
	this->StateMachine.CreateStateMachineNode(InNodeName);
}

void UGeneralStateMachineComponent::SetStateMachineNodeOnEnterStateDelegate(const FName InNodeName, FSimpleDelegate InOnEnterStateDelegate)
{
	this->StateMachine.SetStateMachineNodeOnEnterStateDelegate(InNodeName, InOnEnterStateDelegate);
}

void UGeneralStateMachineComponent::SetStateMachineNodeOnEnterStateDelegate(const FName InNodeName, FSimpleDynamicDelegate InOnEnterStateDelegate)
{
	FSimpleDelegate Delegate;
	Delegate.BindUFunction(InOnEnterStateDelegate.GetUObject(), InOnEnterStateDelegate.GetFunctionName());
	this->SetStateMachineNodeOnEnterStateDelegate(InNodeName, Delegate);
}

void UGeneralStateMachineComponent::SetStateMachineNodeOnLeaveStateDelegate(const FName InNodeName, FSimpleDelegate InOnLeaveStateDelegate)
{
	this->StateMachine.SetStateMachineNodeOnLeaveStateDelegate(InNodeName, InOnLeaveStateDelegate);
}

void UGeneralStateMachineComponent::SetStateMachineNodeOnLeaveStateDelegate(const FName InNodeName, FSimpleDynamicDelegate InOnLeaveStateDelegate)
{
	FSimpleDelegate Delegate;
	Delegate.BindUFunction(InOnLeaveStateDelegate.GetUObject(), InOnLeaveStateDelegate.GetFunctionName());
	this->SetStateMachineNodeOnLeaveStateDelegate(InNodeName, Delegate);
}

void UGeneralStateMachineComponent::SetStateMachineNodeOnUpdateStateDelegate(const FName InNodeName, FSimpleDelegate InOnUpdateStateDelegate)
{
	this->StateMachine.SetStateMachineNodeOnUpdateStateDelegate(InNodeName, InOnUpdateStateDelegate);
}

void UGeneralStateMachineComponent::SetStateMachineNodeOnUpdateStateDelegate(const FName InNodeName, FSimpleDynamicDelegate InOnUpdateStateDelegate)
{
	FSimpleDelegate Delegate;
	Delegate.BindUFunction(InOnUpdateStateDelegate.GetUObject(), InOnUpdateStateDelegate.GetFunctionName());
	this->SetStateMachineNodeOnUpdateStateDelegate(InNodeName, Delegate);
}

void UGeneralStateMachineComponent::SetStateMachineNodeOnTickStateDelegate(const FName InNodeName, FOnStateTickedDelegate InOnTickStateDelegate)
{
	this->StateMachine.SetStateMachineNodeOnTickStateDelegate(InNodeName, InOnTickStateDelegate);
}

void UGeneralStateMachineComponent::SetStateMachineNodeOnTickStateDelegate(const FName InNodeName, FOnStateTickedDynamicDelegate InOnTickStateDelegate)
{
	FOnStateTickedDelegate Delegate;
	Delegate.BindUFunction(InOnTickStateDelegate.GetUObject(), InOnTickStateDelegate.GetFunctionName());
	this->SetStateMachineNodeOnTickStateDelegate(InNodeName, Delegate);
}

void UGeneralStateMachineComponent::SetStateMachineConditionWithCheckAndPassOrNoPassAction(const FName InCurrentStateName, const FName InNextStateName, FGeneralStateMachineConditionCheckDynamicDelegate InConditionCheck, FSimpleDynamicDelegate InPassAction, FSimpleDynamicDelegate InNoPassAction)
{
	FGeneralStateMachineConditionCheckDelegate CheckDelegate;
	CheckDelegate.BindUFunction(InConditionCheck.GetUObject(), InConditionCheck.GetFunctionName());
	FSimpleDelegate PassActionDelegate;
	PassActionDelegate.BindUFunction(InPassAction.GetUObject(), InPassAction.GetFunctionName());
	FSimpleDelegate NoPassActionDelegate;
	NoPassActionDelegate.BindUFunction(InNoPassAction.GetUObject(), InNoPassAction.GetFunctionName());
	this->SetStateMachineCondition(InCurrentStateName, InNextStateName, CheckDelegate, PassActionDelegate, NoPassActionDelegate);
}

void UGeneralStateMachineComponent::SetStateMachineConditionWithCheckAndAction(const FName InCurrentStateName, const FName InNextStateName, FGeneralStateMachineConditionCheckDynamicDelegate InConditionCheck, FSimpleDynamicDelegate InAction)
{
	FGeneralStateMachineConditionCheckDelegate CheckDelegate;
	CheckDelegate.BindUFunction(InConditionCheck.GetUObject(), InConditionCheck.GetFunctionName());
	FSimpleDelegate ActionDelegate;
	ActionDelegate.BindUFunction(InAction.GetUObject(), InAction.GetFunctionName());
	this->SetStateMachineCondition(InCurrentStateName, InNextStateName, CheckDelegate, ActionDelegate);
}

void UGeneralStateMachineComponent::SetStateMachineConditionWithCheck(const FName InCurrentStateName, const FName InNextStateName, FGeneralStateMachineConditionCheckDynamicDelegate InConditionCheck)
{
	FGeneralStateMachineConditionCheckDelegate CheckDelegate;
	CheckDelegate.BindUFunction(InConditionCheck.GetUObject(), InConditionCheck.GetFunctionName());
	this->SetStateMachineCondition(InCurrentStateName, InNextStateName, CheckDelegate);
}

void UGeneralStateMachineComponent::SetStateMachineConditionWithAction(const FName InCurrentStateName, const FName InNextStateName, FSimpleDynamicDelegate InAction)
{
	FSimpleDelegate ActionDelegate;
	ActionDelegate.BindUFunction(InAction.GetUObject(), InAction.GetFunctionName());
	this->SetStateMachineCondition(InCurrentStateName, InNextStateName, FGeneralStateMachineConditionCheckDelegate(), ActionDelegate);
}

void UGeneralStateMachineComponent::SetStateMachineConditionDynamic(const FName InCurrentStateName, const FName InNextStateName)
{
	this->SetStateMachineCondition(InCurrentStateName, InNextStateName, FGeneralStateMachineConditionCheckDelegate());
}

void UGeneralStateMachineComponent::SetStateMachineCondition(const FName InCurrentStateName, const FName InNextStateName, FGeneralStateMachineConditionCheckDelegate InConditionCheck, FSimpleDelegate InPassAction, FSimpleDelegate InNoPassAction)
{
	this->StateMachine.SetStateMachineCondition(InCurrentStateName, InNextStateName, InConditionCheck, InPassAction, InNoPassAction);
}

void UGeneralStateMachineComponent::PushDownState()
{
	this->StateMachine.PushDownState();
}

