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

	FGameplayTag ParentTag = InGameplayTag.RequestDirectParent();
	if (!ParentTag.IsValid())
	{
		// 当前Tag合法父Tag不合法，说明当前Tag为顶层Tag，直接添加进红点树并通知状态。
		this->ReddotHierarchy.Add(InGameplayTag);
		return;
	}

	if (!this->ReddotHierarchy.Contains(ParentTag))
	{
		// 父Tag不存在于当前红点数，递归构建红点树。
		this->AddReddot(ParentTag);
	}

	if (this->ReddotHierarchy.Contains(ParentTag))
	{
		// 父Tag已经被添加过，将本Tag添加到父Tag的子Tag集合中，同时把当前Tag加进红点树里。
		this->ReddotHierarchy.Add(InGameplayTag);
		this->NotifyReddotState(InGameplayTag, true);
		TSet<FGameplayTag>& SubTags = this->ReddotHierarchy[ParentTag];
		SubTags.Add(InGameplayTag);
		// 如果是新添加的子Tag，则需要同时父Tag更改红点状态。
		if (SubTags.Num() == 1)
		{
			this->NotifyReddotState(ParentTag, true);
		}
	}
}

void UReddotSubsystem::RemoveReddot(FGameplayTag InGameplayTag)
{
	if (!InGameplayTag.IsValid())
	{
		// Tag非法。
		return;
	}

	if (!this->ReddotHierarchy.Contains(InGameplayTag))
	{
		// 该Tag没有添加进红点树内，不能被删除。
		return;
	}

	{
		TSet<FGameplayTag>& SubTags = this->ReddotHierarchy[InGameplayTag];
		if (SubTags.Num() > 0)
		{
			// 如果存在子Tag则不能被删除。
			return;
		}
	}

	// 把当前Tag从红点树移除并通知状态。
	this->ReddotHierarchy.Remove(InGameplayTag);
	this->NotifyReddotState(InGameplayTag, false);

	FGameplayTag ParentTag = InGameplayTag.RequestDirectParent();
	if (!ParentTag.IsValid())
	{
		// 当前Tag合法父Tag不合法，说明当前Tag为顶层Tag，直接退出。
		return;
	}

	if (this->ReddotHierarchy.Contains(ParentTag))
	{
		// 从父Tag的子Tag合集中删除当前Tag。
		TSet<FGameplayTag>& SubTags = this->ReddotHierarchy[ParentTag];
		SubTags.Remove(InGameplayTag);
		// 递归检查父Tag是否也需要更新状态。
		this->RemoveReddot(ParentTag);
	}
}

void UReddotSubsystem::NotifyReddotState(FGameplayTag InGameplayTag, bool bHasReddot) const
{
	if (const FOnReddotStateUpdatedDelegate* UpdateDelegate = this->RegisteredReddot.Find(InGameplayTag))
	{
		UpdateDelegate->ExecuteIfBound(bHasReddot);
	}
}