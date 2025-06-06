// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingScreen/LoadingScreenPlayerController.h"

#include "UISubsystem.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/LoadingScreenWidget.h"
#include "WorldManager.h"

DEFINE_LOG_CATEGORY(LogLoadingScreenPlayerController);

ALoadingScreenPlayerController::ALoadingScreenPlayerController(const FObjectInitializer& InObjectInitializer)
	:Super(InObjectInitializer)
{

}

void ALoadingScreenPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 添加加载界面UI。
	UUISubsystem* UISubsystem = ULocalPlayer::GetSubsystem<UUISubsystem>(this->GetLocalPlayer());
	ensureAlwaysMsgf(!this->LoadingScreenWidget, TEXT("LoadingScrrenWidget is not nullptr!"));
	this->LoadingScreenWidget = CastChecked<ULoadingScreenWidget>(UISubsystem->PushUI(TEXT("LoadingScreen")));

	// 从GameMode获取需要加载关卡的ID。
	AGameModeBase* GM = this->GetWorld()->GetAuthGameMode();
	FName NewLevelID = FName(*UGameplayStatics::ParseOption(GM->OptionsString, TEXT("NewLevelID")));

	// 获取需要加载的关卡。
	UGameConfigSubsystem* ConfigSubsystem = UGameInstance::GetSubsystem<UGameConfigSubsystem>(this->GetGameInstance());
	FLevelData* LevelData = nullptr;
	if (ConfigSubsystem->GetDataTableRowFromID(NewLevelID, LevelData))
	{
		this->NewLevel = LevelData->Level;

		// 异步加载关卡资源。
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		FStreamableDelegate StreamingDelegate = FStreamableDelegate::CreateUObject(this, &ALoadingScreenPlayerController::OnNewLevelAsyncLoadingFinished);
		UE_LOG(LogLoadingScreenPlayerController, Log, TEXT("Start Async loading new level and delay timer, new level name: %s"), *this->NewLevel.GetLongPackageName());
		this->StreamableHandle = StreamableManager.RequestAsyncLoad(LevelData->Level.ToSoftObjectPath(), StreamingDelegate);
		FTimerManager& TimerManager = this->GetWorld()->GetTimerManager();
		TimerManager.ClearTimer(this->DelayLoadingScreenTimerHandle);
		TimerManager.SetTimer(this->DelayLoadingScreenTimerHandle,
			[this]() {
				UE_LOG(LogLoadingScreenPlayerController, Log, TEXT("Delay loading screen timer end"));
				this->bIsTimerEnd = true;
				// 如果异步加载关卡结束，则直接打开加载完成的关卡，否则等待StreamableManager加载完成关卡。
				if (this->bIsLoadedNewLevel)
				{
					this->OpenLoadedNewLevel();
				}
			},
			this->DelayLoadingScreenTime,
			false
		);
	}
}

void ALoadingScreenPlayerController::OnNewLevelAsyncLoadingFinished()
{
	UE_LOG(LogLoadingScreenPlayerController, Log, TEXT("Streaming new level finished"));
	this->bIsLoadedNewLevel = true;
	// 如果延迟计时器已经结束，则直接打开加载完成的关卡，否则等待延迟计时器结束。
	if (this->bIsTimerEnd)
	{
		this->OpenLoadedNewLevel();
	}
}

void ALoadingScreenPlayerController::OpenLoadedNewLevel()
{
	UWorldManager* WorldManager = UGameInstance::GetSubsystem<UWorldManager>(this->GetGameInstance());
	FOnFadeEndDelegate OnFadeEnd;
	OnFadeEnd.BindLambda(
		[this]() {
			UGameplayStatics::OpenLevel(this, *this->NewLevel.GetLongPackageName());
		}
	);
	WorldManager->StartFadeWithEvent(OnFadeEnd);
}

