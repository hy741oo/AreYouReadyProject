// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/PlayerInputSubsystem.h"

#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"

void UPlayerInputSubsystem::AddPlayerInputMappingContext(FName InInputMappingContextID)
{
	APlayerController* CurrentPlayerController = this->GetLocalPlayer()->GetPlayerController(this->GetWorld());

	// 合法性检查。
	if (InInputMappingContextID.IsNone() || CurrentPlayerController == nullptr)
	{
		return;
	}

	// 绑定Input Mapping Context。
	if (UEnhancedInputLocalPlayerSubsystem* System = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(CurrentPlayerController->GetLocalPlayer()))
	{
		// 通过InputMappingContextID获取对应的InputMappingContext。
		UGameConfigSubsystem* Config = UGameInstance::GetSubsystem<UGameConfigSubsystem>(CurrentPlayerController->GetGameInstance());
		FPlayerInputMappingContextTableRow* InputMappingContextTableRow = nullptr;
		if (Config->GetDataTableRowFromID<FPlayerInputMappingContextTableRow>(InInputMappingContextID, InputMappingContextTableRow))
		{
			// 绑定InputAction。
			if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(CurrentPlayerController->InputComponent))
			{
				System->AddMappingContext(InputMappingContextTableRow->InputMappingContext, InputMappingContextTableRow->Priority);
			}
		}
	}
}

void UPlayerInputSubsystem::RemovePlayerInputMappingContext(FName InInputMappingContextID)
{
	APlayerController* CurrentPlayerController = this->GetLocalPlayer()->GetPlayerController(this->GetWorld());

	// 合法性检查。
	if (InInputMappingContextID.IsNone() || CurrentPlayerController == nullptr)
	{
		return;
	}

	// 绑定Input Mapping Context。
	if (UEnhancedInputLocalPlayerSubsystem* System = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(CurrentPlayerController->GetLocalPlayer()))
	{
		// 通过InputMappingContextID获取对应的InputMappingContext。
		UGameConfigSubsystem* Config = UGameInstance::GetSubsystem<UGameConfigSubsystem>(CurrentPlayerController->GetGameInstance());
		FPlayerInputMappingContextTableRow* InputMappingContextTableRow = nullptr;
		if (Config->GetDataTableRowFromID<FPlayerInputMappingContextTableRow>(InInputMappingContextID, InputMappingContextTableRow))
		{
			// 绑定InputAction。
			if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(CurrentPlayerController->InputComponent))
			{
				System->RemoveMappingContext(InputMappingContextTableRow->InputMappingContext);
			}
		}
	}
}

bool UPlayerInputSubsystem::UnbindPlayerInputAction(FPlayerInputActionBindingHandle& Handle)
{
	bool bSuccess = false;

	APlayerController* CurrentPlayerController = this->GetLocalPlayer()->GetPlayerController(this->GetWorld());

	// 合法性检查。
	if (CurrentPlayerController == nullptr || Handle.InputActionBingdingHandle == 0)
	{
		return bSuccess;
	}

	// 解绑Input Action
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(CurrentPlayerController->InputComponent))
	{
		bSuccess = EIC->RemoveBindingByHandle(Handle.InputActionBingdingHandle);
	}

	// 如果解绑成功则重置句柄。
	if (bSuccess)
	{
		Handle.InputActionBingdingHandle = 0;
	}

	return bSuccess;
}

FPlayerInputActionBindingHandle UPlayerInputSubsystem::K2_BindPlayerInputAction(FName InInputActionID, FEnhancedInputActionHandlerDynamicSignature InOnInputActionExecute)
{
	FPlayerInputActionBindingHandle Handle;

	APlayerController* CurrentPlayerController = this->GetLocalPlayer()->GetPlayerController(this->GetWorld());

	// 合法性检查。
	if (InInputActionID.IsNone() || CurrentPlayerController == nullptr || InOnInputActionExecute.IsBound() == false)
	{
		return Handle;
	}

	// 通过InputActionID获取对应的InputAction。
	UGameConfigSubsystem* Config = UGameInstance::GetSubsystem<UGameConfigSubsystem>(CurrentPlayerController->GetGameInstance());
	FPlayerInputActionTableRow* InputActionTableRow = nullptr;
	if (Config->GetDataTableRowFromID<FPlayerInputActionTableRow>(InInputActionID, InputActionTableRow))
	{
		// 绑定InputAction。
		if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(CurrentPlayerController->InputComponent))
		{
			FEnhancedInputActionEventBinding& Binding = EIC->BindAction(InputActionTableRow->InputAction, InputActionTableRow->TriggerEvent, InOnInputActionExecute.GetUObject(), InOnInputActionExecute.GetFunctionName());
			Handle.InputActionBingdingHandle = Binding.GetHandle();
		}
	}

	return Handle;
}

bool UPlayerInputSubsystem::K2_UnbindPlayerInputAction(FPlayerInputActionBindingHandle& Handle)
{
	return this->UnbindPlayerInputAction(Handle);
}

