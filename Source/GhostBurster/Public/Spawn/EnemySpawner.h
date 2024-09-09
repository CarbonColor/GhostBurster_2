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

    //�o������X�e�[�W�ԍ�
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Spawn Info")
        int32 Wave;

    //�o������G�̎��
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Spawn Info")
        FString Type;

    //�o��������W�iSpawner����̑��΍��W�j
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Spawn Info")
        FVector StartLocation;

    //��������G�̒x�����ԁifloat�j
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Spawn Info")
        float DelayTime;

    //�Ō�ɐ�������G���ǂ���
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Spawn Info")
        bool LastEnemy;

    //------------------------------------------------------------------
    // �ȉ��A�G�ɃZ�b�g����l
    //------------------------------------------------------------------

    //�G�̖ڕW�n�_�i�����n�_����̑��΍��W�j
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Spawn Info")
        TArray<FVector> GoalLocations;

    //�G�̈ړ����ԁi�b�j
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Spawn Info")
        int32 MoveTime;

    //�G�̗̑�
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Spawn Info")
        int32 EnemyHP;

    //�G�̈ړ����I����Ă���U������܂ł̎��ԁi�b�j
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

    void HandleEnemyCountZero(); // EnemyCount��0�ɂȂ����Ƃ��̏������������邽�߂̃��\�b�h
    TObjectPtr<AVRPlayerCharacter> Player;

    TArray<int32> ParseCSV_StageTime(const FString& FilePath) const;
    TArray<int32> WaveTime;
    // Timer Handles for each wave
    FTimerHandle WaveTimerHandle;

    // �G�̐����x�����Ԃ��i�[����z��
    TArray<FTimerHandle> EnemyTimerHandles;

    // �X�e�[�W���̓G�̔z��
    TArray<AEnemys*> SpawnEnemies;

};