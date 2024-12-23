// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
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
	// 判断是否为开屏加载，如果是的话则不执行过场加载操作，防止过场加载的LoadingScreenAttributes覆盖了开屏加载设置。
	bool bIsFirst = true;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// 加载地图前。用于做加载界面。
	void OnPreLoadMap(const FString& MapName);

	// 加载地图后。 用于取消加载界面。
	void OnPostLoadMapWithWorld(UWorld* LoadedWorld);

	// 是否可以播放过场加载界面。
	bool CanPlayLoadingScreen();
};
