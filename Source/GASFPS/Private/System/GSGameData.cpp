// Copyright Epic Games, Inc. All Rights Reserved.

#include "System/GSGameData.h"
#include "GSAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GSGameData)

UGSGameData::UGSGameData()
{
}

const UGSGameData& UGSGameData::UGSGameData::Get()
{
	return UGSAssetManager::Get().GetGameData();
}
