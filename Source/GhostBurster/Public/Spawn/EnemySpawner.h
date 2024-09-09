#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Player/PlayerSplinePath.h"
#include "Player/VRPlayerCharacter.h"
#include "Enemy/Enemys.h"

#include "EnemySpawner.generated.h"

USTRUCT(BlueprintType)
struct FEnemySpawnInfo
{
    GENERATED_BODY()

    //出現するステージ番号
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Spawn Info")
        int32 Wave;

    //出現する敵の種類
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Spawn Info")
        FString Type;

    //出現する座標（Spawnerからの相対座標）
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Spawn Info")
        FVector StartLocation;

    //生成する敵の遅延時間（float）
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Spawn Info")
        float DelayTime;

    //最後に生成する敵かどうか
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Spawn Info")
        bool LastEnemy;

    //------------------------------------------------------------------
    // 以下、敵にセットする値
    //------------------------------------------------------------------

    //敵の目標地点（生成地点からの相対座標）
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Spawn Info")
        TArray<FVector> GoalLocations;

    //敵の移動時間（秒）
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Spawn Info")
        int32 MoveTime;

    //敵の体力
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Spawn Info")
        int32 EnemyHP;

    //敵の移動が終わってから攻撃するまでの時間（秒）
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Spawn Info")
        int32 AttackTime;    

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
    void SpawnEnemy(const FEnemySpawnInfo& SpawnInfo);

    UFUNCTION(BlueprintCallable, Category = "Enemy Spawning")
    void LoadSpawnInfoFromCSV(const FString& SpawnPath, const FString& TimerPath);

    UFUNCTION(BlueprintCallable, Category = "Enemy Spawning")
    void EnemyDeadFunction();

private:
    void LogCurrentEnemyCount(const int32& Wave) const;
    void LogAttemptingToSpawn(const FString& EnemyType, const FVector& Location) const;
    void LogSpawnedEnemy(const FString& EnemyType, const FVector& Location) const;
    void LogFailedSpawn(const FString& EnemyType, const FVector& Location) const;
    void LogEnemyClassNotFound(const FString& EnemyType) const;
    void LogSpawnInfoArray() const;
    bool FStringToBool(const FString& String) const;
    TArray<FEnemySpawnInfo> ParseCSV_SpawnData(const FString& FilePath) const;
    TArray<FEnemySpawnInfo> SpawnInfoArray;
    int32 MaxStageNumber;
    mutable TArray<int32> WaveEnemyCount;

    void HandleEnemyCountZero(); // EnemyCountが0になったときの処理を実装するためのメソッド
    TObjectPtr<AVRPlayerCharacter> Player;

    TArray<int32> ParseCSV_StageTime(const FString& FilePath) const;
    TArray<int32> WaveTime;
    // Timer Handles for each wave
    FTimerHandle WaveTimerHandle;

    // 敵の生成遅延時間を格納する配列
    TArray<FTimerHandle> EnemyTimerHandles;

    // ステージ内の敵の配列
    TArray<AEnemys*> SpawnEnemies;

};