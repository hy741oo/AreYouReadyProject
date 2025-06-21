// Fill out your copyright notice in the Description page of Project Settings.


#include "AYRPlayerController.h"

#include "EnhancedInputComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameConfigSubsystem.h"
#include "UI/Common/AYRButton.h"
#include "UISubsystem.h"

DEFINE_LOG_CATEGORY(LogAYRPlayerController);

AAYRPlayerController::AAYRPlayerController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

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

