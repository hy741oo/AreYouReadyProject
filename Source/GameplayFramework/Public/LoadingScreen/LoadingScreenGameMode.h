// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AYRGameModeBase.h"
#include "LoadingScreenGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API ALoadingScreenGameMode : public AAYRGameModeBase
{
	GENERATED_BODY()

private:
	class ALoadingScreenPlayerController* LoadingScreenPlayerController = nullptr;

public:
	ALoadingScreenGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	// 将生成的玩家控制器保存成LoadingScreen类型，供后续调用。
	virtual APlayerController* SpawnPlayerControllerCommon(ENetRole RemoteRole, FVector const& SpawnLocation, FRotator const& SpawnRotation, TSubclassOf<APlayerController> PlayerControllerClass) override;
};
