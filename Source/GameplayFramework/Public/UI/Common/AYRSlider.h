// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Slider.h"
#include "AYRSlider.generated.h"

/**
 * 自定义的滑动条控件。
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UAYRSlider : public USlider
{
	GENERATED_BODY()
	
	// 让滑动条的值增加一个进步值。
	UFUNCTION(BlueprintCallable, Category = "AYRSlider")
	void AddValueByStep();

	UFUNCTION(BlueprintCallable, Category = "AYRSlider")
	// 让滑动条的值减少一个进步值。
	void SubtractValueByStep();

};
