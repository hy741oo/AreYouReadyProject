// Fill out your copyright notice in the Description page of Project Settings.


#include "MainLevel/MainLevelCharacter.h"

#include "Input/PlayerInputSubsystem.h"
#include "MainLevel/MainLevelPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/Interactive/InteractableObjectInterface.h"

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

	// 设置运动组件。
	if (UCharacterMovementComponent* MC = this->GetCharacterMovement())
	{
		MC->MaxAcceleration = 700.f;
		MC->BrakingFrictionFactor = 1.f;
		MC->MaxWalkSpeed = 300.f;
		MC->BrakingDecelerationWalking = 1000.f;
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

void AMainLevelCharacter::EndPlay(const EEndPlayReason::Type InEndPlayReason)
{
	Super::EndPlay(InEndPlayReason);

	// 清理可交互对象。
	if (IInteractableObjectInterface* OldActor = Cast<IInteractableObjectInterface>(this->InteractableActor.Get()))
	{
		OldActor->Execute_LeaveInteractableState(this->InteractableActor.Get());
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

void AMainLevelCharacter::Tick(float InDeltaTime)
{
	TArray<FHitResult> HitResults;
	FVector CameraLocationInWorld = this->PlayerCamera->GetComponentLocation();
	// 球形射线初始位置。位于摄像机中点。
	FVector StartLocation = CameraLocationInWorld;
	// 球形射线终止位置。位于摄像机正前方CapsuleTraceDistance距离。
	FVector EndLocation = CameraLocationInWorld + this->PlayerCamera->GetForwardVector() * this->SphereTraceDistance;
	// 球形射线检测可交互Actor。
	UKismetSystemLibrary::SphereTraceMulti(this, StartLocation, EndLocation, this->SphereTraceRadius, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, HitResults, true);

	bool bGetInteractableActor = false;
	if (HitResults.Num() > 0)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			AActor* HitActor = HitResult.GetActor();
			if (HitActor && HitActor->GetClass()->ImplementsInterface(UInteractableObjectInterface::StaticClass()))
			{
				// 即使实现了接口，如果不可交互的话需要跳过该Actor。
				if (!IInteractableObjectInterface::Execute_IsInteractable(HitResult.Actor.Get()))
				{
					continue;
				}

				// 判断当前可交互对象是否和上次检测到的对象一致。
				if (HitResult.Actor == this->InteractableActor)
				{
					bGetInteractableActor = true;
					break;
				}
				else
				{
					if (AActor* OldActor = this->InteractableActor.Get())
					{
						IInteractableObjectInterface::Execute_LeaveInteractableState(this->InteractableActor.Get());
					}

					this->InteractableActor = HitActor;
					IInteractableObjectInterface::Execute_EnterInteractableState(HitActor);

					bGetInteractableActor = true;
					break;
				}
			}
		}
	}

	// 如果在上面的逻辑中没有找到可交互物，或者射线检测没有检测到任何物体，则需要将之前检测到的可交互物脱离交互状态。
	if (!bGetInteractableActor)
	{
		// 如果射线检测不再能够检测到
		if (AActor* OldActor = this->InteractableActor.Get())
		{
			IInteractableObjectInterface::Execute_LeaveInteractableState(OldActor);
			this->InteractableActor.Reset();
		}
	}
}

