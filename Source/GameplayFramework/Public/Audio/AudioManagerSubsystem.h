// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameConfigSubsystem.h"

#include "AudioManagerSubsystem.generated.h"

USTRUCT()
struct FAudioManagerDataTableRow : public FAYRTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	USoundWave* SoundWave = nullptr;
};

/**
 * 声音相关的子系统，用于播放音效、BGM等。
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UAudioManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
};
