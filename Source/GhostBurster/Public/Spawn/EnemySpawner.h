#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Player/PlayerSplinePath.h"

#include "EnemySpawner.generated.h"

USTRUCT(BlueprintType)
struct FEnemySpawnInfo
{
    GENERATED_BODY()

    //出現するステージ番号
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Info")
        int32 Wave;

    //出現する敵の種類
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Info")
        FString Type;

    //出現する座標（Spawnerからの相対座標）
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Info")
        FVector StartLocation;

    //------------------------------------------------------------------
    // 以下、敵にセットする値
    //------------------------------------------------------------------

    //敵の目標地点（生成地点からの相対座標）
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Info")
        FVector GoalLocation;

    //敵の移動時間（秒）
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Info")
        int32 MoveTime;

    //敵の体力
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Info")
        int32 EnemyHP;

    //敵の移動が終わってから攻撃するまでの時間（秒）
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Info")
        int32 AttackTime;

    //以下、必要に応じて追加



};

UCLASS()
class GHOSTBURSTER_API AEnemySpawner : public AActor
{
    GENERATED_BODY()

public:
    AEnemySpawner();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(BlueprintReadOnly, Category = "PlayerSpline")
        TObjectPtr<APlayerSplinePath> PlayerSpline;


public:
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Enemy Spawning")
    void SpawnEnemiesForWave(int32 Wave);

    UFUNCTION(BlueprintCallable, Category = "Enemy Spawning")
    void LoadSpawnInfoFromCSV(const FString& SpawnPath, const FString& TimerPath);

    UFUNCTION(BlueprintCallable, Category = "Enemy Spawning")
    void EnemyDeadFunction();

private:
    void LogCurrentEnemyCount() const;
    void LogAttemptingToSpawn(const FString& EnemyType, const FVector& Location) const;
    void LogSpawnedEnemy(const FString& EnemyType, const FVector& Location) const;
    void LogFailedSpawn(const FString& EnemyType, const FVector& Location) const;
    void LogEnemyClassNotFound(const FString& EnemyType) const;
    void LogSpawnInfoArray() const;
    TArray<FEnemySpawnInfo> ParseCSV_SpawnData(const FString& FilePath) const;
    TArray<FEnemySpawnInfo> SpawnInfoArray;
    int32 EnemyCount;

    void HandleEnemyCountZero(); // EnemyCountが0になったときの処理を実装するためのメソッド
    AActor* GetPlayerActor() const; // プレイヤーアクターを取得するメソッド

    TArray<int32> ParseCSV_StageTime(const FString& FilePath) const;
    TArray<int32> WaveTime;
    // Timer Handles for each wave
    FTimerHandle WaveTimerHandle;

};