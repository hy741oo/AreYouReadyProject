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
	// 红点Tag及其对应的子Tag。
	TMap<FGameplayTag, TSet<FGameplayTag>> ReddotHierarchy;

	// Tag及其对应的更新回调。
	TMap<FGameplayTag, FOnReddotStateUpdatedDelegate> RegisteredReddot;

	// 通知红点信息。
	void NotifyReddotState(FGameplayTag InGameplayTag, bool bHasReddot) const;

	// 构建红点树。
	void BuildReddotHierarchy(FGameplayTag InCurrentGameplayTag, FGameplayTag InParentGameplayTag);

public:
	// 该GameplayTag是否存在红点。
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Reddot Subsystem")
	bool HasAnyReddot(FGameplayTag InGameplayTag) const;

	// 注册红点。用于后续执行回调。同时也可以通过返回值知晓当前Tag是否有红点。
	UFUNCTION(BlueprintCallable, Category = "Reddot Subsystem")
	bool RegisterReddot(FGameplayTag InGameplayTag, FOnReddotStateUpdatedDynamicDelegate InUpdateCallbackDelegate);

	// 注销红点。
	UFUNCTION(BlueprintCallable, Category = "Reddot Subsystem")
	void UnregisterReddot(FGameplayTag InGameplayTag);

	// 添加红点。
	UFUNCTION(BlueprintCallable, Category = "Reddot Subsystem")
	void AddReddot(FGameplayTag InGameplayTag);

	// 删除红点。
	UFUNCTION(BlueprintCallable, Category = "Reddot Subsystem")
	void RemoveReddot(FGameplayTag InGameplayTag);
};
