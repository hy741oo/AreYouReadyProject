// Fill out your copyright notice in the Description page of Project Settings.


#include "AYRPlayerController.h"

#include "EnhancedInputComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameConfigSubsystem.h"
#include "UI/Common/AYRButton.h"
#include "UISubsystem.h"
#include "Camera/AYRPlayerCameraManager.h"

DEFINE_LOG_CATEGORY(LogAYRPlayerController);

AAYRPlayerController::AAYRPlayerController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	// 设置PlayerCameraManager为我们自己的类。
	this->PlayerCameraManagerClass = AAYRPlayerCameraManager::StaticClass();
}

void AAYRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (this->ID.IsNone())
	{
		// 如果ID为None，则不需要执行后续操作。
		return;
	}

	// 从数据表里获取控制器信息。
	UGameConfigSubsystem* GameConfigSubsystem = UGameInstance::GetSubsystem<UGameConfigSubsystem>(this->GetGameInstance());
	FPlayerControllerInfoTableRow* PlayerControllerInfo = nullptr;
	if (!GameConfigSubsystem->GetDataTableRowFromID(this->ID, PlayerControllerInfo))
	{
		UE_LOG(LogAYRPlayerController, Warning, TEXT("Can't find ID: \"%s\""), *this->ID.ToString());
		return;
	}

	this->PlayerControllerInfoTableRow = PlayerControllerInfo;

	// 设置相机管理器参数。
	if (this->PlayerCameraManager)
	{
		this->PlayerCameraManager->ViewPitchMax = this->PlayerControllerInfoTableRow->CameraManagerLimitPitchMax;
		this->PlayerCameraManager->ViewPitchMin = this->PlayerControllerInfoTableRow->CameraManagerLimitPitchMin;
		this->PlayerCameraManager->ViewYawMax = this->PlayerControllerInfoTableRow->CameraManagerLimitYawMax;
		this->PlayerCameraManager->ViewYawMin = this->PlayerControllerInfoTableRow->CameraManagerLimitYawMin;
	}

	// 设置输入系数，用于放大或缩小玩家的输入。
	this->InputPitchScale = this->PlayerControllerInfoTableRow->InputPitchScale;
	this->InputYawScale = this->PlayerControllerInfoTableRow->InputYawScale;
	this->InputRollScale = this->PlayerControllerInfoTableRow->InputRollScale;
}

void AAYRPlayerController::CleanupGameViewport()
{
	// 清空该PlayerController创建的全部UI。
	if (UUISubsystem* UISubsystem = ULocalPlayer::GetSubsystem<UUISubsystem>(this->GetLocalPlayer()))
	{
		UISubsystem->ClearUIStack();
	}

	Super::CleanupGameViewport();
}

AAYRPlayerCameraManager* AAYRPlayerController::GetPlayerCameraManager() const
{
	return Cast<AAYRPlayerCameraManager>(this->PlayerCameraManager);
}

void AAYRPlayerController::UpdateCameraManager(float InDeltaTime)
{
	Super::UpdateCameraManager(InDeltaTime);

	this->OnPlayerCameraManagerUpdateDelegate.Broadcast();
}
