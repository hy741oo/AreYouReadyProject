// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/Interactive/InteractableObjectInterface.h"

// Add default functionality here for any IInteractableObjectInterface functions that are not pure virtual.

bool IInteractableObjectInterface::IsInteractable_Implementation()
{
	return this->bIsInteractable;
}

void IInteractableObjectInterface::SetIsInteractable_Implementation(bool bInIsInteractable)
{
	this->bIsInteractable = bInIsInteractable;
}
