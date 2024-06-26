#pragma once

#include "CoreMinimal.h"
#include "Flashlight_Enumeration.generated.h"

UENUM(BlueprintType)
enum class EFlashlight_Color : uint8
{
	//—ñ‹“q‚ğ‹Lq‚·‚é
	White = 0	UMETA(DisplayName = "White"),
	Green = 1	UMETA(DisplayName = "Green"),
	Red = 2		UMETA(DisplayName = "Red"),
	Blue = 3	UMETA(DisplayName = "Blue")
};