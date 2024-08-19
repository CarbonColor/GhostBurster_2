#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

USTRUCT(BlueprintType)
struct FEnemySpawnInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Info")
    int32 Wave;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Info")
    FString Type;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Info")
    FVector Location;
};

UCLASS()
class GHOSTBURSTER_API AEnemySpawner : public AActor
{
    GENERATED_BODY()

public:
    AEnemySpawner();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Enemy Spawning")
    void SpawnEnemiesForWave(int32 Wave);

    UFUNCTION(BlueprintCallable, Category = "Enemy Spawning")
    void LoadSpawnInfoFromCSV(const FString& FilePath);

private:
    TArray<FEnemySpawnInfo> ParseCSV(const FString& FilePath);
    TArray<FEnemySpawnInfo> SpawnInfoArray;
};