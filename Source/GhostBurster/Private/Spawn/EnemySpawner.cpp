#include "Spawn/EnemySpawner.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Enemy/NormalEnemy.h"
#include "Enemy/GreenEnemy.h"
#include "Enemy/RedEnemy.h"
#include "Enemy/BlueEnemy.h"
#include "Enemy/BossEnemy.h"

AEnemySpawner::AEnemySpawner()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AEnemySpawner::BeginPlay()
{
    Super::BeginPlay();
}

void AEnemySpawner::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

TArray<FEnemySpawnInfo> AEnemySpawner::ParseCSV(const FString& FilePath)
{
    TArray<FEnemySpawnInfo> ParsedSpawnInfoArray;
    FString FileData;
    if (FFileHelper::LoadFileToString(FileData, *FilePath))
    {
        TArray<FString> Lines;
        FileData.ParseIntoArrayLines(Lines);

        for (const FString& Line : Lines)
        {
            if (Line.IsEmpty())
            {
                continue;
            }

            TArray<FString> Values;
            Line.ParseIntoArray(Values, TEXT(","), true);

            if (Values.Num() == 5)
            {
                FEnemySpawnInfo SpawnInfo;
                SpawnInfo.Wave = FCString::Atoi(*Values[0]);
                SpawnInfo.Type = Values[1];
                double X = FCString::Atof(*Values[2]);
                double Y = FCString::Atof(*Values[3]);
                double Z = FCString::Atof(*Values[4]);
                SpawnInfo.Location = FVector(X, Y, Z);
                ParsedSpawnInfoArray.Add(SpawnInfo);
            }
        }
    }
    return ParsedSpawnInfoArray;
}

void AEnemySpawner::LoadSpawnInfoFromCSV(const FString& FilePath)
{
    SpawnInfoArray = ParseCSV(FilePath);
}

void AEnemySpawner::SpawnEnemiesForWave(int32 Wave)
{
    for (const FEnemySpawnInfo& SpawnInfo : SpawnInfoArray)
    {
        if (SpawnInfo.Wave == Wave)
        {
            // スポーンする敵のクラスをTypeに基づいて選択します（例：White、Green、Red、Blue、Boss）
            TSubclassOf<AActor> EnemyClass = nullptr;

            if (SpawnInfo.Type == "White")
            {
                EnemyClass = ANormalEnemy::StaticClass();
            }
            else if (SpawnInfo.Type == "Green")
            {
                EnemyClass = AGreenEnemy::StaticClass();
            }
            else if (SpawnInfo.Type == "Red")
            {
                EnemyClass = ARedEnemy::StaticClass();
            }
            else if (SpawnInfo.Type == "Blue")
            {
                EnemyClass = ABlueEnemy::StaticClass();
            }
            else if (SpawnInfo.Type == "Boss")
            {
                EnemyClass = ABossEnemy::StaticClass();
            }

            if (EnemyClass)
            {
                GetWorld()->SpawnActor<AActor>(EnemyClass, SpawnInfo.Location, FRotator::ZeroRotator);
            }
        }
    }
}