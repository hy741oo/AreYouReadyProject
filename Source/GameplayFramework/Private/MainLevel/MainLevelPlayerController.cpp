// Fill out your copyright notice in the Description page of Project Settings.


#include "MainLevel/MainLevelPlayerController.h"

#include "WorldManager.h"

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
	UWorldManager* WorldManager = UGameInstance::GetSubsystem<UWorldManager>(this->GetGameInstance());
	FOnFadeEndDelegate Delegate;
	Delegate.BindWeakLambda(this,
		[this]() {
			this->EnableInput(this);
		}
		);
	WorldManager->StartFadeWithEvent(Delegate, true, .6f, true);
}

