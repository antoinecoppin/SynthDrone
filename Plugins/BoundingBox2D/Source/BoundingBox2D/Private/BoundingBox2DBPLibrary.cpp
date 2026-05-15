// Copyright Plasma Labs, 2023. All Rights Reserved.

#include "BoundingBox2DBPLibrary.h"
#include "BoundingBox2D.h"
#include "BoundingBoxesDetector.h"
#include "BB2D_TakeCaptures.h"
#include "HAL/PlatformFilemanager.h"


UBoundingBox2DBPLibrary::UBoundingBox2DBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

TArray<FString> UBoundingBox2DBPLibrary::BoundingBox2DGet2DBB(int32 padding, int32 minPixels)
{
	TArray<AActor*> Actors;
	UBoundingBoxesDetector* BoundingBoxesDetector = NewObject<UBoundingBoxesDetector>();
	FString csvString = BoundingBoxesDetector->Get2DBoundingBoxes(padding, Actors, minPixels);
	// transform csvString to TArray<FString>
	TArray<FString> csvStringArray;
	csvString.ParseIntoArray(csvStringArray, TEXT("\n"), true);
	return csvStringArray;
}

TArray<FString> UBoundingBox2DBPLibrary::BoundingBox2DGet2DBBFromActors(TArray<AActor*> Actors, int32 padding, int32 minPixels)
{
	UBoundingBoxesDetector* BoundingBoxesDetector = NewObject<UBoundingBoxesDetector>();
	FString csvString = BoundingBoxesDetector->Get2DBoundingBoxes(padding, Actors, minPixels);
	// transform csvString to TArray<FString>
	TArray<FString> csvStringArray;
	csvString.ParseIntoArray(csvStringArray, TEXT("\n"), true);
	return csvStringArray;
}

void UBoundingBox2DBPLibrary::BoundingBox2DTakeScreenShot(FString Path, int32 padding, int32 minPixels, ESaveType SaveType)
{
	// setup save directory and file name
	FString LabelSaveDirectory = FString(Path + "/labels");
	FString LabelFileName;
	if (SaveType == ESaveType::CSV){
		LabelFileName = FString(".csv");
	} else {
		LabelFileName = FString(".txt");
	}
	FString PNGSaveDirectory = FString(Path + "/images");
	FString PNGFileName = FString(".png");

	int32 FileIndex = 0;
	FString FileNumberString = FString::FromInt(FileIndex);

	UBB2D_TakeCaptures* TakeCaptures = NewObject<UBB2D_TakeCaptures>();
	FileIndex = TakeCaptures->GetLastIndexInDirectory(LabelSaveDirectory, LabelFileName);
	FileNumberString = FString::FromInt(FileIndex);
	LabelFileName = FileNumberString + LabelFileName;

	UBoundingBoxesDetector* BoundingBoxesDetector = NewObject<UBoundingBoxesDetector>();
	TArray<AActor*> Actors;
	FString LabelString = BoundingBoxesDetector->Get2DBoundingBoxes(padding, Actors, minPixels, SaveType);

	BoundingBoxesDetector->SaveCSV(LabelString, LabelSaveDirectory, LabelFileName);
	TakeCaptures->TakeCapture(PNGSaveDirectory, FileIndex);
}

void UBoundingBox2DBPLibrary::BoundingBox2DApplyStencilValues()
{
	UBoundingBoxesDetector* BoundingBoxesDetector = NewObject<UBoundingBoxesDetector>();
	BoundingBoxesDetector->SetStencils();
}
