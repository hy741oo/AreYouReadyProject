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
		UGameConfigSubsystem* Config = UGameInstance::GetSubsystem<UGameConfigSubsystem>(InPlayerController->GetGameInstance());
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

bool UPlayerInputSubsystem::UnbindPlayerInputAction(APlayerController* InPlayerController, FPlayerInputActionBindingHandle Handle)
{
	bool bSuccess = false;

	// 合法性检查。
	if (InPlayerController == nullptr || Handle.InputActionBingdingHandle == 0)
	{
		return bSuccess;
	}

	// 解绑Input Action
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InPlayerController->InputComponent))
	{
		bSuccess = EIC->RemoveBindingByHandle(Handle.InputActionBingdingHandle);
	}

	return bSuccess;
}

FPlayerInputActionBindingHandle UPlayerInputSubsystem::K2_BindPlayerInputAction(FName InInputActionID, APlayerController* InPlayerController, FEnhancedInputActionHandlerDynamicSignature InOnInputActionExecute)
{
	FPlayerInputActionBindingHandle Handle;

	// 合法性检查。
	if (InInputActionID.IsNone() || InPlayerController == nullptr || InOnInputActionExecute.IsBound() == false)
	{
		return Handle;
	}

	// 通过InputActionID获取对应的InputAction。
	UGameConfigSubsystem* Config = UGameInstance::GetSubsystem<UGameConfigSubsystem>(InPlayerController->GetGameInstance());
	FPlayerInputActionTableRow* InputActionTableRow = nullptr;
	if (Config->GetDataTableRowFromID<FPlayerInputActionTableRow>(InInputActionID, InputActionTableRow))
	{
		// 绑定InputAction。
		if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InPlayerController->InputComponent))
		{
			FEnhancedInputActionEventBinding& Binding = EIC->BindAction(InputActionTableRow->InputAction, InputActionTableRow->TriggerEvent, InOnInputActionExecute.GetUObject(), InOnInputActionExecute.GetFunctionName());
			Handle.InputActionBingdingHandle = Binding.GetHandle();
		}
	}

	return Handle;
}

bool UPlayerInputSubsystem::K2_UnbindPlayerInputAction(APlayerController* InPlayerController, FPlayerInputActionBindingHandle Handle)
{
	return this->UnbindPlayerInputAction(InPlayerController, Handle);
}

