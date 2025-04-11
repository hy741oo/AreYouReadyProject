// Fill out your copyright notice in the Description page of Project Settings.


#include "AYRGameViewportClient.h"

#include "Engine/Canvas.h"
void UAYRGameViewportClient::StartFade(const bool InbFadeIn, float InDurationTime)
{
	this->bIsFading = true;
	this->bFadeIn = InbFadeIn;
	this->DurationTime = InDurationTime;
}

void UAYRGameViewportClient::StartFadeWithEvent(FOnFadeEndDelegate InOnFadeEnd, const bool InbFadeIn, const float InDurationTime)
{
	this->bIsFading = true;
	this->bFadeIn = InbFadeIn;
	this->DurationTime = InDurationTime;
	this->OnFadeEnd = InOnFadeEnd;
}

void UAYRGameViewportClient::StopFade()
{
	this->bIsFading = false;
	this->ElapsedTime = .0f;
	this->DurationTime = .0f;
	this->OnFadeEnd.Unbind();
}

void UAYRGameViewportClient::ExecuteEvent()
{
	if (this->OnFadeEnd.IsBound())
	{
		this->OnFadeEnd.Execute();
	}
}

void UAYRGameViewportClient::AbortFade()
{
	this->ExecuteEvent();
	this->StopFade();
}

void UAYRGameViewportClient::PostRender(UCanvas* InCanvas)
{
	Super::PostRender(InCanvas);

	if (InCanvas && this->bIsFading && this->World)
	{
		// 先绘制一次渐变，防止最后一帧渐变突然消失造成的突兀感。
		const uint8 Source = this->bFadeIn ? 255 : 0;
		const uint8 Target = this->bFadeIn ? 0 : 255;
		const uint8 Alpha = FMath::Lerp(Source, Target, this->ElapsedTime / this->DurationTime);
		FColor InCanvasColor = FColor::Black;
		InCanvasColor.A = Alpha;
		InCanvas->SetDrawColor(InCanvasColor);
		InCanvas->DrawTile(InCanvas->DefaultTexture, 0, 0, InCanvas->ClipX, InCanvas->ClipY, 0, 0, InCanvas->DefaultTexture->GetSizeX(), InCanvas->DefaultTexture->GetSizeY());

		// 判断下一帧要不要继续绘制渐变。
		this->ElapsedTime += this->World->GetDeltaSeconds();
		if (this->ElapsedTime >= this->DurationTime)
		{
			this->ExecuteEvent();
			this->StopFade();
		}
	}
}
