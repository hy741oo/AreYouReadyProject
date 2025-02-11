// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"

#include "GameplayMessageSystem.generated.h"

DECLARE_LOG_CATEGORY_CLASS(LogGameplayMessageSystem, Log, All);

// 订阅者信息。
struct FMessageListenerData
{
	// 唯一ID。
	uint32 ID = 0;

	// 接收消息的回调函数。
	TFunction<void(const void*)> RecievedMessageCallback;
};

// 每个GameplayTag对应的订阅者信息。
struct FMessageListenerList
{
	TMap<uint32, FMessageListenerData> GameplayListenerDataList;
	uint32 CurrentID = 0;
};

/**
 * 发布-订阅（Pub-Sub）模式的全局消息系统。
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UGameplayMessageSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:
	// 保存已订阅的消息。
	TMap<FGameplayTag, FMessageListenerList> MessageListeners;

public:
	// 订阅消息。
	template <typename SubscriberType>
	void Register(FGameplayTag InGameplayTag, SubscriberType* InObject, void(SubscriberType::* InCallback)(const void*))
	{
		if (!InObject)
		{
			UE_LOG(LogGameplayMessageSystem, Display, TEXT("Object is nullptr on register message."));
			return;
		}

		if (!InCallback)
		{
			UE_LOG(LogGameplayMessageSystem, Display, TEXT("Callback is nullptr on register message."));
			return;
		}

		FMessageListenerList& Listeners = this->MessageListeners.FindOrAdd(InGameplayTag);

		FMessageListenerData ListenerData;
		ListenerData.ID = ++Listeners.CurrentID;
		TWeakObjectPtr<SubscriberType> Object(InObject);
		TFunction<void(const void*)> Callback = [Object, InCallback](const void* Message) -> void
			{
				if (SubscriberType* Owner = Object.Get())
				{
					(Owner->*InCallback)(Message);
				}
			};
		ListenerData.RecievedMessageCallback = Callback;

		Listeners.GameplayListenerDataList.Add(ListenerData.ID, ListenerData);

	}

	// 发布消息。
	void Broadcast(const FGameplayTag& InGameplayTag, const void* InMessage);

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override
	{
		FGameplayTag GameplayTag = UGameplayTagsManager::Get().AddNativeGameplayTag("A.B.C");
		this->Register(GameplayTag, this, &ThisClass::Test);
		int i = 123;
		this->Broadcast(GameplayTag, &GameplayTag);
	}

	void Test(const void* InMessage)
	{
		if (const int* p = static_cast<const int*>(InMessage))
		{
			int i = *p;
			int asd = i;
		}
		return;
	}
};
