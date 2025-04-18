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
		TMap<int32, FMessageListenerData> TempLists(Listeners->GameplayListenerDataList);

		for (const TPair<int32, FMessageListenerData>& ListenerData : TempLists)
		{
			if (ListenerData.Value.IsValid())
			{
				ListenerData.Value.Execute(InMessage);
			}
			else
			{
				this->UnregisterInternal(InGameplayTag, ListenerData.Value.ID);
			}
		}
	}
}

FGMSListenerHandle UGameplayMessageSystem::K2_Register(FGameplayTag InGameplayTag, FOnMessageReceivedBP OnMessageReceived)
{
	if (!InGameplayTag.IsValid())
	{
		UE_LOG(LogGameplayMessageSystem, Warning, TEXT("GameplayTag is invalid on register blueprint message, abort."));
		return FGMSListenerHandle();
	}

	if (!OnMessageReceived.IsBound())
	{
		UE_LOG(LogGameplayMessageSystem, Warning, TEXT("OnMessageReceived dynamic delegate is invalid on register blueprint message, abort."));
		return FGMSListenerHandle();
	}

	FOnMessageReceived CppCallbackHolder;
	CppCallbackHolder.BindUFunction(OnMessageReceived.GetUObject(), OnMessageReceived.GetFunctionName());
	return this->RegisterInternal(InGameplayTag, CppCallbackHolder);
}

FGMSListenerHandle UGameplayMessageSystem::Register(FGameplayTag InGameplayTag, FOnMessageReceived OnMessageReceived)
{
	if (!InGameplayTag.IsValid())
	{
		UE_LOG(LogGameplayMessageSystem, Warning, TEXT("GameplayTag is invalid on register message, abort."));
		return FGMSListenerHandle();
	}

	if (!OnMessageReceived.IsBound())
	{
		UE_LOG(LogGameplayMessageSystem, Warning, TEXT("OnMessageReceived delegate is invalid on register message, abort."));
	}

	return this->RegisterInternal(InGameplayTag, OnMessageReceived);
}

FGMSListenerHandle UGameplayMessageSystem::RegisterInternal(FGameplayTag InGameplayTag, FOnMessageReceived InCallbackHolder)
{
		FMessageListenerList& Listeners = this->MessageListeners.FindOrAdd(InGameplayTag);

		FMessageListenerData ListenerData(++Listeners.CurrentID, InCallbackHolder);
		Listeners.GameplayListenerDataList.Add(ListenerData.ID, ListenerData);

		FGMSListenerHandle Handle;
		Handle.Tag = InGameplayTag;
		Handle.CurrentID = ListenerData.ID;
		return Handle;
}

void UGameplayMessageSystem::FMessageListenerData::Execute(UGMSMessageBase* InMessage) const
{
	bool bSuccess = false;
	if (this->ReceivedMessageCallback.IsBound())
	{
		bSuccess = true;
		this->ReceivedMessageCallback.Execute(InMessage);
	}

	if (bSuccess)
	{
		UE_LOG(LogGameplayMessageSystem, Log, TEXT("Broadcast message, message context: %s"), InMessage ? *InMessage->ToString() : TEXT("Empty parameter message."));
	}
	else
	{
		UE_LOG(LogGameplayMessageSystem, Warning, TEXT("Broadcast message failed because there is object is expired"));
	}
}

bool UGameplayMessageSystem::FMessageListenerData::IsValid() const
{
	return this->ReceivedMessageCallback.IsBound();
}

void UGameplayMessageSystem::Unregister(UPARAM(Ref)FGMSListenerHandle& InHandle)
{
	this->UnregisterInternal(InHandle.Tag, InHandle.CurrentID);
	InHandle.CurrentID = -1;
	InHandle.Tag = FGameplayTag();
}

void UGameplayMessageSystem::UnregisterInternal(FGameplayTag InGameplayTag, int32 InHandleID)
{
	if (FMessageListenerList* ListenerList = this->MessageListeners.Find(InGameplayTag))
	{
		if (FMessageListenerData* ListenerData = ListenerList->GameplayListenerDataList.Find(InHandleID))
		{
			ListenerList->GameplayListenerDataList.Remove(InHandleID);
			if (ListenerList->GameplayListenerDataList.Num() == 0)
			{
				this->MessageListeners.Remove(InGameplayTag);
			}
		}
		else
		{
			UE_LOG(LogGameplayMessageSystem, Warning, TEXT("Can't find handle id: \"%d\", unregister failed."), InHandleID);
		}
	}
	else
	{
		UE_LOG(LogGameplayMessageSystem, Warning, TEXT("Can't find gameplaytag: \"%s\", unregister failed."), *InGameplayTag.ToString());
	}
}

