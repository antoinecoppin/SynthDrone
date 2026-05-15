#pragma once

#include "CoreMinimal.h"
#include "SaveType.generated.h"

UENUM(BlueprintType)
enum class ESaveType : uint8
{
    CSV UMETA(DisplayName = "CSV"),
    YOLO UMETA(DisplayName = "YOLO (txt))"),
};