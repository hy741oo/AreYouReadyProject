// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AYRCharacter.h"

// Sets default values
AAYRCharacter::AAYRCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAYRCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAYRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAYRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

