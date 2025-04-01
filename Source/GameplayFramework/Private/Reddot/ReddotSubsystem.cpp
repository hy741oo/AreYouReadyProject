// Fill out your copyright notice in the Description page of Project Settings.


#include "Reddot/ReddotSubsystem.h"

DEFINE_LOG_CATEGORY(LogReddotSubsystem)

bool UReddotSubsystem::HasAnyReddot(FGameplayTag InGameplayTag) const
{
	if (!InGameplayTag.IsValid())
	{
		UE_LOG(LogReddotSubsystem, Warning, TEXT("The gameplay tag passed in is not valid! Tag: %s"), *InGameplayTag.ToString());
		return false;
	}

	if (!this->ReddotHierarchy.Contains(InGameplayTag))
	{
		return false;
	}

	const TSet<FGameplayTag>& Element = this->ReddotHierarchy[InGameplayTag];
	if (Element.Num() == 0)
	{
		return false;
	}

	return true;
}

bool UReddotSubsystem::RegisterReddot(FGameplayTag InGameplayTag, FOnReddotStateUpdatedDynamicDelegate InUpdateCallbackDelegate)
{
	if (!InGameplayTag.IsValid() || this->RegisteredReddot.Contains(InGameplayTag))
	{
		// Tag非法，或该Tag已经注册。
		return false;
	}

	FOnReddotStateUpdatedDelegate UpdateDelegate;
	UpdateDelegate.BindUFunction(InUpdateCallbackDelegate.GetUObject(), InUpdateCallbackDelegate.GetFunctionName());
	this->RegisteredReddot.Add(InGameplayTag, UpdateDelegate);

	return this->HasAnyReddot(InGameplayTag);
}

void UReddotSubsystem::UnregisterReddot(FGameplayTag InGameplayTag)
{
	if (this->RegisteredReddot.Contains(InGameplayTag))
	{
		this->RegisteredReddot.Remove(InGameplayTag);
	}
}

void UReddotSubsystem::BuildReddotHierarchy(FGameplayTag InCurrentGameplayTag, FGameplayTag InParentGameplayTag)
{
	if (!InCurrentGameplayTag.IsValid())
	{
		// Tag非法。
		return;
	}

	if (!InParentGameplayTag.IsValid())
	{
		// 父Tag无效，说明当前Tag是最高层Tag，直接返回。
		return;
	}

	// 递归构建红点树。
	this->BuildReddotHierarchy(InParentGameplayTag, InParentGameplayTag.RequestDirectParent());

	TSet<FGameplayTag>& SubTags = this->ReddotHierarchy.FindOrAdd(InParentGameplayTag);
	SubTags.Add(InCurrentGameplayTag);
	// 通知父Tag已添加红点。
	this->NotifyReddotState(InParentGameplayTag, this->HasAnyReddot(InParentGameplayTag));
}

void UReddotSubsystem::AddReddot(FGameplayTag InGameplayTag)
{
	if (!InGameplayTag.IsValid())
	{
		// Tag非法。
		return;
	}

	if (this->ReddotHierarchy.Contains(InGameplayTag))
	{
		// 该Tag已经构建了Tag树，不再重复构建。
		return;
	}

	// 通知当前Tag状态。
	this->NotifyReddotState(InGameplayTag, true);

	// 递归构建红点树。
	this->BuildReddotHierarchy(InGameplayTag, InGameplayTag.RequestDirectParent());
}

void UReddotSubsystem::NotifyReddotState(FGameplayTag InGameplayTag, bool bHasReddot) const
{
	if (const FOnReddotStateUpdatedDelegate* UpdateDelegate = this->RegisteredReddot.Find(InGameplayTag))
	{
		UpdateDelegate->ExecuteIfBound(bHasReddot);
	}
}

void UReddotSubsystem::RemoveReddot(FGameplayTag InGameplayTag)
{
	if (!InGameplayTag.IsValid())
	{
		// Tag非法。
		return;
	}

	if (this->ReddotHierarchy.Contains(InGameplayTag))
	{
		// 该Tag已经构建了Tag树，说明有子Tag，不能被删除。
		return;
	}

	// 通知当前Tag状态。
	this->NotifyReddotState(InGameplayTag, false);

	// 递归构建红点树。
	this->BuildReddotHierarchy(InGameplayTag, InGameplayTag.RequestDirectParent());
}