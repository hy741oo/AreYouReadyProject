// Fill out your copyright notice in the Description page of Project Settings.


#include "MainLevel/MainLevelPlayerController.h"

#include "Subsystem/GameWorldSubsystem.h"

AMainLevelPlayerController::AMainLevelPlayerController(const FObjectInitializer& InObjectInitializer)
	:Super(InObjectInitializer)
{

}

void AMainLevelPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 禁止玩家输入，等待黑屏渐变结束。
	this->DisableInput(this);

	// 黑屏渐变。结束后恢复玩家输入。
	UGameWorldSubsystem* GameWorldSubsystem = UGameInstance::GetSubsystem<UGameWorldSubsystem>(this->GetGameInstance());
	FOnFadeEndDelegate Delegate;
	Delegate.BindWeakLambda(this,
		[this]() {
			this->EnableInput(this);
		}
		);
	GameWorldSubsystem->StartFadeWithEvent(Delegate, "EnterMainLevelFadeIn");
}

