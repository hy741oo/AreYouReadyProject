// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "GameConfigSubsystem.h"

#include "Kismet/GameplayStatics.h"

#include "PlayerInputSubsystem.generated.h"

class UInputMappingContext;

// Input Action数据表。
USTRUCT(BlueprintType)
struct FPlayerInputActionTableRow : public FAYRTableRowBase
{
	GENERATED_BODY()

	// Input Action资产。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UInputAction* InputAction = nullptr;

	// Input Action的触发方式。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	ETriggerEvent TriggerEvent = ETriggerEvent::Started;
};

// Input Mapping Context数据表。
USTRUCT(BlueprintType)
struct FPlayerInputMappingContextTableRow : public FAYRTableRowBase
{
	GENERATED_BODY()

	// Input Mapping Context资产。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UInputMappingContext* InputMappingContext = nullptr;

	// 优先级。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 Priority = 0;

};

/**
 * 结合增强输入（Enhanced Input）系统实现Game和UI层级的输入管理。
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UPlayerInputSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	// 绑定Enhanced Input Action。
	template<typename UObjectType>
	void BindPlayerInputAction(FName InInputActionID, APlayerController* InPlayerController, UObjectType* InBindingObject, void(UObjectType::*InOnInputActionExecute)(const FInputActionInstance&))
	{
		// 合法性检查。
		if (InInputActionID.IsNone() || InPlayerController == nullptr || InBindingObject == nullptr || InOnInputActionExecute == nullptr)
		{
			return;
		}

		// 通过InputActionID获取对应的InputAction。
		UGameConfigSubsystem* Config = UGameInstance::GetSubsystem<UGameConfigSubsystem>(this->GetGameInstance());
		FPlayerInputActionTableRow* InputActionTableRow = nullptr;
		if (Config->GetDataTableRowFromID<FPlayerInputActionTableRow>(InInputActionID, InputActionTableRow))
		{
			// 绑定InputAction。
			if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InPlayerController->InputComponent))
			{
				EIC->BindAction(InputActionTableRow->InputAction, InputActionTableRow->TriggerEvent, InBindingObject, InOnInputActionExecute);
			}
		}
	}

	// 添加Enhanced Input Mapping Context。
	UFUNCTION(BlueprintCallable)
	void AddPlayerInputMappingContext(FName InInputMappingContextID, APlayerController* InPlayerController);
};
