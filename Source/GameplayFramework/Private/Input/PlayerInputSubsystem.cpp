// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/PlayerInputSubsystem.h"

#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"

void UPlayerInputSubsystem::AddPlayerInputMappingContext(FName InInputMappingContextID, APlayerController* InPlayerController)
{
	// 合法性检查。
	if (InInputMappingContextID.IsNone() || InPlayerController == nullptr)
	{
		return;
	}

	// 绑定Input Mapping Context。
	if (UEnhancedInputLocalPlayerSubsystem* System = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(InPlayerController->GetLocalPlayer()))
	{
		// 通过InputMappingContextID获取对应的InputMappingContext。
		UGameConfigSubsystem* Config = UGameInstance::GetSubsystem<UGameConfigSubsystem>(this->GetGameInstance());
		FPlayerInputMappingContextTableRow* InputMappingContextTableRow = nullptr;
		if (Config->GetDataTableRowFromID<FPlayerInputMappingContextTableRow>(InInputMappingContextID, InputMappingContextTableRow))
		{
			// 绑定InputAction。
			if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InPlayerController->InputComponent))
			{
				System->AddMappingContext(InputMappingContextTableRow->InputMappingContext, InputMappingContextTableRow->Priority);
			}
		}
	}
}

void UPlayerInputSubsystem::K2_BindPlayerInputAction(FName InInputActionID, APlayerController* InPlayerController, FEnhancedInputActionHandlerDynamicSignature InOnInputActionExecute)
{
	// 合法性检查。
	if (InInputActionID.IsNone() || InPlayerController == nullptr || InOnInputActionExecute.IsBound() == false)
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
			EIC->BindAction(InputActionTableRow->InputAction, InputActionTableRow->TriggerEvent, InOnInputActionExecute.GetUObject(), InOnInputActionExecute.GetFunctionName());
		}
	}
}

