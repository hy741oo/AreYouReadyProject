// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingScreen/LoadingScreenPlayerController.h"

#include "UISubsystem.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/LoadingScreenWidget.h"

ALoadingScreenPlayerController::ALoadingScreenPlayerController(const FObjectInitializer& InObjectInitializer)
	:Super(InObjectInitializer)
{

}

void ALoadingScreenPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 添加加载界面UI。
	UUISubsystem* UISubsystem = this->GetWorld()->GetSubsystem<UUISubsystem>();
	ensureAlwaysMsgf(!this->LoadingScreenWidget, TEXT("LoadingScrrenWidget is not nullptr!"));
	this->LoadingScreenWidget = CastChecked<ULoadingScreenWidget>(UISubsystem->PushUI(TEXT("LoadingScreen")));

	// 从GameMode获取需要加载关卡的ID。
	AGameModeBase* GM = this->GetWorld()->GetAuthGameMode();
	FName NewLevelID = FName(*UGameplayStatics::ParseOption(GM->OptionsString, TEXT("NewLevelID")));

	// 获取需要加载的关卡。
	UConfigSubsystem* ConfigSubsystem = UGameInstance::GetSubsystem<UConfigSubsystem>(this->GetGameInstance());
	FLevelData* LevelData = nullptr;
	if (ConfigSubsystem->GetDataTableRowFromID<FLevelData>(NewLevelID, LevelData))
	{
		this->NewLevel = LevelData->Level;

		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		FStreamableDelegate StreamingDelegate = FStreamableDelegate::CreateUObject(this, &ALoadingScreenPlayerController::OnNewLevelAsyncLoadingFinished);
		this->StreamableHandle = StreamableManager.RequestAsyncLoad(LevelData->Level.ToSoftObjectPath(), StreamingDelegate);
	}
}

void ALoadingScreenPlayerController::OnNewLevelAsyncLoadingFinished()
{
	UGameplayStatics::OpenLevel(this, *this->NewLevel.GetAssetName());
}

