// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingScreen/LoadingScreenPlayerController.h"

#include "UISubsystem.h"

ALoadingScreenPlayerController::ALoadingScreenPlayerController(const FObjectInitializer& InObjectInitializer)
	:Super(InObjectInitializer)
{

}

void ALoadingScreenPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 添加加载界面UI。
	if (UUISubsystem* UISubsystem = this->GetWorld()->GetSubsystem<UUISubsystem>())
	{
		UISubsystem->PushUI(TEXT("LoadingScreen"));
	}
}

