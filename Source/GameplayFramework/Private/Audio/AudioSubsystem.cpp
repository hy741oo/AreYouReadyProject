// Fill out your copyright notice in the Description page of Project Settings.


#include "Audio/AudioSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

void UAudioSubsystem::Initialize(FSubsystemCollectionBase& InCollection)
{
	this->GameConfig = InCollection.InitializeDependency<UGameConfigSubsystem>();
	check(this->GameConfig);
}

void UAudioSubsystem::PlaySound2D(FName InSoundID, float InStartTime, AActor* InOwningActor)
{
	FAudioManagerDataTableRow* TableRow = nullptr;
	if (this->GameConfig->GetDataTableRowFromID<FAudioManagerDataTableRow>(InSoundID, TableRow))
	{
		UGameplayStatics::PlaySound2D(this, TableRow->SoundBase, TableRow->VolumeMultiplier, TableRow->PitchMultiplier, InStartTime, TableRow->OverriddenConcurrencySettings, InOwningActor, TableRow->bIsUISound);
	}
}

UAudioComponent* UAudioSubsystem::SpawnSound2D(FName InSoundID, float InStartTime, bool bInPersistAcrossLevelTransition)
{
	FAudioManagerDataTableRow* TableRow = nullptr;
	UAudioComponent* AudioComponent = nullptr;

	if (this->GameConfig->GetDataTableRowFromID<FAudioManagerDataTableRow>(InSoundID, TableRow))
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

void UAudioSubsystem::PlaySound3D(FName InSoundID, FVector InLocation, FRotator InRotation, float InStartTime, AActor* InOwningActor)
{
	FAudioManagerDataTableRow* TableRow = nullptr;
	if (this->GameConfig->GetDataTableRowFromID<FAudioManagerDataTableRow>(InSoundID, TableRow))
	{
		UGameplayStatics::PlaySoundAtLocation(this, TableRow->SoundBase, InLocation, InRotation, TableRow->VolumeMultiplier, TableRow->PitchMultiplier, InStartTime, TableRow->OverriddenAttenuationSettings, TableRow->OverriddenConcurrencySettings, InOwningActor);
	}
}

UAudioComponent* UAudioSubsystem::SpawnSound3D(FName InSoundID, FVector InLocation, FRotator InRotation, float InStartTime, bool bInPersistAcrossLevelTransition)
{
	FAudioManagerDataTableRow* TableRow = nullptr;
	UAudioComponent* AudioComponent = nullptr;

	if (this->GameConfig->GetDataTableRowFromID<FAudioManagerDataTableRow>(InSoundID, TableRow))
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

void UAudioSubsystem::SetSoundMixClassOverride(FName InSoundMixClassID, float InVolume, float InPitch, float InFadeInTime, bool InbApplyToChildren)
{
	FSoundMixClassOverrideTableRow* TableRow = nullptr;
	if (this->GameConfig->GetDataTableRowFromID<FSoundMixClassOverrideTableRow>(InSoundMixClassID, TableRow))
	{
		UGameplayStatics::SetSoundMixClassOverride(this, TableRow->SoundMix, TableRow->SoundClass, InVolume, InPitch, InFadeInTime, InbApplyToChildren);
	}
}

void UAudioSubsystem::ClearSoundMixClassOverride(FName InSoundMixClassID, float InFadeOutTime)
{
	FSoundMixClassOverrideTableRow* TableRow = nullptr;
	if (this->GameConfig->GetDataTableRowFromID<FSoundMixClassOverrideTableRow>(InSoundMixClassID, TableRow))
	{
		UGameplayStatics::ClearSoundMixClassOverride(this, TableRow->SoundMix, TableRow->SoundClass, InFadeOutTime);
	}
}

void UAudioSubsystem::PushSoundMix(FName InSoundMixID)
{
	FSoundMixTableRow* TableRow = nullptr;
	if (this->GameConfig->GetDataTableRowFromID<FSoundMixTableRow>(InSoundMixID, TableRow))
	{
		UGameplayStatics::PushSoundMixModifier(this, TableRow->SoundMix);
	}
}

void UAudioSubsystem::PopSoundMix(FName InSoundMixID)
{
	FSoundMixTableRow* TableRow = nullptr;
	if (this->GameConfig->GetDataTableRowFromID<FSoundMixTableRow>(InSoundMixID, TableRow))
	{
		UGameplayStatics::PopSoundMixModifier(this, TableRow->SoundMix);
	}
}
