// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/PlayerInputSubsystem.h"

#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"

void UPlayerInputSubsystem::BindActionTest(APlayerController* InPlayerController, UInputAction* InInputAction, UInputMappingContext* InInputMappingContext)
{
	if (!InPlayerController || !InInputAction || !InInputMappingContext)
	{
		return;
	}

	this->BindPlayerInputAction("asdasd", InPlayerController, InInputAction, ETriggerEvent::Started, this, &UPlayerInputSubsystem::TestFunc);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InPlayerController->InputComponent))
	{
		EIC->BindAction(InInputAction, ETriggerEvent::Started, this, &UPlayerInputSubsystem::TestFunc);
	}

	if (UEnhancedInputLocalPlayerSubsystem* System = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(InPlayerController->GetLocalPlayer()))
	{
		System->AddMappingContext(InInputMappingContext, 0);
	}
}

void UPlayerInputSubsystem::TestFunc(const FInputActionInstance& In)
{
	UE_LOG(LogTemp, Warning, TEXT("asdasdasdsad"));
}

void UPlayerInputSubsystem::AddPlayerInputMappingContext(FName InActionID, APlayerController* InPlayerController, UInputMappingContext* InInputMappingContext)
{
	if (UEnhancedInputLocalPlayerSubsystem* System = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(InPlayerController->GetLocalPlayer()))
	{
		System->AddMappingContext(InInputMappingContext, 0);
	}
}

