// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TickableGameInstanceSubsystem.h"
#include "WorldManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogWorldManager, Log, All);

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UWorldManager : public UTickableGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:
	// 判断是否为开屏加载，如果是的话则不执行过场加载操作，防止过场加载的LoadingScreenAttributes覆盖了开屏加载设置。
	bool bIsFirst = true;

	// 控制延迟加载定时器。
	FTimerHandle DelayLoadingScreenHandle;

	/*
	** 由于定时器只能在Game线程里使用，因此为了实现一定时间后调用淡出过场加载的逻辑，我们需要自己实现定时功能。
	*/ 
	// 是否需要Tick。
	bool bIsTick = false;

	// 定时的目标时间。
	float TargetTime = .0f;

	// 定时的已经经过时间。
	float ElapsedTime = .0f;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Tick(float DeltaTime) override;

	// 加载地图前。用于做加载界面。
	void OnPreLoadMap(const FString& MapName);

	// 加载地图后。 用于取消加载界面。
	void OnPostLoadMapWithWorld(UWorld* LoadedWorld);

	// 是否可以播放过场加载界面。
	bool CanPlayLoadingScreen() const;

	// 当加载结束后，清除MoviePlayer设置。
	void OnLoadingScreenEnd();

	// 开始Tick定时。
	void StartTickTimer(float TargetTime);

	// 重置Tick定时。
	void ResetTickTimer();

	// 执行黑屏渐入渐出。
	UFUNCTION(BlueprintCallable)
	void StartFade(const float DurationTime, const bool bFadeIn = false);
};
