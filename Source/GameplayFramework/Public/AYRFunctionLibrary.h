// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AYRInputProcessor.h"

#include "AYRFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UAYRFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// 通过ID获取指定输入设备下的输入按键图标信息。
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Game Config|Input Device", Meta = (WorldContext = "InWorldContextObject"))
	static bool GetInputIconBrush(const UObject* InWorldContextObject, const FName InInputName, FSlateBrush& OutInputIconBrush, const EInputDeviceType::Type InInputDeviceType = EInputDeviceType::IDT_KeyboardAndMouse);

	// 通过ID获取当前输入设备下的输入案件图标信息。
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Game Config|Input Device", Meta = (WorldContext = "InWorldContextObject"))
	static bool GetCurrentInputIconBrush(const UObject* InWorldContextObject, const FName InInputName, FSlateBrush& OutInputIconBrush);
};
