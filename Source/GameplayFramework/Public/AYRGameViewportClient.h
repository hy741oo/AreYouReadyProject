// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "AYRGameViewportClient.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnFadeEndBPDelegate);

DECLARE_DELEGATE(FOnFadeEndDelegate);

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UAYRGameViewportClient : public UGameViewportClient
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

	// 当淡入淡出结束后需要调用的委托。
	FOnFadeEndDelegate OnFadeEnd;
	// 蓝图版本委托。
	FOnFadeEndBPDelegate OnFadeEndBP;

protected:
	// 执行Fade后的绑定事件。
	virtual void ExecuteEvent();
	
public:
	// 开始淡入淡出屏幕。无委托版本。
	virtual void StartFade(const bool bFadeIn = false,const float DurationTime = .3f);
	// 蓝图委托版本的渐变。
	virtual void StartFadeWithEvent(FOnFadeEndBPDelegate OnFadeEndBP, const bool bFadeIn = false, const float DurationTime = .3f);
	// 普通委托版本的渐变。
	virtual void StartFadeWithEvent(FOnFadeEndDelegate OnFadeEnd, const bool bFadeIn = false, const float DurationTime = .3f);

	// 重置淡入淡出设置。
	virtual void StopFade();

	// 中断淡入淡出。中断屏幕渐变并执行绑定的委托（如果有）。
	virtual void AbortFade();

	virtual void PostRender(UCanvas* Canvas) override;

	// 是否正处于淡入淡出状态。
	bool IsFading() const
	{
		return this->bIsFading;
	}
};
