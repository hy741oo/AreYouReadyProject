// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/OSGameViewportClient.h"

#include "Engine/Canvas.h"
void UOSGameViewportClient::StartFade(const FRichCurve InCurve)
{
	this->bIsFading = true;
	this->OnFadeEnd.Unbind();
	this->Curve = InCurve;
}

void UOSGameViewportClient::StartFadeWithEvent(FOnFadeEndDelegate InOnFadeEnd, const FRichCurve InCurve)
{
	this->bIsFading = true;
	this->OnFadeEnd = InOnFadeEnd;
	this->Curve = InCurve;
}

void UOSGameViewportClient::StopFade()
{
	this->bIsFading = false;
	this->ElapsedTime = .0f;
	this->Curve = FRichCurve();
	this->OnFadeEnd.Unbind();
}

void UOSGameViewportClient::ExecuteEvent()
{
	if (this->OnFadeEnd.IsBound())
	{
		this->OnFadeEnd.Execute();
	}
}

void UOSGameViewportClient::AbortFade()
{
	this->ExecuteEvent();
	this->StopFade();
}

void UOSGameViewportClient::PostRender(UCanvas* InCanvas)
{
	Super::PostRender(InCanvas);

	if (InCanvas && this->bIsFading && this->World)
	{
		// 先绘制一次渐变，防止最后一帧渐变突然消失造成的突兀感。
		FColor InCanvasColor = FColor::Black;
		InCanvasColor.A = 255 - FMath::GetMappedRangeValueClamped(FVector2D(.0f, 1.f), FVector2D(.0f, 255.f), this->Curve.Eval(this->ElapsedTime));
		InCanvas->SetDrawColor(InCanvasColor);
		InCanvas->DrawTile(InCanvas->DefaultTexture, 0, 0, InCanvas->ClipX, InCanvas->ClipY, 0, 0, InCanvas->DefaultTexture->GetSizeX(), InCanvas->DefaultTexture->GetSizeY());

		// 判断下一帧要不要继续绘制渐变。
		this->ElapsedTime += this->World->GetDeltaSeconds();
		if (this->ElapsedTime >= this->Curve.GetLastKey().Time)
		{
			this->ExecuteEvent();
			this->StopFade();
		}
	}
}
