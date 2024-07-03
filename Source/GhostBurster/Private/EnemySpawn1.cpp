// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawn1.h"
#include "Enemys.h" 
#include "Components/BoxComponent.h" // 例としてBoxComponentを使用する場合のインクルード
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemySpawn1::AEnemySpawn1()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemySpawn1::BeginPlay()
{
    Super::BeginPlay();

    // イベントディスパッチャーのバインド
    OnSpawnEnemy1.AddDynamic(this, &AEnemySpawn1::HandleSpawnEvent);

    // 例えば、BoxComponentを使用してコリジョンイベントを設定する場合の例
    UBoxComponent* BoxComponent = FindComponentByClass<UBoxComponent>();
    if (BoxComponent)
    {
        BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemySpawn1::HandleSpawnEvent);
    }
}

void AEnemySpawn1::HandleSpawnEvent()
{
    // スポーン位置と回転の設定例
    FVector SpawnLocation = GetActorLocation(); // 自身の位置をスポーン位置とする
    FRotator SpawnRotation = GetActorRotation(); // 自身の回転をスポーン回転とする

    // スポーンするEnemyの数と位置調整
    int32 NumEnemiesToSpawn = 3; // 例として3体スポーンさせる
    SpawnEnemies(SpawnLocation, SpawnRotation, NumEnemiesToSpawn);
}

void AEnemySpawn1::SpawnEnemies(FVector SpawnLocation, FRotator SpawnRotation, int32 NumEnemies)
{
    UWorld* World = GetWorld();
    if (World)
    {
        for (int32 i = 0; i < NumEnemies; ++i)
        {
            // スポーン位置を微調整する場合、ランダムなオフセットを加えることができます
            FVector AdjustedSpawnLocation = SpawnLocation + FVector(7750.f, -2090.f + i * 800.f, 780.f); // 例として高さを100.fずつ増やす

            // Enemyをスポーンさせる処理
            AEnemy* NewEnemy = World->SpawnActor<AEnemy>(AEnemy::StaticClass(), AdjustedSpawnLocation, SpawnRotation);
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
