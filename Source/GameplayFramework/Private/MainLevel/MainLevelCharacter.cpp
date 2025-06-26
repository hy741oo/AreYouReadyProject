// Fill out your copyright notice in the Description page of Project Settings.


#include "MainLevel/MainLevelCharacter.h"

#include "Input/PlayerInputSubsystem.h"
#include "MainLevel/MainLevelPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"

AMainLevelCharacter::AMainLevelCharacter(const FObjectInitializer& InObjectInitializer)
{
	// 生成摄像机。
	this->PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Character Camera"));
	if (this->PlayerCamera)
	{
		this->PlayerCamera->SetupAttachment(this->GetRootComponent());
		this->PlayerCamera->SetRelativeLocation(this->GetPawnViewLocation());
	}

	// 设置胶囊体尺寸。
	if (UCapsuleComponent* PlayerCapsule = this->GetCapsuleComponent())
	{
		PlayerCapsule->SetCapsuleRadius(42);
		PlayerCapsule->SetCapsuleHalfHeight(96);
	}
}

void AMainLevelCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 绑定基础运动。
	if (AMainLevelPlayerController* OwningController = this->GetController<AMainLevelPlayerController>())
	{
		if (UPlayerInputSubsystem* PlayerInputSubsystem = ULocalPlayer::GetSubsystem<UPlayerInputSubsystem>(OwningController->GetLocalPlayer()))
		{
			// 前后移动。
			PlayerInputSubsystem->BindPlayerInputAction("MainLevel_MoveForward", this, &AMainLevelCharacter::MoveForward);

			// 左右移动。
			PlayerInputSubsystem->BindPlayerInputAction("MainLevel_MoveRight", this, &AMainLevelCharacter::MoveRight);

			// 左右移动镜头。
			PlayerInputSubsystem->BindPlayerInputAction("MainLevel_LookAround", this, &AMainLevelCharacter::LookAround);

			// 上下移动镜头。
			PlayerInputSubsystem->BindPlayerInputAction("MainLevel_LookUp", this, &AMainLevelCharacter::LookUp);

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
void AMainLevelCharacter::LookAround(const FInputActionInstance& InValue)
{
	float Value = .0f;
	if (UPlayerInputSubsystem::GetAxis1DTypeInstanceValue(InValue, Value))
	{
		this->AddControllerYawInput(Value);
	}
}

void AMainLevelCharacter::LookUp(const FInputActionInstance& InValue)
{
	float Value = .0f;
	if (UPlayerInputSubsystem::GetAxis1DTypeInstanceValue(InValue, Value))
	{
		if (this->PlayerCamera)
		{
			FRotator LastRotator = this->PlayerCamera->GetRelativeRotation();
			Value = FMath::Clamp(LastRotator.Pitch + Value, -70.f, 70.f);
			FRotator NewRotator(Value, .0f, .0f);
			this->PlayerCamera->SetRelativeRotation(NewRotator);
		}
	}
}

