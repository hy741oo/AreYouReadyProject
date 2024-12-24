// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/AYRUserWidget.h"
#include "LoadingScreenWidget.generated.h"

/**
 * 专供过场加载使用的加载屏幕Widget。
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API ULoadingScreenWidget : public UAYRUserWidget
{
	GENERATED_BODY()
	
public:
	// 播放淡入动画。
	UFUNCTION(BlueprintImplementableEvent)
	void PlayFadeinAnimation();
};
