// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"

#include "GameplayMessageSystem.generated.h"

DECLARE_LOG_CATEGORY_CLASS(LogGameplayMessageSystem, Log, All);

// 订阅蓝图函数需要用到的委托。
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnMessageReceivedBP, UGMSMessageBase*, Message);

// 订阅C++消息需要用到的委托。
DECLARE_DELEGATE_OneParam(FOnMessageReceived, UGMSMessageBase*)

using FGMSCallbackHolder = TVariant<FOnMessageReceivedBP, FOnMessageReceived>;

// 消息基类。
UCLASS(Abstract, Blueprintable, BlueprintType, HideDropdown)
class UGMSMessageBase : public UObject
{
	GENERATED_BODY()

public:
	// 调试代码。
	virtual FString ToString() const { return this->GetClass()->GetName(); }
};

// 注册信息后返回的句柄。
USTRUCT(BlueprintType)
struct FGMSListenerHandle
{
	GENERATED_BODY()

	FGameplayTag Tag;

	int32 CurrentID = -1;
};

//// 常用类型的消息类。
//#define DEFINE_GMS_MESSAGETYPE(Type) \
//UCLASS() \
//class UGMS##Type##Message : public UGMSMessageBase \
//{ \
//	GENERATED_BODY() \
// \
//public: \
//	UPROPERTY(BlueprintReadWrite, EditAnywhere) \
//	##Type Value; \
//};
//#define DEFINE_GMS_MESSAGETYPE_HELPER(Type) DEFINE_GMS_MESSAGETYPE(Type)
//
//#undef DEFINE_GMS_MESSAGETYPE
//#undef DEFINE_GMS_MESSAGETYPE_HELPER

UCLASS()
class UGMSintMessage : public UGMSMessageBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Value;
};

UCLASS()
class UGMSfloatMessage : public UGMSMessageBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Value;
};

/**
 * 发布-订阅（Pub-Sub）模式的全局消息系统。
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UGameplayMessageSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	// 订阅者信息。
	struct FMessageListenerData
	{
		// 唯一ID。
		int32 ID = -1;

		// 接收消息的回调函数。
		FGMSCallbackHolder ReceivedMessageCallback;

		// 执行回调。
		void Execute(UGMSMessageBase* InMessage) const;

		// 检测是否可用。
		bool IsValid() const;

		FMessageListenerData(int32 InID, FGMSCallbackHolder InCallback)
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

protected:
	// 蓝图订阅消息接口。
	UFUNCTION(BlueprintCallable, Category = "GameplayMessageSystem", Meta = (DisplayName = "Register"))
	FGMSListenerHandle K2_Register(FGameplayTag InGameplayTag, FOnMessageReceivedBP OnMessageReceived);

	// 实际执行消息注册的接口。
	FGMSListenerHandle RegisterInternal(FGameplayTag InGameplayTag, FGMSCallbackHolder InCallbackHolder);

	// 实际执行注销信息。
	void UnregisterInternal(FGameplayTag InGameplayTag, int32 HandleID);

public:
	// C++订阅消息接口。
	FGMSListenerHandle Register(FGameplayTag InGameplayTag, FOnMessageReceived OnMessageReceived);

	// 发布消息。
	UFUNCTION(BlueprintCallable, Category = "Gameplay Message System")
	void Broadcast(FGameplayTag InGameplayTag, UGMSMessageBase* InMessage);

	// 注销已经注册的订阅。
	UFUNCTION(BlueprintCallable, Category = "Gameplay Message System")
	void Unregister(UPARAM(Ref)FGMSListenerHandle& InHandle);
};

