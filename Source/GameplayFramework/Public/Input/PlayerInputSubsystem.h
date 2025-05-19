// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"

#include "PlayerInputSubsystem.generated.h"

class UInputMappingContext;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnInputActionExecuteBP, const FInputActionInstance&, InputActionInstance);

/**
 * 结合增强输入（Enhanced Input）系统实现Game和UI层级的输入管理。
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UPlayerInputSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	// 测试用。
	UFUNCTION(BlueprintCallable)
	void BindActionTest(APlayerController* InPlayerController, UInputAction* InInputAction, UInputMappingContext* InInputMappingContext);
	void TestFunc(const FInputActionInstance& In);
	// 测试用End。

	// 绑定Enhanced Input Action。
	template<typename UObjectType>
	void BindPlayerInputAction(FName InActionID, APlayerController* InPlayerController, UInputAction* InInputAction, ETriggerEvent InTriggerEvent, UObjectType* InBindingObject, void(UObjectType::*InOnInputActionExecute)(const FInputActionInstance&))
	{
		// 合法性检查。
		if (InActionID.IsNone() || InPlayerController == nullptr || InInputAction == nullptr)
		{
			return;
		}

		// 绑定InputAction。
		if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InPlayerController->InputComponent))
		{
			EIC->BindAction(InInputAction, InTriggerEvent, InBindingObject, InOnInputActionExecute);
		}
	}

	// 添加Enhanced Input Mapping Context。
	UFUNCTION(BlueprintCallable)
	void AddPlayerInputMappingContext(FName InActionID, APlayerController* InPlayerController, UInputMappingContext* InInputMappingContext);
};
