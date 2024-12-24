// Fill out your copyright notice in the Description page of Project Settings.


#include "ConfigSubsystem.h"

#include "AssetRegistry/IAssetRegistry.h"
#include "AssetRegistry/AssetRegistryModule.h"

void UConfigSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AR = AssetRegistryModule.Get();
	FARFilter Filter;
	Filter.ClassNames.Add(UDataTable::StaticClass()->GetFName());
	Filter.PackagePaths.Add("/Game/AYR/Configs/");
	Filter.bRecursiveClasses = true;
	Filter.bRecursivePaths = true;
	TArray<FAssetData> SearchedAssets;
	AR.GetAssets(Filter, SearchedAssets);

	for (const FAssetData& SearchedAsset : SearchedAssets)
	{
		if (SearchedAsset.IsValid())
		{
			UDataTable* DT = CastChecked<UDataTable>(SearchedAsset.GetAsset());
			UDataTable*& DataTable = this->LoadedDataTables.Add(DT->GetRowStruct()->GetFName());
			DataTable = DT;
			UE_LOG(LogConfigSubsystem, Display, TEXT("Load DataTable: %s"), *(DataTable->GetFName().ToString()));
		}
		else
		{
			UE_LOG(LogConfigSubsystem, Warning, TEXT("Asset is invalid, asset path: %s"), *SearchedAsset.ObjectPath.ToString());
		}
	}
}

