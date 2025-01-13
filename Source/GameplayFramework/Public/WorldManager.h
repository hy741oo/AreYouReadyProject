// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AYRGameViewportClient.h"

#include "WorldManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogWorldManager, Log, All);

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UWorldManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:
	// 最小加载时间。
	const float MinimumLoadingTime = 3.f;

	// 控制延迟加载的定时器。
	FTimerHandle DelayLoadingScreenHandle;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// 执行黑屏渐入渐出。
	UFUNCTION(BlueprintCallable, Category = "World Manager")
	void StartFade(const float DurationTime, const bool bFadeIn = false) const;
	// 带委托版本。
	UFUNCTION(BlueprintCallable, Category = "World Manager")
	void StartFadeWithEvent(FOnFadeEnd OnFadeEnd, const float DurationTime, const bool bFadeIn = false) const;

	// 终止渐入渐出。
	UFUNCTION(BlueprintCallable)
	void StopFade() const;
};
