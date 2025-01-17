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
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/*
	 *  全局渐变。
	 */

	// 执行黑屏渐入渐出。
	UFUNCTION(BlueprintCallable, Category = "World Manager|Fade")
	void StartFade(const bool bFadeIn = false,const float DurationTime = .3f) const;
	// 带蓝图委托版本。
	UFUNCTION(BlueprintCallable, Category = "World Manager|Fade")
	void StartFadeWithEvent(FOnFadeEndBPDelegate OnFadeEndBP, const bool bFadeIn = false, const float DurationTime = .3f) const;
	// 普通委托版本。供C++代码使用。
	void StartFadeWithEvent(FOnFadeEndDelegate OnFadeEnd, const bool bFadeIn = false, const float DurationTime = .3f) const;

	// 终止渐入渐出。
	UFUNCTION(BlueprintCallable, Category = "World Manager|Fade")
	void StopFade() const;

	// 中断渐入渐出。即终止并执行绑定的事件。
	UFUNCTION(BlueprintCallable, Category = "World Manager|Fade")
	void AbortFade() const;

	/*
	 *  控制关卡的加载以及加载界面关卡的生成和关卡之间的切换过渡。
	 */

	// 打开NewLevelID指定的关卡。但实际上并不是直接打开关卡， 而是加载固定的加载场景关卡，然后Async Load指定关卡的资源，等待加载完成后再执行后续的逻辑。
	UFUNCTION(BlueprintCallable, Category = "World Manager|Level")
	void OpenNewLevel(const FName NewLevelID) const;
};
