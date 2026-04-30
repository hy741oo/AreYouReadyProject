// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/OSSlider.h"

void UOSSlider::AddValueByStep()
{
	float NewValue = FMath::Clamp<float>(this->GetValue() + StepSize, this->MinValue, this->MaxValue);

	this->SetValue(NewValue);

	this->OnValueChanged.Broadcast(NewValue);
}

void UOSSlider::SubtractValueByStep()
{
	float NewValue = FMath::Clamp<float>(this->GetValue() - StepSize, this->MinValue, this->MaxValue);

	this->SetValue(NewValue);

	this->OnValueChanged.Broadcast(NewValue);
}


