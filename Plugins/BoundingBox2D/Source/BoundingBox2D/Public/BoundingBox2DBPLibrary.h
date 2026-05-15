// Copyright Plasma Labs, 2023. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SaveType.h"
#include "BoundingBox2DBPLibrary.generated.h"

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/


UCLASS()
class UBoundingBox2DBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get 2D Bounding Boxes", Keywords = "2D bounding box"), Category = "2D Bounding Box Tool")
		static TArray<FString> BoundingBox2DGet2DBB(int32 padding = 0, int32 minPixels = 0);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get 2D Bounding Boxes From Actors List", Keywords = "2D bounding box"), Category = "2D Bounding Box Tool")
		static TArray<FString> BoundingBox2DGet2DBBFromActors(TArray<AActor*> Actors, int32 padding = 0, int32 minPixels = 0);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Take Screenshot With Bounding Box", Keywords = "2D bounding box screenshots", AdvancedDisplay = "2"), Category = "2D Bounding Box Tool")
		static void BoundingBox2DTakeScreenShot(FString Path, int32 padding = 0, int32 minPixels = 0, ESaveType SaveType = ESaveType::CSV);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Setup 2D Bounding Boxes", Keywords = "2D bounding box stencils setup"), Category = "2D Bounding Box Tool")
		static void BoundingBox2DApplyStencilValues();
};
