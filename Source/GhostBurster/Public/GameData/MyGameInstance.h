#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

UCLASS()
class GHOSTBURSTER_API UMyGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    // 保存するデータ
    UPROPERTY(BlueprintReadWrite, Category="Game Data")
    float EscapeTime;

    UPROPERTY(BlueprintReadWrite, Category="Game Data")
    int32 GhostsDefeated;

    UPROPERTY(BlueprintReadWrite, Category="Game Data")
    FString TotalScore;

    // データのリセット
    UFUNCTION(BlueprintCallable, Category="Game Data")
    void ResetGameData();

    int32 DefeatedGhostCount;

    UMyGameInstance();

    void IncrementDefeatedGhostCount();
};