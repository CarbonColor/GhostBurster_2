// Fill out your copyright notice in the Description page of Project Settings.

#include "Spawn/EnemySpawn11.h"
#include "Enemy/Enemys.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/Blueprint.h"

AEnemySpawn11::AEnemySpawn11()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AEnemySpawn11::BeginPlay()
{
    Super::BeginPlay();

    FString BlueprintPath = TEXT("/Script/Engine.Blueprint'/Game/_TeamFolder/map/EnemySpawn1_BP.EnemySpawn1_BP'"); // ブループリントのパスを指定する
    UBlueprint* BlueprintObj = Cast<UBlueprint>(StaticLoadObject(UBlueprint::StaticClass(), nullptr, *BlueprintPath));
  
    if (BlueprintObj)
    {
        UClass* BPClass = BlueprintObj->GeneratedClass;
        if (BPClass)
        {
            AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(BPClass, FVector::ZeroVector, FRotator::ZeroRotator);
            if (SpawnedActor)
            {
                // オーバーラップイベントを取得
                UBoxComponent* BoxComponent = SpawnedActor->FindComponentByClass<UBoxComponent>();
                if (BoxComponent)
                {
                    BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemySpawn11::HandleOverlap);
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("BoxComponent not found on spawned actor."));
                }
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load blueprint class: %s"), *BlueprintPath);
    }
}

void AEnemySpawn11::HandleSpawnEvent()
{
    FVector SpawnLocation = GetActorLocation();
    FRotator SpawnRotation = GetActorRotation();

    int32 NumEnemiesToSpawn = 4; // 4体出現
    SpawnEnemies(SpawnLocation, SpawnRotation, NumEnemiesToSpawn);
}

void AEnemySpawn11::SpawnEnemies(FVector SpawnLocation, FRotator SpawnRotation, int32 NumEnemies)
{
    UWorld* World = GetWorld();
    if (World)
    {
        FString ClassPath = TEXT("Blueprint'/Game/Path/To/Your/BlueprintClass.YourBlueprintClass_C'");
        UBlueprint* BP = Cast<UBlueprint>(StaticLoadObject(UBlueprint::StaticClass(), nullptr, *ClassPath));

        if (BP && BP->GeneratedClass)
        {
            UClass* EnemyClass = BP->GeneratedClass;

            for (int32 i = 0; i < NumEnemies; ++i)
            {
                FVector AdjustedSpawnLocation = SpawnLocation + FVector(0.f, i * 200.f, 0.f); // Adjust Y position for each enemy
                AActor* NewEnemy = World->SpawnActor<AActor>(EnemyClass, AdjustedSpawnLocation, SpawnRotation);
                if (NewEnemy)
                {
                    // Additional processing upon successful spawn
                }
                else
                {
                    // Handle spawn failure
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Could not load the enemy blueprint class!"));
        }
    }
}
void AEnemySpawn11::HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    /*
    // ブループリントクラスのディスパッチャーを呼び出す
    AEnemySpawn1_BP* SpawnBP = Cast<AEnemySpawn1_BP>(OtherActor);
    if (SpawnBP)
    {
        SpawnBP->EnemySpawn1();
    }*/
}