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
			if (SearchedAsset.IsValid())
			{
				UDataTable* DT = LoadObject<UDataTable>(this, *SearchedAsset.ObjectPath.ToString());
				UDataTable*& DataTable = this->LoadedDataTables.Add(DT->GetRowStruct()->GetFName());
				DataTable = DT;
			}
		}
	}
}

