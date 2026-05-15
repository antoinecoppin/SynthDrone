// Copyright Plasma Labs, 2023. All Rights Reserved.


#include "BoundingBoxesDetector.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/TextureRenderTarget.h"
#include <Kismet/GameplayStatics.h>
#include "Kismet/KismetRenderingLibrary.h"
#include "UnrealClient.h"
#include "Engine/Engine.h"
#include "Misc/FileHelper.h"


// Sets default values
UBoundingBoxesDetector::UBoundingBoxesDetector()
{
}

FString UBoundingBoxesDetector::Get2DBoundingBoxes(int32 Padding, TArray<AActor*> Actors, int32 minPixels, ESaveType SaveType)
{
	FString csvString;
	UTextureRenderTarget2D* RenderTexture = FindFirstObjectSafe<UTextureRenderTarget2D>(TEXT("RT_2DBB"));
	// if Actors is empty, find all actors with tag "BB"
	if (Actors.Num() == 0)
	{
		Actors = UBoundingBoxesDetector::FindActors("BB");
	}

	int32 line_nb = 0;
	for (int32 i = 0; i < Actors.Num(); i++) {
		FName tag = Actors[i]->Tags[0];
		FString tagString = tag.ToString();
		FString boxString = UBoundingBoxesDetector::Get2DBoundingBox(RenderTexture, i+1, 1, Padding, minPixels, SaveType);

		/* if (SaveType == ESaveType::YOLO){
			// replace commas with spaces
			boxString = boxString.Replace(*FString(","), *FString(" "));
			tagString = tag.ToString() + " ";
		} */

		if (boxString != "") {
			boxString = tagString + boxString;
			if (line_nb == 0) {
				csvString = boxString;
			}
			else {
				csvString += "\n" + boxString;
			}
			line_nb++;
		}
	}
	return csvString;
}




FString UBoundingBoxesDetector::Get2DBoundingBox(UTextureRenderTarget2D* RenderTexture, int32 label, int32 ResMultiplier, int32 padding, int32 minPixels, ESaveType SaveType)
{
	FWorldContext* world = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	UWorld* World = world->World();

	TArray<FColor> ImageData;

	UKismetRenderingLibrary::ReadRenderTarget(World, RenderTexture, ImageData, true);
	FIntVector2 maxPoint(0, 0);
	FIntVector2 minPoint(RenderTexture->SizeX, RenderTexture->SizeY);

	int32 PixelCount = 0;

	for (int32 x = 0; x < RenderTexture->SizeX; x++) {
		for (int32 y = 0; y < RenderTexture->SizeY; y++) {
			int32 i = x + y * RenderTexture->SizeX;
			float pixelValue = ImageData[i].R;
			int32 int_pixelValue = static_cast<int32>(round(pixelValue));
			if (int_pixelValue == label) {
				// we have a labeled pixel
				PixelCount++;
				if (x <= minPoint.X) {
					minPoint.X = x;
				}
				if (y <= minPoint.Y) {
					minPoint.Y = y;
				}
				if (x >= maxPoint.X) {
					maxPoint.X = x;
				}
				if (y >= maxPoint.Y) {
					maxPoint.Y = y;
				}
			}
		}
	}
	
	// return string with minPoint and maxPoint
	if (PixelCount <= minPixels) {
		return "";
	}
	else if (SaveType == ESaveType::CSV) {
		FString boxString = "," + FString::FromInt(FMath::Clamp(minPoint.X - padding, 0, RenderTexture->SizeX)) + "," +
															FString::FromInt(FMath::Clamp(minPoint.Y - padding, 0, RenderTexture->SizeY)) + "," +
															FString::FromInt(FMath::Clamp(maxPoint.X + padding, 0, RenderTexture->SizeX)) + "," +
															FString::FromInt(FMath::Clamp(maxPoint.Y + padding, 0, RenderTexture->SizeY));
		return boxString;
	} 
	else {
		float maxPoint_X = maxPoint.X ;
		float maxPoint_Y = maxPoint.Y ;
		float minPoint_X = minPoint.X ;
		float minPoint_Y = minPoint.Y ;

		float x_center = (minPoint_X + maxPoint_X) / (2 * float(RenderTexture->SizeX));
		float y_center = (minPoint_Y + maxPoint_Y) / (2 * float(RenderTexture->SizeY));
		float width = (maxPoint_X - minPoint_X) / float(RenderTexture->SizeX);
		float height = (maxPoint_Y - minPoint_Y) / float(RenderTexture->SizeY);

		FString boxString = " " + FString::SanitizeFloat(FMath::Clamp(x_center, 0, 1)) + " " +
															FString::SanitizeFloat(FMath::Clamp(y_center, 0, 1)) + " " +
															FString::SanitizeFloat(FMath::Clamp(width, 0, 1)) + " " +
															FString::SanitizeFloat(FMath::Clamp(height, 0, 1));
		return boxString;
	}
	
}

TArray<AActor*> UBoundingBoxesDetector::FindActors(FName Tag)
{
	TArray<AActor*> FoundActors;
	FWorldContext* world = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	UWorld* World = world->World();
	if (World) {
		UGameplayStatics::GetAllActorsWithTag(World, Tag, FoundActors);
		if (FoundActors.Num() > 0) {
			return FoundActors;
		}
	}
	return FoundActors;
}

void UBoundingBoxesDetector::SetStencil(AActor* Actor, int32 StencilNumber)
{
	TArray<UMeshComponent*> Components;
	Actor->GetComponents<UMeshComponent>(Components);
	for (int32 i = 0; i < Components.Num(); i++) {
		Components[i]->SetRenderCustomDepth(true);
		Components[i]->SetCustomDepthStencilValue(StencilNumber);
	}
}

void UBoundingBoxesDetector::SetStencils()
{
	TArray<AActor*> FoundActors = UBoundingBoxesDetector::FindActors("BB");
	for (int32 i = 0; i < FoundActors.Num(); i++) {
		UBoundingBoxesDetector::SetStencil(FoundActors[i], i+1);
	}
}

void UBoundingBoxesDetector::SaveCSV(FString csvString, FString SaveDirectory, FString FileName)
{
	FString TextToSave = csvString;
	FFileHelper::SaveStringToFile(TextToSave, *(SaveDirectory + "/" + FileName));
}
