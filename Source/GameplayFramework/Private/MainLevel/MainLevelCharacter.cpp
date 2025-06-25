// Fill out your copyright notice in the Description page of Project Settings.


#include "MainLevel/MainLevelCharacter.h"

#include "Input/PlayerInputSubsystem.h"
#include "MainLevel/MainLevelPlayerController.h"

void AMainLevelCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 绑定基础运动。
	if (AMainLevelPlayerController* OwningController = this->GetController<AMainLevelPlayerController>())
	{
		if (UPlayerInputSubsystem* PlayerInputSubsystem = ULocalPlayer::GetSubsystem<UPlayerInputSubsystem>(OwningController->GetLocalPlayer()))
		{
			// 前后运动。
			PlayerInputSubsystem->BindPlayerInputAction("MainLevel_MoveForward", this, &AMainLevelCharacter::MoveForward);

			// 左右运动。
			PlayerInputSubsystem->BindPlayerInputAction("MainLevel_MoveRight", this, &AMainLevelCharacter::MoveRight);

			// 基础运动的IMC。
			PlayerInputSubsystem->AddPlayerInputMappingContext("MainLevel_Movement");
		}
	}
}

void AMainLevelCharacter::MoveForward(const FInputActionInstance& InValue)
{
	float Value = .0f;
	if (UPlayerInputSubsystem::GetAxis1DTypeInstanceValue(InValue, Value))
	{
		this->AddMovementInput(this->GetActorForwardVector(), Value);
	}
}

void AMainLevelCharacter::MoveRight(const FInputActionInstance& InValue)
{
	float Value = .0f;
	if (UPlayerInputSubsystem::GetAxis1DTypeInstanceValue(InValue, Value))
	{
		this->AddMovementInput(this->GetActorRightVector(), Value);
	}
}

