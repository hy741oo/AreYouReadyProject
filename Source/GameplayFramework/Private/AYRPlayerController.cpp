// Fill out your copyright notice in the Description page of Project Settings.


#include "AYRPlayerController.h"

AAYRPlayerController::AAYRPlayerController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

void AAYRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 从数据表里获取控制器信息。
	UConfigSubsystem * ConfigSubsystem = this->GetGameInstance()->GetSubsystem<UConfigSubsystem>();
	check(ConfigSubsystem);
	FPlayerControllerInfoTableRow* PlayerControllerInfo = nullptr;
	if (ensureAlways(ConfigSubsystem->GetDataTableRowFromID<FPlayerControllerInfoTableRow>(this->ID, PlayerControllerInfo)))
	{
		this->PlayerControllerInfoTableRow = PlayerControllerInfo;
	}
}

