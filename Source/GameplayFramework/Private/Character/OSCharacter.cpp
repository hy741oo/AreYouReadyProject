// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/OSCharacter.h"

// Sets default values
AOSCharacter::AOSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AOSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AOSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

