// Copyright Plasma Labs, 2023. All Rights Reserved.


#include "BB2D_TakeCaptures.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Engine/World.h"
#include "Misc/LocalTimestampDirectoryVisitor.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

UBB2D_TakeCaptures::UBB2D_TakeCaptures()
{
}

void UBB2D_TakeCaptures::SetRenderTextureSize(int32 SizeX, int32 SizeY)
{
	UTextureRenderTarget2D* RenderTarget_Capture = FindFirstObjectSafe<UTextureRenderTarget2D>(TEXT("RT_Capture"));
	RenderTarget_Capture->ResizeTarget(SizeX, SizeY);

	UTextureRenderTarget2D* RenderTarget_Labels = FindFirstObjectSafe<UTextureRenderTarget2D>(TEXT("RT_2DBB"));
	RenderTarget_Labels->ResizeTarget(SizeX, SizeY);
}

void UBB2D_TakeCaptures::TakeCapture(FString Path, int32 i)
{
	// find UTextureRenderTarget2D
	UTextureRenderTarget2D* RenderTarget = FindFirstObjectSafe<UTextureRenderTarget2D>(TEXT("RT_Capture"));
	FString filename = FString::FromInt(i) + ".png";

	UKismetRenderingLibrary::ExportRenderTarget(GWorld, RenderTarget, Path, filename);

}


int32 UBB2D_TakeCaptures::GetLastIndexInDirectory(const FString directory, FString Extension)
	{
		IFileManager* FileManager  = &IFileManager::Get();
		static int32 LastScreenShotIndex = 0;
		int32 SearchIndex = 0;

		FString File = FString::Printf(TEXT("%s/%i%s"), *directory, LastScreenShotIndex, *Extension);

		if (FileManager->FileExists(*File))
		{
			SearchIndex = LastScreenShotIndex+1;
		}
		
		for( int32 TestIndex = SearchIndex; TestIndex < 100000; ++TestIndex )
		{
			File = FString::Printf(TEXT("%s/%i%s"), *directory, TestIndex, *Extension);
			if( FileManager->FileExists(*File) == false)
			{
				LastScreenShotIndex = TestIndex;
				break;
			}
		}
		return LastScreenShotIndex;
	}
