// Copyright Plasma Labs, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BB2D_TakeCaptures.generated.h"

/**
 * 
 */
UCLASS()
class BOUNDINGBOX2D_API UBB2D_TakeCaptures : public UObject
{
	GENERATED_BODY()
		public:
			UBB2D_TakeCaptures();

			void TakeCapture(FString Path, int32 i);
			void SetRenderTextureSize(int32 SizeX = 1920, int32 SizeY = 1080);
			int32 GetLastIndexInDirectory(const FString directory, FString Extension);
};
