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

			// 交互。
			PlayerInputSubsystem->BindPlayerInputAction("MainLevel_Interact", this, &AMainLevelCharacter::Interact);

			// 打开暂停菜单。
			PlayerInputSubsystem->BindPlayerInputAction("MainLevel_OpenPauseMenu", this, &AMainLevelCharacter::OpenPauseMenu);
		}

		this->OnPlayerCameraManagerUpdatedHandle = OwningController->OnPlayerCameraManagerUpdateDelegate.AddUObject(this, &AMainLevelCharacter::OnPlayerCameraManagerUpdated);
	}

	// 初始化状态机。
	this->InitializeGeneralStateMachine();

	// 添加HUD。
	UUISubsystem* UISubsystem = ULocalPlayer::GetSubsystem<UUISubsystem>(this->GetController<AMainLevelPlayerController>()->GetLocalPlayer());
	this->PlayerHUD = CastChecked<UPlayerHUD>(UISubsystem->PushUI("PlayerHUD"));

	// 添加基础的IMC。
	this->AddCharacterInputMappingContext();
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

	// 移除基础的IMC。
	this->RemoveCharacterInputMappingContext();

	// 清除关闭暂停菜单GMS句柄。
	if (UGameplayMessageSubsystem* GMS = UGameInstance::GetSubsystem<UGameplayMessageSubsystem>(this->GetGameInstance()))
	{
		if (UGameplayMessageSubsystem::IsHandleValid(this->OnClosePauseMenuHandle))
		{
			GMS->Unregister(this->OnClosePauseMenuHandle);
		}
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
	/*
	创建状态机节点。
	*/

	FSimpleDelegate StateDelegate;

	// 待机状态。
	this->MovementStateMachineComponent->CreateStateMachineNode("Idle");
	StateDelegate.BindUObject(this, &AMainLevelCharacter::OnEnterIdleState);
	this->MovementStateMachineComponent->SetStateMachineNodeOnEnterStateDelegate("Idle", StateDelegate);
	StateDelegate.Unbind();
	FOnStateTickedDelegate IdleStateTickDelegate;
	IdleStateTickDelegate.BindLambda(
		[this](const float& InDeltaTime, const float& InTickingElapsedTime) {
			this->StartCameraShake("IdleCameraShake");
		}
	);
	this->MovementStateMachineComponent->SetStateMachineNodeOnTickStateDelegate("Idle", IdleStateTickDelegate);
	StateDelegate.BindLambda(
		[this]() {
			this->StopCameraShake("IdleCameraShake");
		}
	);
	this->MovementStateMachineComponent->SetStateMachineNodeOnLeaveStateDelegate("Idle", StateDelegate);
	StateDelegate.Unbind();

	// 走路状态。
	this->MovementStateMachineComponent->CreateStateMachineNode("Walk");
	StateDelegate.BindLambda(
		[this]()
		{
			if (this->MovementDataStateMachineComponent->IsSameState("HighSpeed"))
			{
				this->PlayerCamera->ChangeCameraTo("Running");
			}
		}
	);
	this->MovementStateMachineComponent->SetStateMachineNodeOnEnterStateDelegate("Walk", StateDelegate);
	StateDelegate.Unbind();
	StateDelegate.BindLambda(
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
	this->MovementStateMachineComponent->SetStateMachineNodeOnUpdateStateDelegate("Walk", StateDelegate);
	StateDelegate.Unbind();
	StateDelegate.BindLambda(
		[this]() {
			this->StopCameraShake("WalkingCameraShake");
			this->StopCameraShake("RunningCameraShake");
		}
	);
	this->MovementStateMachineComponent->SetStateMachineNodeOnLeaveStateDelegate("Walk", StateDelegate);
	StateDelegate.Unbind();

	// 跳跃状态。
	this->MovementStateMachineComponent->CreateStateMachineNode("Jump");
	StateDelegate.BindUObject(this, &AMainLevelCharacter::OnEnterJumpState);
	this->MovementStateMachineComponent->SetStateMachineNodeOnEnterStateDelegate("Jump", StateDelegate);
	StateDelegate.Unbind();

	// 跳跃落地状态。
	this->MovementStateMachineComponent->CreateStateMachineNode("JumpLanded");
	StateDelegate.BindLambda(
		[this]() {
			this->MovementStateMachineComponent->ChangeStateTo("Idle");
			this->MovementDataStateMachineComponent->ChangeStateTo("NormalSpeed");
		}
	);
	this->MovementStateMachineComponent->SetStateMachineNodeOnEnterStateDelegate("JumpLanded", StateDelegate);
	StateDelegate.Unbind();

	// Idle状态可以切换到Idle、Walk和Jump状态且不需要任何判断。
	this->MovementStateMachineComponent->SetStateMachineCondition("Idle", "Idle");
	this->MovementStateMachineComponent->SetStateMachineCondition("Idle", "Walk");
	this->MovementStateMachineComponent->SetStateMachineCondition("Idle", "Jump");
	// Walk状态可以切换到Walk、Idle和Jump状态。
	this->MovementStateMachineComponent->SetStateMachineCondition("Walk", "Walk");
	{
		FGeneralStateMachineConditionCheckDelegate Condition;
		Condition.BindLambda(
			[this]() {
				return !this->MovementStateMachineComponent->IsSameState("Jump");
			}
		);
		this->MovementStateMachineComponent->SetStateMachineCondition("Walk", "Idle", Condition);
	}
	this->MovementStateMachineComponent->SetStateMachineCondition("Walk", "Jump");

	// Jump状态唯一可以切换的状态是JumpLanded，即落地的一瞬间。
	this->MovementStateMachineComponent->SetStateMachineCondition("Jump", "JumpLanded");

	// JumpLanded可以转换到Idle和Walk状态。
	this->MovementStateMachineComponent->SetStateMachineCondition("JumpLanded", "Idle");
	this->MovementStateMachineComponent->SetStateMachineCondition("JumpLanded", "Walk");

	// 初始化状态机，选择一个状态机节点作为最初始的状态。
	this->MovementStateMachineComponent->InitGeneralStateMachine("Idle");

	// 创建移动数据状态节点并绑定。
	this->MovementDataStateMachineComponent->CreateStateMachineNode("NormalSpeed");
	StateDelegate.BindUObject(this, &AMainLevelCharacter::OnEnterNormalSpeedState);
	this->MovementDataStateMachineComponent->SetStateMachineNodeOnEnterStateDelegate("NormalSpeed", StateDelegate);
	StateDelegate.Unbind();
	this->MovementDataStateMachineComponent->CreateStateMachineNode("HighSpeed");
	StateDelegate.BindUObject(this, &AMainLevelCharacter::OnEnterHighSpeedState);
	this->MovementDataStateMachineComponent->SetStateMachineNodeOnEnterStateDelegate("HighSpeed", StateDelegate);
	StateDelegate.Unbind();

	FGeneralStateMachineConditionCheckDelegate NormalSpeedToHighSpeed;
	NormalSpeedToHighSpeed.BindWeakLambda(this, 
		[this]() {
			return this->MovementStateMachineComponent->IsSameState("Walk");
		}
		);
	this->MovementDataStateMachineComponent->SetStateMachineCondition("NormalSpeed", "HighSpeed", NormalSpeedToHighSpeed);

	FGeneralStateMachineConditionCheckDelegate HighSpeedToNormalSpeed;
	HighSpeedToNormalSpeed.BindLambda(
		[this]() {
			return !this->MovementStateMachineComponent->IsSameState("Jump");
		}
	);
	this->MovementDataStateMachineComponent->SetStateMachineCondition("HighSpeed", "NormalSpeed", HighSpeedToNormalSpeed);

	FGeneralStateMachineConditionCheckDelegate HighSpeedToHighSpeed;
	HighSpeedToHighSpeed.BindLambda(
		[this]() {
			return this->MovementStateMachineComponent->IsSameState("Walk");
		}
	);
	this->MovementDataStateMachineComponent->SetStateMachineCondition("HighSpeed", "HighSpeed", HighSpeedToHighSpeed);

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
	if (UKismetSystemLibrary::SphereTraceSingle(this, StartLocation, EndLocation, this->SphereTraceRadius, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::None, HitResult, true))
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

					// 设置交互需要显示的字段。
					this->PlayerHUD->SetInputIconName(IInteractableObjectInterface::Execute_GetInputIconName(HitResult.Actor.Get()));

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

void AMainLevelCharacter::SetGettingBlueIDCard()
{
	this->BlueIDCard = true;
}

bool AMainLevelCharacter::HaveGotBlueIDCard() const
{
	return this->BlueIDCard;
}

void AMainLevelCharacter::SetGettingGreenIDCard()
{
	this->GreenIDCard = true;
}

bool AMainLevelCharacter::HaveGotGreenIDCard() const
{
	return this->GreenIDCard;
}

void AMainLevelCharacter::SetGettingRedIDCard()
{
	this->RedIDCard = true;
}

bool AMainLevelCharacter::HaveGotRedIDCard() const
{
	return this->RedIDCard;
}

void AMainLevelCharacter::OpenPauseMenu(const FInputActionInstance& InValue)
{
	// 暂停游戏。
	UGameplayStatics::SetGamePaused(this, true);

	// 移除玩家角色的IMC。
	this->RemoveCharacterInputMappingContext();

	// 绑定关闭暂停菜单事件。
	if (UGameplayMessageSubsystem* GMS = UGameInstance::GetSubsystem<UGameplayMessageSubsystem>(this->GetGameInstance()))
	{
		FGameplayTag Tag = FGameplayTag::RequestGameplayTag("GMSMessage.Gameplay.OnClosePauseMenu");
		FOnMessageReceived OnMessageReceived;
		OnMessageReceived.BindWeakLambda(this,
			[this, GMS](const UGMSMessageBase* InMessage) {
				this->AddCharacterInputMappingContext();
				UGameplayStatics::SetGamePaused(this, false);
				if (GMS)
				{
					GMS->Unregister(this->OnClosePauseMenuHandle);
				}
			}
			);
		this->OnClosePauseMenuHandle = GMS->Register(Tag, OnMessageReceived);
	}

	// 打开菜单。
	if (UUISubsystem* UISubsystem = ULocalPlayer::GetSubsystem<UUISubsystem>(this->GetController<AMainLevelPlayerController>()->GetLocalPlayer()))
	{
		UISubsystem->PushUI(TEXT("MainLevel_PauseMenu"));
	}
}

void AMainLevelCharacter::AddCharacterInputMappingContext()
{
	if (AMainLevelPlayerController* OwningController = this->GetController<AMainLevelPlayerController>())
	{
		// 绑定基础运动。
		if (UPlayerInputSubsystem* PlayerInputSubsystem = ULocalPlayer::GetSubsystem<UPlayerInputSubsystem>(OwningController->GetLocalPlayer()))
		{
			// 基础动作（如交互）的IMC。
			PlayerInputSubsystem->AddPlayerInputMappingContext("MainLevel_BaseAction");

			// 基础运动IMC。
			PlayerInputSubsystem->AddPlayerInputMappingContext("MainLevel_Movement");
		}
	}
}

void AMainLevelCharacter::RemoveCharacterInputMappingContext()
{
	// 移除已经绑定了的全部IMC。
	if (AMainLevelPlayerController* OwningController = this->GetController<AMainLevelPlayerController>())
	{
		if (UPlayerInputSubsystem* PlayerInputSubsystem = ULocalPlayer::GetSubsystem<UPlayerInputSubsystem>(OwningController->GetLocalPlayer()))
		{
			// 基础动作（如交互）的IMC。
			PlayerInputSubsystem->RemovePlayerInputMappingContext("MainLevel_BaseAction");

			// 基础运动IMC。
			PlayerInputSubsystem->RemovePlayerInputMappingContext("MainLevel_Movement");
		}
	}
}

void AMainLevelCharacter::HideIDCardIcons()
{
	if (this->PlayerHUD)
	{
		this->PlayerHUD->HideIDCardIcons();
	}
}

