// Copyright Plasma Labs, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/TextureRenderTarget2D.h"
#include "SaveType.h"
#include "BoundingBoxesDetector.generated.h"


UCLASS()
class BOUNDINGBOX2D_API UBoundingBoxesDetector : public UObject
{
	GENERATED_BODY()
		public:
			UBoundingBoxesDetector();

			FString Get2DBoundingBoxes(int32 Padding, TArray<AActor*> Actors, int32 minPixels, ESaveType SaveType = ESaveType::CSV);

			//FVector4 Get2DBoundingBoxes(int32 padding, bool bvector);

			FString Get2DBoundingBox(UTextureRenderTarget2D* RenderTexture, int32 label, int32 ResMultiplier, int32 padding, int32 minPixels, ESaveType SaveType);

			TArray<AActor*> FindActors(FName Tag);

			void SetStencil(AActor* Actor, int32 StencilNumber);

			void SetStencils();

			void SaveCSV(FString csvString, FString SaveDirectory, FString FileName);





	
};