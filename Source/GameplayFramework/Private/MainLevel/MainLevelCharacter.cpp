// Fill out your copyright notice in the Description page of Project Settings.


#include "MainLevel/MainLevelCharacter.h"

#include "Input/PlayerInputSubsystem.h"
#include "MainLevel/MainLevelPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/Interactive/InteractableObjectInterface.h"
#include "Component/GeneralStateMachine/GeneralStateMachineComponent.h"

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

	// 生成状态机组件。
	this->GeneralStateMachineComponent = CreateDefaultSubobject<UGeneralStateMachineComponent>(TEXT("State Mechine"));
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

			// 松开移动按键。
			PlayerInputSubsystem->BindPlayerInputAction("MainLevel_ReleaseMoveButton", this, &AMainLevelCharacter::OnMoveButtonReleased);

			// 上下移动镜头。
			PlayerInputSubsystem->BindPlayerInputAction("MainLevel_LookUp", this, &AMainLevelCharacter::LookUp);

			// 奔跑。
			PlayerInputSubsystem->BindPlayerInputAction("MainLevel_Run", this, &AMainLevelCharacter::Run);

			// 停止奔跑。
			PlayerInputSubsystem->BindPlayerInputAction("MainLevel_StopRun", this, &AMainLevelCharacter::StopRun);

			// 基础运动的IMC。
			PlayerInputSubsystem->AddPlayerInputMappingContext("MainLevel_Movement");
		}
	}

	// 初始化状态机。
	this->InitializeGeneralStateMachine();
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
	if (this->GeneralStateMachineComponent->ChangeStateTo("Walk"))
	{
		float Value = .0f;
		if (UPlayerInputSubsystem::GetAxis1DTypeInstanceValue(InValue, Value))
		{
			this->AddMovementInput(this->GetActorForwardVector(), Value);
		}
	}
}

void AMainLevelCharacter::MoveRight(const FInputActionInstance& InValue)
{
	if (this->GeneralStateMachineComponent->ChangeStateTo("Walk"))
	{
		float Value = .0f;
		if (UPlayerInputSubsystem::GetAxis1DTypeInstanceValue(InValue, Value))
		{
			this->AddMovementInput(this->GetActorRightVector(), Value);
		}
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

/* 状态机相关--------------------Begin*/
void AMainLevelCharacter::OnEnterIdleState()
{
	// 设置Idle状态。
	if (UCharacterMovementComponent* MC = this->GetCharacterMovement())
	{
		MC->MaxAcceleration = 700.f;
		MC->BrakingFrictionFactor = 1.f;
		MC->MaxWalkSpeed = 300.f;
		MC->BrakingDecelerationWalking = 1000.f;
	}
}

void AMainLevelCharacter::OnEnterWalkState()
{
	// 设置Walk状态。
	if (UCharacterMovementComponent* MC = this->GetCharacterMovement())
	{
		MC->MaxAcceleration = 700.f;
		MC->BrakingFrictionFactor = 1.f;
		MC->MaxWalkSpeed = 300.f;
		MC->BrakingDecelerationWalking = 1000.f;
	}

}

void AMainLevelCharacter::OnEnterRunState()
{
	// 设置Run状态。
	if (UCharacterMovementComponent* MC = this->GetCharacterMovement())
	{
		MC->MaxAcceleration = 1000.f;
		MC->BrakingFrictionFactor = 1.f;
		MC->MaxWalkSpeed = 400.f;
		MC->BrakingDecelerationWalking = 1000.f;
	}
}

void AMainLevelCharacter::OnEnterJumpState()
{
	// 设置Jump状态。
	this->Jump();
}

void AMainLevelCharacter::InitializeGeneralStateMachine()
{
	// 创建状态节点，并绑定激活节点时执行的逻辑。
	FGeneralStateMachineNode& IdleState = this->GeneralStateMachineComponent->CreateStateMachineNode("Idle");
	IdleState.OnEnterState.BindUObject(this, &AMainLevelCharacter::OnEnterIdleState);

	FGeneralStateMachineNode& WalkState = this->GeneralStateMachineComponent->CreateStateMachineNode("Walk");
	WalkState.OnEnterState.BindUObject(this, &AMainLevelCharacter::OnEnterWalkState);

	FGeneralStateMachineNode& RunState = this->GeneralStateMachineComponent->CreateStateMachineNode("Run");
	RunState.OnEnterState.BindUObject(this, &AMainLevelCharacter::OnEnterRunState);

	FGeneralStateMachineNode& JumpState = this->GeneralStateMachineComponent->CreateStateMachineNode("Jump");
	JumpState.OnEnterState.BindUObject(this, &AMainLevelCharacter::OnEnterJumpState);

	// Idle状态可以切换到Idle、Walk和Jump状态且不需要任何判断。
	FGeneralStateMachineCondition IdleToIdle;
	IdleToIdle.NextStateName = "Idle";
	IdleToIdle.Condition.BindLambda(
		[]() {
			return true;
		}
	);
	IdleState.NextStates.Add("Idle", IdleToIdle);
	FGeneralStateMachineCondition IdleToWalk;
	IdleToWalk.NextStateName = "Walk";
	IdleToWalk.Condition.BindLambda(
		[]() {
			return true;
		}
	);
	IdleState.NextStates.Add("Walk", IdleToWalk);
	FGeneralStateMachineCondition IdleToJump;
	IdleToJump.NextStateName = "Jump";
	IdleToJump.Condition.BindLambda(
		[]() {
			return true;
		}
	);
	IdleState.NextStates.Add("Jump", IdleToJump);

	// Walk状态可以切换到Walk、Idle和Jump状态。
	FGeneralStateMachineCondition WalkToWalk;
	WalkToWalk.NextStateName = "Walk";
	WalkToWalk.Condition.BindLambda(
		[]() {
			return true;
		}
	);
	WalkState.NextStates.Add("Walk", WalkToWalk);
	FGeneralStateMachineCondition WalkToIdle;
	WalkToIdle.NextStateName = "Idle";
	WalkToIdle.Condition.BindLambda(
		[]() {
			return true;
		}
	);
	WalkState.NextStates.Add("Idle", WalkToIdle);
	FGeneralStateMachineCondition WalkToJump;
	WalkToJump.NextStateName = "Jump";
	WalkToJump.Condition.BindLambda(
		[]() {
			return true;
		}
	);
	WalkState.NextStates.Add("Jump", WalkToJump);

	// Jump状态可以切换到Idle状态，即落地的一瞬间。
	FGeneralStateMachineCondition JumpToIdle;
	JumpToIdle.NextStateName = "Idle";
	JumpToIdle.Condition.BindLambda(
		[]() {
			return true;
		}
	);
	JumpState.NextStates.Add("Idle", JumpToIdle);

	// 初始化状态机，选择一个状态机节点作为最初始的状态。
	this->GeneralStateMachineComponent->InitGeneralStateMachine("Idle");
}
/* 状态机相关--------------------End*/

void AMainLevelCharacter::OnMoveButtonReleased(const FInputActionInstance& InValue)
{
	this->GeneralStateMachineComponent->ChangeStateTo("Idle");
}

void AMainLevelCharacter::Run(const FInputActionInstance& InValue)
{
}

void AMainLevelCharacter::StopRun(const FInputActionInstance& InValue)
{
}

