// Fill out your copyright notice in the Description page of Project Settings.


#include "Audio/AudioManagerSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

void UAudioManagerSubsystem::PlaySound2D(FName InSoundID, float InStartTime, AActor* InOwningActor)
{
	UGameConfigSubsystem* Config = UGameInstance::GetSubsystem<UGameConfigSubsystem>(this->GetGameInstance());
	FAudioManagerDataTableRow* TableRow = nullptr;
	if (Config->GetDataTableRowFromID<FAudioManagerDataTableRow>(InSoundID, TableRow))
	{
		UGameplayStatics::PlaySound2D(this, TableRow->SoundBase, TableRow->VolumeMultiplier, TableRow->PitchMultiplier, InStartTime, TableRow->OverriddenConcurrencySettings, InOwningActor, TableRow->bIsUISound);
	}
}

UAudioComponent* UAudioManagerSubsystem::SpawnSound2D(FName InSoundID, float InStartTime, bool bInPersistAcrossLevelTransition)
{
	UGameConfigSubsystem* Config = UGameInstance::GetSubsystem<UGameConfigSubsystem>(this->GetGameInstance());
	FAudioManagerDataTableRow* TableRow = nullptr;
	UAudioComponent* AudioComponent = nullptr;

	if (Config->GetDataTableRowFromID<FAudioManagerDataTableRow>(InSoundID, TableRow))
	{
		AudioComponent = UGameplayStatics::SpawnSound2D(this, TableRow->SoundBase, TableRow->VolumeMultiplier, TableRow->PitchMultiplier, InStartTime, TableRow->OverriddenConcurrencySettings, bInPersistAcrossLevelTransition, TableRow->bAutoDestroy);
	}

	if (AudioComponent)
	{
		AudioComponent->AttenuationSettings = TableRow->OverriddenAttenuationSettings;
		AudioComponent->bStopWhenOwnerDestroyed = TableRow->bStopWhenAttachedToDestroyed;
		AudioComponent->bIsUISound = TableRow->bIsUISound;
	}

	return AudioComponent;
}

void UAudioManagerSubsystem::PlaySound3D(FName InSoundID, FVector InLocation, FRotator InRotation, float InStartTime, AActor* InOwningActor)
{
	UGameConfigSubsystem* Config = UGameInstance::GetSubsystem<UGameConfigSubsystem>(this->GetGameInstance());
	FAudioManagerDataTableRow* TableRow = nullptr;
	if (Config->GetDataTableRowFromID<FAudioManagerDataTableRow>(InSoundID, TableRow))
	{
		UGameplayStatics::PlaySoundAtLocation(this, TableRow->SoundBase, InLocation, InRotation, TableRow->VolumeMultiplier, TableRow->PitchMultiplier, InStartTime, TableRow->OverriddenAttenuationSettings, TableRow->OverriddenConcurrencySettings, InOwningActor);
	}
}

UAudioComponent* UAudioManagerSubsystem::SpawnSound3D(FName InSoundID, FVector InLocation, FRotator InRotation, float InStartTime, bool bInPersistAcrossLevelTransition)
{
	UGameConfigSubsystem* Config = UGameInstance::GetSubsystem<UGameConfigSubsystem>(this->GetGameInstance());
	FAudioManagerDataTableRow* TableRow = nullptr;
	UAudioComponent* AudioComponent = nullptr;

	if (Config->GetDataTableRowFromID<FAudioManagerDataTableRow>(InSoundID, TableRow))
	{
		AudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, TableRow->SoundBase, InLocation, InRotation, TableRow->VolumeMultiplier, TableRow->PitchMultiplier, InStartTime, TableRow->OverriddenAttenuationSettings, TableRow->OverriddenConcurrencySettings, TableRow->bAutoDestroy);
	}

	if (AudioComponent)
	{
		AudioComponent->AttenuationSettings = TableRow->OverriddenAttenuationSettings;
		AudioComponent->bStopWhenOwnerDestroyed = TableRow->bStopWhenAttachedToDestroyed;
		AudioComponent->bIsUISound = TableRow->bIsUISound;
	}

	return AudioComponent;
}

