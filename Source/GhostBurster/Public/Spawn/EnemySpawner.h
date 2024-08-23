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

    UFUNCTION(BlueprintCallable, Category = "Enemy Spawning")
    void EnemyDeadFunction();

    UPROPERTY(BlueprintReadWrite, Category = "Enemy Spawning")
    int32 EnemyCount;
private:
    void LogCurrentEnemyCount() const;
    void LogAttemptingToSpawn(const FString& EnemyType, const FVector& Location) const;
    void LogSpawnedEnemy(const FString& EnemyType, const FVector& Location) const;
    void LogFailedSpawn(const FString& EnemyType, const FVector& Location) const;
    void LogEnemyClassNotFound(const FString& EnemyType) const;
    void LogSpawnInfoArray() const;
    TArray<FEnemySpawnInfo> ParseCSV(const FString& FilePath) const;
    TArray<FEnemySpawnInfo> SpawnInfoArray;

    void HandleEnemyCountZero(); // EnemyCount��0�ɂȂ����Ƃ��̏������������邽�߂̃��\�b�h
    AActor* GetPlayerActor() const; // �v���C���[�A�N�^�[���擾���郁�\�b�h
};