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
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// 加载地图前。用于做加载界面。
	void OnPreLoadMap(const FString& MapName);

	// 加载地图后。 用于取消加载界面。
	void OnPostLoadMapWithWorld(UWorld* LoadedWorld);
};
