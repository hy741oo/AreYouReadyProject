// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactive/InteractableActor.h"

// Sets default values
AInteractableActor::AInteractableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInteractableActor::BeginPlay()
{
	Super::BeginPlay();
	
	// 设置要显示的输入案件图标Name。
	IInteractableObjectInterface::Execute_SetInputIconName(this, this->InputIconName);
}

// Called every frame
void AInteractableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

