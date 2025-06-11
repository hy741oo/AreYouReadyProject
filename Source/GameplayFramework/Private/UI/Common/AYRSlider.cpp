// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/AYRSlider.h"

void UAYRSlider::AddValueByStep()
{
	float NewValue = FMath::Clamp<float>(this->GetValue() + StepSize, this->MinValue, this->MaxValue);

	this->SetValue(NewValue);

	this->OnValueChanged.Broadcast(NewValue);
}

void UAYRSlider::SubtractValueByStep()
{
	float NewValue = FMath::Clamp<float>(this->GetValue() - StepSize, this->MinValue, this->MaxValue);

	this->SetValue(NewValue);

	this->OnValueChanged.Broadcast(NewValue);
}


