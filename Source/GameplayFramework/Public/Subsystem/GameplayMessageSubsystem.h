// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "GMSMessageType.h"

#include "GameplayMessageSubsystem.generated.h"

DECLARE_LOG_CATEGORY_CLASS(LogGameplayMessageSubsystem, Log, All);

// 订阅蓝图函数需要用到的委托。
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnMessageReceivedBP, UGMSMessageBase*, Message);

// 订阅C++消息需要用到的委托。
DECLARE_DELEGATE_OneParam(FOnMessageReceived, UGMSMessageBase*)

// 注册信息后返回的句柄。
USTRUCT(BlueprintType)
struct FGMSListenerHandle
{
	GENERATED_BODY()

	FGameplayTag Tag;

	int32 CurrentID = -1;

	bool IsValid() const
	{
		return this->Tag.IsValid() && this->CurrentID >= 0;
	}
};

/**
 * 发布-订阅（Pub-Sub）模式的全局消息系统。
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UGameplayMessageSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	// 订阅者信息。
	struct FMessageListenerData
	{
		// 唯一ID。
		int32 ID = -1;

		// 接收消息的回调函数。
		FOnMessageReceived ReceivedMessageCallback;

		// 执行回调。
		void Execute(UGMSMessageBase* InMessage) const;

		// 检测是否可用。
		bool IsValid() const;

		FMessageListenerData(int32 InID, FOnMessageReceived InCallback)
			: ID(InID), ReceivedMessageCallback(InCallback)
		{
		}
	};

	// 每个GameplayTag对应的所有订阅者信息。
	struct FMessageListenerList
	{
		TMap<int32, FMessageListenerData> GameplayListenerDataList;
		int32 CurrentID = -1;
	};

	// 保存已订阅的消息。
	TMap<FGameplayTag, FMessageListenerList> MessageListeners;

	// 已缓存的消息组。用户在Broadcast消息时可以通过选择“InNeedCache”让发送的消息缓存到消息组里，这样后续如果有需要某个Tag的消息时可以通过API直接获取缓存的消息。
	// 这个机制是为了防止例如玩家的血量已经变更，血量变更消息已经发送出去，这个时候需要生成一个显示玩家血量的UI，但是因为血量变更的消息已经发送了，这个UI再注册消息也不会收到当前血量的信息，所以用这个缓存消息组缓存发送过的消息的话，这个显示UI就可以通过API获取到最后一次发送的消息并获取正确的数值。
	UPROPERTY()
	TMap<FGameplayTag, UGMSMessageBase*> CachedMessages;

protected:
	// 蓝图订阅消息接口。
	UFUNCTION(BlueprintCallable, Category = "GameplayMessageSubsystem", Meta = (DisplayName = "Register"))
	FGMSListenerHandle K2_Register(FGameplayTag InGameplayTag, FOnMessageReceivedBP OnMessageReceived);

	// 实际执行消息注册的接口。
	FGMSListenerHandle RegisterInternal(FGameplayTag InGameplayTag, FOnMessageReceived InCallbackHolder);

	// 实际执行注销信息。
	void UnregisterInternal(FGameplayTag InGameplayTag, int32 HandleID);

public:
	// C++订阅消息接口。
	FGMSListenerHandle Register(FGameplayTag InGameplayTag, FOnMessageReceived OnMessageReceived);

	// 发布消息。
	UFUNCTION(BlueprintCallable, Category = "Gameplay Message System")
	void Broadcast(FGameplayTag InGameplayTag, UGMSMessageBase* InMessage = nullptr, bool InNeedCache = false);

	// 注销已经注册的订阅。
	UFUNCTION(BlueprintCallable, Category = "Gameplay Message System")
	void Unregister(UPARAM(Ref)FGMSListenerHandle& InHandle);

	// 检测GMS句柄是否合法。
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay Message System")
	static bool IsHandleValid(UPARAM(Ref)const FGMSListenerHandle& InHandle);

	// 获取缓存的消息。如果Tag没有缓存过消息，则返回空。
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay Message System")
	UGMSMessageBase* GetCachedMessage(const FGameplayTag InGameplayTag);

	// 移除缓存的消息。删除指定Tag缓存的消息。
	UFUNCTION(BlueprintCallable, Category = "Gameplay Message System")
	void RemoveCachedMessage(const FGameplayTag InGameplayTag);
};

