// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayMessageSubsystem.h"

void UGameplayMessageSubsystem::Broadcast(FGameplayTag InGameplayTag, UGMSMessageBase* InMessage, bool InNeedCache)
{
	if (!InGameplayTag.IsValid())
	{
		UE_LOG(LogGameplayMessageSubsystem, Warning, TEXT("GameplayTag is invalid on broadcast, abort."));
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
				// 如果绑定的委托无效（可能是订阅者忘记了主动解绑），则进行解绑操作。
				this->UnregisterInternal(InGameplayTag, ListenerData.Value.ID);
			}
		}
	}

	// 判断是否需要执行缓存操作，如果需要缓存则更新InGameplayTag的键值，如果不需要缓存操作的话则清除之前已经缓存的消息（如果有），之所以考虑删除之前缓存的消息而不是忽略，是因为忽略了的话，之前缓存的消息就是过期消息，会对后续逻辑产生不良的影响。
	if (InNeedCache)
	{
		this->CachedMessages.FindOrAdd(InGameplayTag) = InMessage;
	}
	else
	{
		this->CachedMessages.Remove(InGameplayTag);
	}
}

FGMSListenerHandle UGameplayMessageSubsystem::K2_Register(FGameplayTag InGameplayTag, FOnMessageReceivedBP OnMessageReceived)
{
	if (!InGameplayTag.IsValid())
	{
		UE_LOG(LogGameplayMessageSubsystem, Warning, TEXT("GameplayTag is invalid on register blueprint message, abort."));
		return FGMSListenerHandle();
	}

	if (!OnMessageReceived.IsBound())
	{
		UE_LOG(LogGameplayMessageSubsystem, Warning, TEXT("OnMessageReceived dynamic delegate is invalid on register blueprint message, abort."));
		return FGMSListenerHandle();
	}

	FOnMessageReceived CppCallbackHolder;
	CppCallbackHolder.BindUFunction(OnMessageReceived.GetUObject(), OnMessageReceived.GetFunctionName());
	return this->RegisterInternal(InGameplayTag, CppCallbackHolder);
}

FGMSListenerHandle UGameplayMessageSubsystem::Register(FGameplayTag InGameplayTag, FOnMessageReceived OnMessageReceived)
{
	if (!InGameplayTag.IsValid())
	{
		UE_LOG(LogGameplayMessageSubsystem, Warning, TEXT("GameplayTag is invalid on register message, abort."));
		return FGMSListenerHandle();
	}

	if (!OnMessageReceived.IsBound())
	{
		UE_LOG(LogGameplayMessageSubsystem, Warning, TEXT("OnMessageReceived delegate is invalid on register message, abort."));
	}

	return this->RegisterInternal(InGameplayTag, OnMessageReceived);
}

FGMSListenerHandle UGameplayMessageSubsystem::RegisterInternal(FGameplayTag InGameplayTag, FOnMessageReceived InCallbackHolder)
{
		FMessageListenerList& Listeners = this->MessageListeners.FindOrAdd(InGameplayTag);

		FMessageListenerData ListenerData(++Listeners.CurrentID, InCallbackHolder);
		Listeners.GameplayListenerDataList.Add(ListenerData.ID, ListenerData);

		FGMSListenerHandle Handle;
		Handle.Tag = InGameplayTag;
		Handle.CurrentID = ListenerData.ID;
		return Handle;
}

void UGameplayMessageSubsystem::FMessageListenerData::Execute(UGMSMessageBase* InMessage) const
{
	bool bSuccess = false;
	if (this->ReceivedMessageCallback.IsBound())
	{
		bSuccess = true;
		this->ReceivedMessageCallback.Execute(InMessage);
	}

	if (bSuccess)
	{
		UE_LOG(LogGameplayMessageSubsystem, Verbose, TEXT("Broadcast message, message context: %s"), InMessage ? *InMessage->ToString() : TEXT("Empty parameter message."));
	}
	else
	{
		UE_LOG(LogGameplayMessageSubsystem, Warning, TEXT("Broadcast message failed because there is object is expired"));
	}
}

bool UGameplayMessageSubsystem::FMessageListenerData::IsValid() const
{
	return this->ReceivedMessageCallback.IsBound();
}

void UGameplayMessageSubsystem::Unregister(UPARAM(Ref)FGMSListenerHandle& InHandle)
{
	this->UnregisterInternal(InHandle.Tag, InHandle.CurrentID);
	InHandle.CurrentID = -1;
	InHandle.Tag = FGameplayTag();
}

void UGameplayMessageSubsystem::UnregisterInternal(FGameplayTag InGameplayTag, int32 InHandleID)
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
			UE_LOG(LogGameplayMessageSubsystem, Warning, TEXT("Can't find handle id: \"%d\", unregister failed."), InHandleID);
		}
	}
	else
	{
		UE_LOG(LogGameplayMessageSubsystem, Warning, TEXT("Can't find gameplaytag: \"%s\", unregister failed."), *InGameplayTag.ToString());
	}
}

bool UGameplayMessageSubsystem::IsHandleValid(UPARAM(Ref)const FGMSListenerHandle& InHandle)
{
	return InHandle.CurrentID != -1 && InHandle.Tag.IsValid();
}

UGMSMessageBase* UGameplayMessageSubsystem::GetCachedMessage(const FGameplayTag InGameplayTag)
{
	if (!InGameplayTag.IsValid())
	{
		UE_LOG(LogGameplayMessageSubsystem, Log, TEXT("Get cached message failed, InGameplayTag:\"%s\" is invalid."), *InGameplayTag.ToString());
		return nullptr;
	}

	if (this->CachedMessages.Contains(InGameplayTag))
	{
		return this->CachedMessages[InGameplayTag];
	}
	else
	{
		UE_LOG(LogGameplayMessageSubsystem, Log, TEXT("Get cached message failed, InGameplayTag:\"%s\" has no cached message."), *InGameplayTag.ToString());
		return nullptr;
	}
}

void UGameplayMessageSubsystem::RemoveCachedMessage(const FGameplayTag InGameplayTag)
{
	this->CachedMessages.Remove(InGameplayTag);
}

