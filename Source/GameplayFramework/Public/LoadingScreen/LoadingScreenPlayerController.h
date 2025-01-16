// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AYRPlayerController.h"
#include "Engine/AssetManager.h"

#include "LoadingScreenPlayerController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLoadingScreenPlayerController, Log, All);

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API ALoadingScreenPlayerController : public AAYRPlayerController
{
	GENERATED_BODY()
	
private:
	// 加载界面。
	class ULoadingScreenWidget* LoadingScreenWidget = nullptr;

	// 异步加载句柄。
	TSharedPtr<FStreamableHandle> StreamableHandle;

	// 需要加载关卡的软引用。
	TSoftObjectPtr<UWorld> NewLevel;

	// 延迟加载时间。用于增加停留在加载场景的时间，预防需要加载的关卡体量太小导致“加载太快”的问题。
	const float DelayLoadingScreenTime = 1.f;

	// 延迟加载计时器是否到点。
	bool bIsTimerEnd = false;

	// 延迟加载计时器句柄。
	FTimerHandle DelayLoadingScreenTimerHandle;

	// 新关卡资产是否加载完成。
	bool bIsLoadedNewLevel = false;

public:
	ALoadingScreenPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 添加加载UI、执行异步加载新关卡资产等逻辑。
	virtual void BeginPlay() override;

	// 关卡加载完毕后的回调。
	virtual void OnNewLevelAsyncLoadingFinished();

	virtual void OpenLoadedNewLevel();
};
