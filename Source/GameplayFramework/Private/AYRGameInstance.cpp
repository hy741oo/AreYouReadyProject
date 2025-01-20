// Fill out your copyright notice in the Description page of Project Settings.


#include "AYRGameInstance.h"

#include "AYRInputProcessor.h"
#include "Framework/Application/SlateApplication.h"

void UAYRGameInstance::OnStart()
{
	Super::OnStart();

	// 添加InputProcess。
	FSlateApplication& App = FSlateApplication::Get();
	// 检测是否已经添加过一次，防止重复添加。
	if (App.FindInputPreProcessor(FAYRInputProcessor::Get()) == INDEX_NONE)
	{
		App.RegisterInputPreProcessor(FAYRInputProcessor::Get());
	}
}
