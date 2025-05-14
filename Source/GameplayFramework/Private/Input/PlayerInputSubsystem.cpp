// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/PlayerInputSubsystem.h"

#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void UPlayerInputSubsystem::BindActionTest(APlayerController* InPlayerController, UInputAction* InInputAction, UInputMappingContext* InInputMappingContext)
{
	if (!InPlayerController || !InInputAction || !InInputMappingContext)
	{
		return;
	}

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InPlayerController->InputComponent))
	{
		EIC->BindAction(InInputAction, ETriggerEvent::Started, this, &UPlayerInputSubsystem::TestFunc);
	}

	if (UEnhancedInputLocalPlayerSubsystem* System = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(InPlayerController->GetLocalPlayer()))
	{
		System->AddMappingContext(InInputMappingContext, 0);
	}
}

void UPlayerInputSubsystem::TestFunc()
{
	UE_LOG(LogTemp, Warning, TEXT("asdasdasdsad"));
}

