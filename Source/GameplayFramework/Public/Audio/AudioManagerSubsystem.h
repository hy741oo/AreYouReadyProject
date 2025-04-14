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

	// 要播放的音频资产。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	USoundWave* SoundWave = nullptr;

	// 音量系数。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float VolumeMultiplier = 1.f;

	// 音调系数。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float PitchMultiplier = 1.f;

	// 并发设置。用于配置当该音频同时播放多次时的表现。会覆盖Sound Wave或Sound Cue里的并发设置。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	USoundConcurrency* OverriddenConcurrencySettings = nullptr;

	// 音频衰减设置。会覆盖Sound Wave或Sound Cue里的音频衰减设置。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	USoundAttenuation* OverriddenAttenuationSettings = nullptr;

	// 是否为UI音频。如果为UI音频的话当游戏暂停后依然会播放。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bIsUISound = false;

	// 是否播放完后销毁。对“PlaySound2D”和“PlayerSound3D”不起效。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bAutoDestroy = true;

	// 是否在附加对象被销毁后停止播放。对“PlaySound2D”和“PlayerSound3D”不起效。
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bStopWhenAttachedToDestroyed = false;
};

/**
 * 声音相关的子系统，用于播放音效、BGM等。
 */
UCLASS()
class GAMEPLAYFRAMEWORK_API UAudioManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	// 生成并播放2D音频。适用于播放UI音效、BGM等。
	UFUNCTION(BlueprintCallable, Category = "Audio|2D")
	void PlaySound2D(FName SoundID, float StartTime = -1.f, AActor* OwningActor = nullptr);

	// 生成并播放2D音频，同时返回Audio Component供后续使用。
	UFUNCTION(BlueprintCallable, Category = "Audio|2D")
	UAudioComponent* SpawnSound2D(FName SoundID, float StartTime = -1.f, bool bPersistAcrossLevelTransition = false);

	// 生成并播放3D音频，在指定Location和Rotation上生成空间音效。
	UFUNCTION(BlueprintCallable, Category = "Audio|3D")
	void PlaySound3D(FName SoundID, FVector Location, FRotator Rotation, float StartTime = -1.f, AActor* OwningActor = nullptr);

	// 生成并播放3D音频，同时返回Audio Component供后续使用。
	UFUNCTION(BlueprintCallable, Category = "Audio|3D")
	UAudioComponent* SpawnSound3D(FName SoundID, FVector Location, FRotator Rotation, float StartTime = -1.f, bool bPersistAcrossLevelTransition = false);
};
