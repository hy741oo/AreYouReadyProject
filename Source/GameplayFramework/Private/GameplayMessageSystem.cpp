// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayMessageSystem.h"

void UGameplayMessageSystem::Broadcast(const FGameplayTag& InGameplayTag, const void* InMessage)
{
	if (FMessageListenerList* Listeners = this->MessageListeners.Find(InGameplayTag))
	{
		for (const TPair<uint32, FMessageListenerData>& ListenerData : Listeners->GameplayListenerDataList)
		{
			ListenerData.Value.RecievedMessageCallback(InMessage);
		}
	}
}

