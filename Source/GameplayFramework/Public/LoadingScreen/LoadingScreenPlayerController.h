// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AYRPlayerController.h"
#include "Engine/AssetManager.h"

#include "LoadingScreenPlayerController.generated.h"

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

public:
	ALoadingScreenPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 添加加载UI、执行异步加载新关卡资产等逻辑。
	virtual void BeginPlay() override;

	// 关卡加载完毕后的回调。
	void OnNewLevelAsyncLoadingFinished();
};
