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

	if (!this->ReddotList.Contains(InGameplayTag))
	{
		return false;
	}

	const TMap<FGameplayTag, FOnReddotStateUpdatedDelegate>& Element = this->ReddotList[InGameplayTag];
	if (Element.Num() == 0)
	{
		return false;
	}

	return true;
}

void UReddotSubsystem::AddReddot(FGameplayTag InGameplayTag, FOnReddotStateUpdatedDynamicDelegate InUpdateCallbackDelegate)
{
	// 查询直接父Tag，如果不存在则直接退出。
	const FGameplayTag& ParentTag = InGameplayTag.RequestDirectParent();
	if (!ParentTag.IsValid())
	{
		return;
	}

	// 从Reddot list里查找父Tag对应的数据并把该tag添加进去。
	TMap<FGameplayTag, FOnReddotStateUpdatedDelegate>& FoundPair = this->ReddotList.FindOrAdd(ParentTag);
    FOnReddotStateUpdatedDelegate Delegate;
	Delegate.BindUFunction(InUpdateCallbackDelegate.GetUObject(), InUpdateCallbackDelegate.GetFunctionName());
	FoundPair.Add(InGameplayTag, Delegate);
	this->UpdateReddotState(ParentTag);
}

void UReddotSubsystem::UpdateReddotState(FGameplayTag InGameplayTag) const
{
	const TMap<FGameplayTag, FOnReddotStateUpdatedDelegate>* FoundMap = this->ReddotList.Find(InGameplayTag);
	if (!FoundMap)
	{
		return;
	}

	for (const TPair<FGameplayTag, FOnReddotStateUpdatedDelegate>FoundPair : *FoundMap)
	{
		bool bHasAnyReddot = this->HasAnyReddot(FoundPair.Key);
		FoundPair.Value.ExecuteIfBound(bHasAnyReddot);
	}

	FGameplayTag ParentTag = InGameplayTag.RequestDirectParent();
	if (ParentTag.IsValid())
	{
		this->UpdateReddotState(ParentTag);
	}
		
}

void UReddotSubsystem::RemoveReddot(FGameplayTag InGameplayTag)
{
	if (this->ReddotList.Contains(InGameplayTag))
	{
		// 如果该Tag已经被添加到红点数据里，则说明有其他Tag以此Tag为父类并添加到红点数据了，因此不能被移除掉。
		return;
	}

	FGameplayTag ParentTag = InGameplayTag.RequestDirectParent();
	if (!ParentTag.IsValid() || !this->ReddotList.Contains(ParentTag))
	{
		// 不存在父Tag，或父Tag没有被注册过，则无法被删除。
		return;
	}

	TMap<FGameplayTag, FOnReddotStateUpdatedDelegate>& FoundMap = this->ReddotList[ParentTag];
	FOnReddotStateUpdatedDelegate Callback;
	FoundMap.Remove(InGameplayTag);

	// 更新Tag树上的红点状态。
	this->UpdateReddotState(ParentTag);
}