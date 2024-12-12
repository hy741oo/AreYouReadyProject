// Fill out your copyright notice in the Description page of Project Settings.


#include "ConfigSubsystem.h"

#include "AssetRegistry/IAssetRegistry.h"


void UConfigSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	if (IAssetRegistry* AR = IAssetRegistry::Get())
	{
		FARFilter Filter;
		Filter.ClassNames.Add(UDataTable::StaticClass()->GetFName());
		Filter.PackagePaths.Add("/Game/AYR/Configs/");
		Filter.bRecursiveClasses = true;
		Filter.bRecursivePaths = true;
		TArray<FAssetData> SearchedAssets;
		AR->GetAssets(Filter, SearchedAssets);

		for (const FAssetData& SearchedAsset : SearchedAssets)
		{
			UDataTable* DT = CastChecked<UDataTable>(SearchedAsset.GetAsset());
			DT->AddToRoot();
			TMap<FName, uint8*>& DataTableRows = this->LoadedDataTables.Add(DT->GetRowStruct()->GetFName());
			DataTableRows = DT->GetRowMap();
		}
	}
}

