// Fill out your copyright notice in the Description page of Project Settings.

#include "Spawn.h"
#include "Enemy/Enemys.h" 

// Sets default values
ASpawn::ASpawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	static ConstructorHelpers::FClassFinder<AEnemys> EnemyClassFinder(TEXT("/Game/EnemyBP"));
	//DefaultEnemyClass = EnemyClassFinder.Class;
}

void ASpawn::BeginPlay()
{
    Super::BeginPlay();

    // イベントディスパッチャーをバインド
    OnSpawn.Broadcast();
}

void ASpawn::SpawnEnemies(FVector SpawnLocation, FRotator SpawnRotation, int32 NumEnemies)
{
    UWorld* World = GetWorld();
    if (World)
    {
        for (int32 i = 0; i < NumEnemies; ++i)
        {
            // スポーン位置を微調整する場合、ランダムなオフセットを加えることができます
            FVector AdjustedSpawnLocation = SpawnLocation + FVector(0.f, 0.f, i * 100.f); // 例として高さを100.fずつ増やす

            // Enemyをスポーンさせる処理
            AEnemys* NewEnemy = World->SpawnActor<AEnemys>(AEnemys::StaticClass(), AdjustedSpawnLocation, SpawnRotation);
            if (NewEnemy)
            {
                // スポーン成功時の追加処理
            }
            else
            {
                // スポーン失敗時の処理
            }
        }
    }
}