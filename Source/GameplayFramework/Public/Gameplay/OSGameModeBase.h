// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OSGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API AOSGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FName PlayerControllerID;
	
public:
	AOSGameModeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 初始化项目的Controller。
	virtual APlayerController* SpawnPlayerControllerCommon(ENetRole InRemoteRole, FVector const& SpawnLocation, FRotator const& SpawnRotation, TSubclassOf<APlayerController> InPlayerControllerClass) override;
};
