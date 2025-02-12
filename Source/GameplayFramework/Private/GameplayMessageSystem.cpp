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
		for (const TPair<uint32, FMessageListenerData>& ListenerData : Listeners->GameplayListenerDataList)
		{
			TVariant<FOnMessageReceived, TFunction<void(UGMSMessageBase*)>> Callback = ListenerData.Value.RecievedMessageCallback;
			if (Callback.IsType<FOnMessageReceived>())
			{
				Callback.Get<FOnMessageReceived>().ExecuteIfBound(InMessage);
			}
			else if (Callback.IsType<TFunction<void(UGMSMessageBase*)>>())
			{
				Callback.Get<TFunction<void(UGMSMessageBase*)>>()(InMessage);
			}
		}
	}
}

