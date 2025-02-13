// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayMessageSystem.h"

void UGameplayMessageSystem::Broadcast(FGameplayTag InGameplayTag, UGMSMessageBase* InMessage)
{
	if (!InGameplayTag.IsValid())
	{
		UE_LOG(LogGameplayMessageSystem, Warning, TEXT("GameplayTag is invalid on broadcast, abort."));
		return;
	}

	if (FMessageListenerList* Listeners = this->MessageListeners.Find(InGameplayTag))
	{
		for (const TPair<int32, FMessageListenerData>& ListenerData : Listeners->GameplayListenerDataList)
		{
			ListenerData.Value.Execute(InMessage);
		}
	}
}

FGMSListenerHandle UGameplayMessageSystem::RegisterInternal(FGameplayTag InGameplayTag, FGMSCallbackHolder InCallbackHolder)
{
		FMessageListenerList& Listeners = this->MessageListeners.FindOrAdd(InGameplayTag);

		FMessageListenerData ListenerData;
		ListenerData.ID = ++Listeners.CurrentID;
		ListenerData.ReceivedMessageCallback = InCallbackHolder;
		Listeners.GameplayListenerDataList.Add(ListenerData.ID, ListenerData);

		FGMSListenerHandle Handle;
		Handle.Tag = InGameplayTag;
		Handle.CurrentID = ListenerData.ID;
		return Handle;
}

void FMessageListenerData::Execute(UGMSMessageBase* InMessage) const
{
	if (this->ReceivedMessageCallback.IsType<FOnMessageReceived>())
	{
		this->ReceivedMessageCallback.Get<FOnMessageReceived>().ExecuteIfBound(InMessage);
	}
	else if (this->ReceivedMessageCallback.IsType<TFunction<void(UGMSMessageBase*)>>())
	{
		this->ReceivedMessageCallback.Get<TFunction<void(UGMSMessageBase*)>>()(InMessage);
	}
}

void UGameplayMessageSystem::Unregister(FGMSListenerHandle& InHandle)
{
	if (FMessageListenerList* ListenerList = this->MessageListeners.Find(InHandle.Tag))
	{
		if (FMessageListenerData* ListenerData = ListenerList->GameplayListenerDataList.Find(InHandle.CurrentID))
		{
			ListenerList->GameplayListenerDataList.Remove(InHandle.CurrentID);
			if (ListenerList->GameplayListenerDataList.Num() == 0)
			{
				this->MessageListeners.Remove(InHandle.Tag);
			}
			InHandle.CurrentID = -1;
			InHandle.Tag = FGameplayTag();
		}
		else
		{
			UE_LOG(LogGameplayMessageSystem, Warning, TEXT("Can't find handle id: \"%d\", unregister failed."), InHandle.CurrentID);
		}
	}
	else
	{
		UE_LOG(LogGameplayMessageSystem, Warning, TEXT("Can't find gameplaytag: \"%s\", unregister failed."), *InHandle.Tag.ToString());
	}
}

