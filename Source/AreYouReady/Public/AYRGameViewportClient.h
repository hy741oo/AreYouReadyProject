// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "AYRGameViewportClient.generated.h"

/**
 * 
 */
UCLASS()
class AREYOUREADY_API UAYRGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()

private:
	// 是否正处于淡入淡出状态。
	bool bIsFading = false;

	// 为true时执行由暗到明的淡入；false时执行由明到暗的淡出。
	bool bFadeIn = false;

	// 淡入淡出持续时间。
	float DurationTime = .0f;

	// 淡入淡出已经过的时间。
	float ElapsedTime = .0f;
	
public:
	// 开始淡入淡出屏幕。
	virtual void StartFade(const float DurationTime, const bool bFadeIn = false);

	// 重置淡入淡出设置。
	virtual void ResetFade();

	virtual void PostRender(UCanvas* Canvas) override;
};
