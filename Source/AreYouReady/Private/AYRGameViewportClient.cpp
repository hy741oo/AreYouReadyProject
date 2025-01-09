// Fill out your copyright notice in the Description page of Project Settings.


#include "AYRGameViewportClient.h"

#include "Engine/Canvas.h"

void UAYRGameViewportClient::StartFade(const float InDurationTime, const bool InbFadeIn)
{
	this->bIsFading = true;
	this->bFadeIn = InbFadeIn;
	this->DurationTime = InDurationTime;
}

void UAYRGameViewportClient::ResetFade()
{
	this->bIsFading = false;
	this->ElapsedTime = .0f;
	this->DurationTime = .0f;
}

void UAYRGameViewportClient::PostRender(UCanvas* InCanvas)
{
	Super::PostRender(InCanvas);

	if (InCanvas && this->bIsFading && this->World)
	{
		this->ElapsedTime += this->World->GetDeltaSeconds();
		if (this->ElapsedTime >= this->DurationTime)
		{
			this->ResetFade();
			FColor InCanvasColor = InCanvas->DrawColor;
			InCanvasColor.A = 255;
			InCanvas->SetDrawColor(InCanvasColor);
		}
		else
		{
			uint8 Source = this->bFadeIn ? 0 : 255;
			uint8 Target = this->bFadeIn ? 255 : 0;
			uint8 Alpha = FMath::Lerp(Source, Target, this->ElapsedTime / this->DurationTime);
			FColor InCanvasColor = InCanvas->DrawColor;
			InCanvasColor.A = Alpha;
			InCanvas->SetDrawColor(InCanvasColor);
		}
	}
}
