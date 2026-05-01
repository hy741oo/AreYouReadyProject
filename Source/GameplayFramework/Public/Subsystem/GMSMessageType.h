// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GMSMessageType.generated.h"

// 消息基类。
UCLASS(Abstract, Blueprintable, BlueprintType, HideDropdown)
class GAMEPLAYFRAMEWORK_API UGMSMessageBase : public UObject
{
	GENERATED_BODY()

public:
	// 调试代码。
	virtual FString ToString() const { return this->GetClass()->GetName(); }
};

UCLASS()
class GAMEPLAYFRAMEWORK_API UGMSBoolMessage : public UGMSMessageBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (ExposeOnSpawn))
	bool Value = false;
};

UCLASS()
class GAMEPLAYFRAMEWORK_API UGMSIntMessage : public UGMSMessageBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (ExposeOnSpawn))
	int Value = 0;
};

UCLASS()
class GAMEPLAYFRAMEWORK_API UGMSInt64Message : public UGMSMessageBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (ExposeOnSpawn))
	int64 Value = 0;
};

UCLASS()
class GAMEPLAYFRAMEWORK_API UGMSFloatMessage : public UGMSMessageBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (ExposeOnSpawn))
	float Value = .0f;
};
