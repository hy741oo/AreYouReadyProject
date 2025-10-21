// Fill out your copyright notice in the Description page of Project Settings.


#include "MainLevel/MainLevelCharacter.h"

#include "Input/PlayerInputSubsystem.h"
#include "MainLevel/MainLevelPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/Interactive/InteractableObjectInterface.h"
#include "Component/GeneralStateMachine/GeneralStateMachineComponent.h"
#include "Component/Camera/AYRCameraComponent.h"
#include "Camera/AYRPlayerCameraManager.h"
#include "UISubsystem.h"
#include "UI/MainLevel/PlayerHUD.h"
#include "Blueprint/WidgetLayoutLibrary.h"

AMainLevelCharacter::AMainLevelCharacter(const FObjectInitializer& InObjectInitializer)
{
	// 生成摄像机。
	this->PlayerCamera = CreateDefaultSubobject<UAYRCameraComponent>(TEXT("Character Camera"));
	if (this->PlayerCamera)
	{
		this->PlayerCamera->InitCameraInfo("Default");
		this->PlayerCamera->SetupAttachment(this->GetRootComponent());
		this->PlayerCamera->SetRelativeLocation(this->GetPawnViewLocation());
	}

	// 设置胶囊体尺寸。
	if (UCapsuleComponent* PlayerCapsule = this->GetCapsuleComponent())
	{
		PlayerCapsule->SetCapsuleRadius(40);
		PlayerCapsule->SetCapsuleHalfHeight(92);
	}

	// 生成运动状态机组件。
	this->MovementStateMachineComponent = CreateDefaultSubobject<UGeneralStateMachineComponent>(TEXT("Movement State Machine"));

	// 生成移动数据状态机组件。
	this->MovementDataStateMachineComponent = CreateDefaultSubobject<UGeneralStateMachineComponent>(TEXT("Movement Data State Machine"));
}

void AMainLevelCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AMainLevelPlayerController* OwningController = this->GetController<AMainLevelPlayerController>())
	{
		// 绑定基础运动。
		if (UPlayerInputSubsystem* PlayerInputSubsystem = ULocalPlayer::GetSubsystem<UPlayerInputSubsystem>(OwningController->GetLocalPlayer()))
		{
			// 移动。
			PlayerInputSubsystem->BindPlayerInputAction("MainLevel_Move", this, &AMainLevelCharacter::Move);
			// 停止移动。
			PlayerInputSubsystem->BindPlayerInputAction("MainLevel_OnMoveStopped", this, &AMainLevelCharacter::OnMoveStopped);

			// 旋转镜头。
			PlayerInputSubsystem->BindPlayerInputAction("MainLevel_LookAround", this, &AMainLevelCharacter::LookAround);

			// 奔跑。
			PlayerInputSubsystem->BindPlayerInputAction("MainLevel_Run", this, &AMainLevelCharacter::Run);
			// 停止奔跑。
			PlayerInputSubsystem->BindPlayerInputAction("MainLevel_OnRunStopped", this, &AMainLevelCharacter::OnRunStopped);

			// 跳跃。
			PlayerInputSubsystem->BindPlayerInputAction("MainLevel_Jump", this, &AMainLevelCharacter::CharacterJump);
			// 停止跳跃。
			PlayerInputSubsystem->BindPlayerInputAction("MainLevel_OnJumpStopped", this, &AMainLevelCharacter::OnCharacterJumpStopped);

			// 基础运动的IMC。
			PlayerInputSubsystem->AddPlayerInputMappingContext("MainLevel_Movement");

			// 交互。
			PlayerInputSubsystem->BindPlayerInputAction("MainLevel_Interact", this, &AMainLevelCharacter::Interact);

			// 基础动作（如交互）的IMC。
			PlayerInputSubsystem->AddPlayerInputMappingContext("MainLevel_BaseAction");
		}

		this->OnPlayerCameraManagerUpdatedHandle = OwningController->OnPlayerCameraManagerUpdateDelegate.AddUObject(this, &AMainLevelCharacter::OnPlayerCameraManagerUpdated);
	}

	// 初始化状态机。
	this->InitializeGeneralStateMachine();

	// 添加HUD。
	UUISubsystem* UISubsystem = ULocalPlayer::GetSubsystem<UUISubsystem>(this->GetController<AMainLevelPlayerController>()->GetLocalPlayer());
	this->PlayerHUD = CastChecked<UPlayerHUD>(UISubsystem->PushUI("PlayerHUD"));
}

void AMainLevelCharacter::Destroyed()
{
	// 清理可交互对象。
	if (IInteractableObjectInterface* OldActor = Cast<IInteractableObjectInterface>(this->InteractableActor.Get()))
	{
		OldActor->Execute_LeaveInteractableState(this->InteractableActor.Get());
	}

	// 移除HUD。
	UUISubsystem* UISubsystem = ULocalPlayer::GetSubsystem<UUISubsystem>(this->GetController<AMainLevelPlayerController>()->GetLocalPlayer());
	UISubsystem->PopUI(this->PlayerHUD);

	if (AMainLevelPlayerController* PC = this->GetController<AMainLevelPlayerController>())
	{
		PC->OnPlayerCameraManagerUpdateDelegate.Remove(this->OnPlayerCameraManagerUpdatedHandle);
		this->OnPlayerCameraManagerUpdatedHandle.Reset();
	}
}

void AMainLevelCharacter::Move(const FInputActionInstance& InValue)
{
	if (this->MovementStateMachineComponent->ChangeStateTo("Walk"))
	{
		FVector2D Value = .0f;
		if (UPlayerInputSubsystem::GetAxis2DTypeInstanceValue(InValue, Value))
		{
			FVector Direction(Value, .0f);
			Direction = this->GetActorRotation().RotateVector(Direction);
			this->AddMovementInput(Direction, Value.Size());
		}
	}
}

void AMainLevelCharacter::LookAround(const FInputActionInstance& InValue)
{
	FVector2D Value = .0f;
	if (UPlayerInputSubsystem::GetAxis2DTypeInstanceValue(InValue, Value))
	{
		this->AddControllerYawInput(Value.X);
		this->AddControllerPitchInput(Value.Y);
	}
}

void AMainLevelCharacter::Tick(float InDeltaTime)
{
	Super::Tick(InDeltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////状态机相关Begin
void AMainLevelCharacter::OnEnterNormalSpeedState()
{
	// 设置Walk状态。
	if (UCharacterMovementComponent* MC = this->GetCharacterMovement())
	{
		MC->MaxAcceleration = 700.f;
		MC->BrakingFrictionFactor = 1.f;
		MC->MaxWalkSpeed = 300.f;
		MC->BrakingDecelerationWalking = 1000.f;
	}

	this->PlayerCamera->ChangeCameraTo("Default");
}

void AMainLevelCharacter::OnEnterHighSpeedState()
{
	// 设置Run状态。
	if (UCharacterMovementComponent* MC = this->GetCharacterMovement())
	{
		MC->MaxAcceleration = 1000.f;
		MC->BrakingFrictionFactor = 1.f;
		MC->MaxWalkSpeed = 700.f;
		MC->BrakingDecelerationWalking = 1000.f;
	}

	// 切换摄像机。
	this->PlayerCamera->ChangeCameraTo("Running");
}

void AMainLevelCharacter::OnEnterJumpState()
{
	// 设置Jump状态。
	this->Jump();
}

void AMainLevelCharacter::InitializeGeneralStateMachine()
{
	// 创建状态节点。
	FGeneralStateMachineNode& IdleState = this->MovementStateMachineComponent->CreateStateMachineNode("Idle");
	IdleState.OnEnterState.BindUObject(this, &AMainLevelCharacter::OnEnterIdleState);
	IdleState.OnTickState.BindLambda(
		[this](const float& InDeltaTime, const float& InTickingElapsedTime) {
			this->StartCameraShake("IdleCameraShake");
		}
	);
	IdleState.OnLeaveState.BindLambda(
		[this]() {
			this->StopCameraShake("IdleCameraShake");
		}
	);
	FGeneralStateMachineNode& WalkState = this->MovementStateMachineComponent->CreateStateMachineNode("Walk");
	WalkState.OnEnterState.BindLambda(
		[this]()
		{
			if (this->MovementDataStateMachineComponent->IsSameState("HighSpeed"))
			{
				this->PlayerCamera->ChangeCameraTo("Running");
			}
		}
	);
	WalkState.OnUpdateState.BindLambda(
		[this]() {
			if (this->MovementDataStateMachineComponent->IsSameState("HighSpeed"))
			{
				this->StartCameraShake("RunningCameraShake");
			}
			else
			{
				this->StartCameraShake("WalkingCameraShake");
			}
		}
	);
	WalkState.OnLeaveState.BindLambda(
		[this]() {
			this->StopCameraShake("WalkingCameraShake");
			this->StopCameraShake("RunningCameraShake");
		}
	);
	FGeneralStateMachineNode& JumpState = this->MovementStateMachineComponent->CreateStateMachineNode("Jump");
	JumpState.OnEnterState.BindUObject(this, &AMainLevelCharacter::OnEnterJumpState);
	FGeneralStateMachineNode& JumpLandedState = this->MovementStateMachineComponent->CreateStateMachineNode("JumpLanded");
	JumpLandedState.OnEnterState.BindLambda(
		[this]() {
			this->MovementStateMachineComponent->ChangeStateTo("Idle");
			this->MovementDataStateMachineComponent->ChangeStateTo("NormalSpeed");
		}
	);

	// Idle状态可以切换到Idle、Walk和Jump状态且不需要任何判断。
	FGeneralStateMachineCondition IdleToIdle;
	IdleToIdle.NextStateName = "Idle";
	IdleState.NextStates.Add("Idle", IdleToIdle);
	FGeneralStateMachineCondition IdleToWalk;
	IdleToWalk.NextStateName = "Walk";
	IdleState.NextStates.Add("Walk", IdleToWalk);
	FGeneralStateMachineCondition IdleToJump;
	IdleToJump.NextStateName = "Jump";
	IdleState.NextStates.Add("Jump", IdleToJump);

	// Walk状态可以切换到Walk、Idle和Jump状态。
	FGeneralStateMachineCondition WalkToWalk;
	WalkToWalk.NextStateName = "Walk";
	WalkState.NextStates.Add("Walk", WalkToWalk);
	FGeneralStateMachineCondition WalkToIdle;
	WalkToIdle.NextStateName = "Idle";
	WalkToIdle.Condition.BindLambda(
		[this]() {
			return !this->MovementStateMachineComponent->IsSameState("Jump");
		}
	);
	WalkState.NextStates.Add("Idle", WalkToIdle);
	FGeneralStateMachineCondition WalkToJump;
	WalkToJump.NextStateName = "Jump";
	WalkState.NextStates.Add("Jump", WalkToJump);

	// Jump状态唯一可以切换的状态是JumpLanded，即落地的一瞬间。
	FGeneralStateMachineCondition JumpToJumpLanded;
	JumpToJumpLanded.NextStateName = "JumpLanded";
	JumpState.NextStates.Add("JumpLanded", JumpToJumpLanded);

	// JumpLanded可以转换到Idle和Walk状态。
	FGeneralStateMachineCondition JumpLandedToIdle;
	JumpLandedToIdle.NextStateName = "Idle";
	JumpLandedState.NextStates.Add("Idle", JumpLandedToIdle);
	FGeneralStateMachineCondition JumpLandedToWalk;
	JumpLandedToWalk.NextStateName = "Walk";
	JumpLandedState.NextStates.Add("Walk", JumpLandedToWalk);

	// 初始化状态机，选择一个状态机节点作为最初始的状态。
	this->MovementStateMachineComponent->InitGeneralStateMachine("Idle");

	// 创建移动数据状态节点并绑定。
	FGeneralStateMachineNode& NormalSpeedState = this->MovementDataStateMachineComponent->CreateStateMachineNode("NormalSpeed");
	NormalSpeedState.OnEnterState.BindUObject(this, &AMainLevelCharacter::OnEnterNormalSpeedState);
	FGeneralStateMachineNode& HighSpeedState = this->MovementDataStateMachineComponent->CreateStateMachineNode("HighSpeed");
	HighSpeedState.OnEnterState.BindUObject(this, &AMainLevelCharacter::OnEnterHighSpeedState);

	FGeneralStateMachineCondition NormalSpeedToHighSpeed;
	NormalSpeedToHighSpeed.NextStateName = "HighSpeed";
	NormalSpeedToHighSpeed.Condition.BindWeakLambda(this, 
		[this]() {
			return this->MovementStateMachineComponent->IsSameState("Walk");
		}
		);
	NormalSpeedState.NextStates.Add("HighSpeed", NormalSpeedToHighSpeed);

	FGeneralStateMachineCondition HighSpeedToNormalSpeed;
	HighSpeedToNormalSpeed.NextStateName = "NormalSpeed";
	HighSpeedToNormalSpeed.Condition.BindLambda(
		[this]() {
			return !this->MovementStateMachineComponent->IsSameState("Jump");
		}
	);
	HighSpeedState.NextStates.Add("NormalSpeed", HighSpeedToNormalSpeed);

	FGeneralStateMachineCondition HighSpeedToHighSpeed;
	HighSpeedToHighSpeed.NextStateName = "HighSpeed";
	HighSpeedToHighSpeed.Condition.BindLambda(
		[this]() {
			return this->MovementStateMachineComponent->IsSameState("Walk");
		}
	);
	HighSpeedState.NextStates.Add("HighSpeed", HighSpeedToHighSpeed);

	this->MovementDataStateMachineComponent->InitGeneralStateMachine("NormalSpeed");
}

void AMainLevelCharacter::OnMoveStopped(const FInputActionInstance& InValue)
{
	this->MovementStateMachineComponent->ChangeStateTo("Idle");
}

void AMainLevelCharacter::Run(const FInputActionInstance& InValue)
{
	this->MovementDataStateMachineComponent->ChangeStateTo("HighSpeed");
}

void AMainLevelCharacter::OnRunStopped(const FInputActionInstance& InValue)
{
	if (!this->MovementStateMachineComponent->IsSameState("Jump"))
	{
		this->MovementDataStateMachineComponent->ChangeStateTo("NormalSpeed");
	}
}

void AMainLevelCharacter::CharacterJump(const FInputActionInstance& InValue)
{
	this->MovementStateMachineComponent->ChangeStateTo("Jump");
}

void AMainLevelCharacter::OnCharacterJumpStopped(const FInputActionInstance& InValue)
{
	this->StopJumping();
}

void AMainLevelCharacter::OnEnterIdleState()
{
	this->PlayerCamera->ChangeCameraTo("Default");
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////状态机相关End

void AMainLevelCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	this->StopJumping();
	this->MovementStateMachineComponent->ChangeStateTo("JumpLanded");
}

void AMainLevelCharacter::OnPlayerCameraManagerUpdated()
{
	AMainLevelPlayerController* PC = this->GetController<AMainLevelPlayerController>();

	FHitResult HitResult;
	FVector CameraLocationInWorld = this->PlayerCamera->GetComponentLocation();
	// 球形射线初始位置。位于摄像机中点。
	FVector StartLocation = CameraLocationInWorld;
	// 球形射线终止位置。位于摄像机正前方CapsuleTraceDistance距离。
	FVector EndLocation = CameraLocationInWorld + this->PlayerCamera->GetForwardVector() * this->SphereTraceDistance;
	// 球形射线检测可交互Actor。


	bool bGetInteractableActor = false;
	if (UKismetSystemLibrary::SphereTraceSingle(this, StartLocation, EndLocation, this->SphereTraceRadius, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, HitResult, true))
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor && HitActor->GetClass()->ImplementsInterface(UInteractableObjectInterface::StaticClass()))
		{
			// 即使实现了接口，如果不可交互的话需要跳过该Actor。
			if (IInteractableObjectInterface::Execute_IsInteractable(HitResult.Actor.Get()))
			{

				// 判断当前可交互对象是否和上次检测到的对象一致。
				if (HitResult.Actor == this->InteractableActor)
				{
					bGetInteractableActor = true;
				}
				else
				{
					if (AActor* OldActor = this->InteractableActor.Get())
					{
						IInteractableObjectInterface::Execute_LeaveInteractableState(OldActor);
					}

					this->InteractableActor = HitActor;
					IInteractableObjectInterface::Execute_EnterInteractableState(HitActor);
					this->PlayerHUD->OnEnterInteractableState();

					bGetInteractableActor = true;
				}
			}
		}
	}

	// 如果在上面的逻辑中没有找到可交互物，或者射线检测没有检测到任何物体，则需要将之前检测到的可交互物脱离交互状态。
	if (!bGetInteractableActor)
	{
		if (AActor* OldActor = this->InteractableActor.Get())
		{
			IInteractableObjectInterface::Execute_LeaveInteractableState(OldActor);
			this->PlayerHUD->OnLeaveInteractableState();
			this->InteractableActor.Reset();
		}
		// 如果物体已经失效了（如某些原因自我销毁），则需要重置。
		else if (this->InteractableActor.IsStale())
		{
			this->PlayerHUD->OnLeaveInteractableState();
			this->InteractableActor.Reset();
		}

	}

	// 设置准星位置。
	FVector2D CrosshairPosition;
	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PC, EndLocation, CrosshairPosition, true);
	this->PlayerHUD->SetCrosshairPositionInScreen(CrosshairPosition);
}

void AMainLevelCharacter::Interact(const FInputActionInstance& InValue)
{
	if (this->InteractableActor.IsValid())
	{
		IInteractableObjectInterface::Execute_Interact(this->InteractableActor.Get());
	}
}


