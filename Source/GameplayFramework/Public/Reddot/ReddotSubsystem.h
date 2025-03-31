// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"

#include "ReddotSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogReddotSubsystem, Log, All);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnReddotStateUpdatedDynamicDelegate, bool, bHasAnyReddot);
DECLARE_DELEGATE_OneParam(FOnReddotStateUpdatedDelegate, bool);

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UReddotSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:
	// 已添加红点的数据。
	TMap<FGameplayTag, TMap<FGameplayTag, FOnReddotStateUpdatedDelegate>> ReddotList;

	// 根据Gameplay Tag结构递归更新红点信息。
	void UpdateReddotState(FGameplayTag InGameplayTag) const;

public:
	// 该GameplayTag是否存在红点。
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Reddot Subsystem")
	bool HasAnyReddot(FGameplayTag InGameplayTag) const;

	// 添加红点。
	UFUNCTION(BlueprintCallable, Category = "Reddot Subsystem")
	void AddReddot(FGameplayTag InGameplayTag, FOnReddotStateUpdatedDynamicDelegate InUpdateCallbackDelegate);

	// 删除红点。
	UFUNCTION(BlueprintCallable, Category = "Reddot Subsystem")
	void RemoveReddot(FGameplayTag InGameplayTag);
};
