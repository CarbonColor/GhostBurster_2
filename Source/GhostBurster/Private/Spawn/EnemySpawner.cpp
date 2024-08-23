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

DEFINE_LOG_CATEGORY_STATIC(LogEnemySpawner, Log, All);

AEnemySpawner::AEnemySpawner()
{
    PrimaryActorTick.bCanEverTick = true;
    EnemyCount = 0;
    Tags.Add(FName("Spawner"));
}

void AEnemySpawner::BeginPlay()
{
    Super::BeginPlay();

    FString FilePath = FPaths::ProjectContentDir() + TEXT("_TeamFolder/map/enemy_spawn_data.csv");
    //UE_LOG(LogEnemySpawner, Log, TEXT("Looking for file at: %s"), *FilePath);

    if (FPaths::FileExists(FilePath))
    {
        LoadSpawnInfoFromCSV(FilePath);
        //UE_LOG(LogEnemySpawner, Log, TEXT("CSV file successfully loaded: %s"), *FilePath);
    }
    else
    {
        UE_LOG(LogEnemySpawner, Warning, TEXT("CSV file not found at: %s"), *FilePath);
    }
}

void AEnemySpawner::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

TArray<FEnemySpawnInfo> AEnemySpawner::ParseCSV(const FString& FilePath) const
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

void AEnemySpawner::LogCurrentEnemyCount() const
{
    UE_LOG(LogEnemySpawner, Warning, TEXT("Current EnemyCount: %d"), EnemyCount);
}

void AEnemySpawner::LogAttemptingToSpawn(const FString& EnemyType, const FVector& Location) const
{
    UE_LOG(LogEnemySpawner, Warning, TEXT("Attempting to spawn enemy: %s at Location: (%f, %f, %f)"),
        *EnemyType, Location.X, Location.Y, Location.Z);
}

void AEnemySpawner::LogSpawnedEnemy(const FString& EnemyType, const FVector& Location) const
{
    UE_LOG(LogEnemySpawner, Warning, TEXT("Spawned %s at Location: (%f, %f, %f)"),
        *EnemyType, Location.X, Location.Y, Location.Z);
}

void AEnemySpawner::LogFailedSpawn(const FString& EnemyType, const FVector& Location) const
{
    UE_LOG(LogEnemySpawner, Error, TEXT("Failed to spawn %s at Location: (%f, %f, %f)"),
        *EnemyType, Location.X, Location.Y, Location.Z);
}

void AEnemySpawner::LogEnemyClassNotFound(const FString& EnemyType) const
{
    UE_LOG(LogEnemySpawner, Error, TEXT("EnemyClass not found for Type: %s"), *EnemyType);
}

void AEnemySpawner::LogSpawnInfoArray() const
{
    for (const FEnemySpawnInfo& SpawnInfo : SpawnInfoArray)
    {
        UE_LOG(LogEnemySpawner, Warning, TEXT("SpawnInfo: Wave: %d, Type: %s, Location: (%f, %f, %f)"),
            SpawnInfo.Wave, *SpawnInfo.Type, SpawnInfo.Location.X, SpawnInfo.Location.Y, SpawnInfo.Location.Z);
    }
}

void AEnemySpawner::LoadSpawnInfoFromCSV(const FString& FilePath)
{
    if (!FPaths::FileExists(FilePath))
    {
        UE_LOG(LogEnemySpawner, Warning, TEXT("CSV file not found: %s"), *FilePath);
        return;
    }

    SpawnInfoArray = ParseCSV(FilePath);

    if (SpawnInfoArray.Num() == 0)
    {
        UE_LOG(LogEnemySpawner, Warning, TEXT("No spawn info parsed from CSV file: %s"), *FilePath);
    }
}

void AEnemySpawner::SpawnEnemiesForWave(int32 Wave)
{
    UE_LOG(LogEnemySpawner, Warning, TEXT("SpawnEnemiesForWave called for Wave: %d"), Wave);

    LoadSpawnInfoFromCSV(FPaths::ProjectContentDir() + TEXT("_TeamFolder/map/enemy_spawn_data.csv"));

    //LogSpawnInfoArray();

    for (const FEnemySpawnInfo& SpawnInfo : SpawnInfoArray)
    {
        if (SpawnInfo.Wave == Wave)
        {
            EnemyCount += 1;
            LogCurrentEnemyCount();
            LogAttemptingToSpawn(SpawnInfo.Type, SpawnInfo.Location);

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
                UE_LOG(LogEnemySpawner, Warning, TEXT("Spawning enemy class: %s"), *EnemyClass->GetName());

                AActor* SpawnedEnemy = GetWorld()->SpawnActor<AActor>(EnemyClass, SpawnInfo.Location, FRotator::ZeroRotator);

                if (SpawnedEnemy)
                {
                    LogSpawnedEnemy(SpawnInfo.Type, SpawnInfo.Location);
                }
                else
                {
                    LogFailedSpawn(SpawnInfo.Type, SpawnInfo.Location);
                }
            }
            else
            {
                LogEnemyClassNotFound(SpawnInfo.Type);
            }
        }
    }
}

void AEnemySpawner::EnemyDeadFunction()
{
    UE_LOG(LogEnemySpawner, Warning, TEXT("EnemyDeadFunction called"));

    EnemyCount--;
    LogCurrentEnemyCount();

    if (EnemyCount <= 0)
    {
        HandleEnemyCountZero();
    }
}

void AEnemySpawner::HandleEnemyCountZero()
{
    AActor* PlayerActor = GetPlayerActor();

    if (PlayerActor)
    {
        UFunction* StartMovementFunction = PlayerActor->FindFunction(TEXT("StartMovement"));
        if (StartMovementFunction)
        {
            PlayerActor->ProcessEvent(StartMovementFunction, nullptr);
        }
    }

    TArray<AActor*> OverlappingActors;
    if (PlayerActor)
    {
        PlayerActor->GetOverlappingActors(OverlappingActors);

        for (AActor* Actor : OverlappingActors)
        {
            if (Actor)
            {
                Actor->Destroy();
            }
        }
    }
}

AActor* AEnemySpawner::GetPlayerActor() const
{
    return GetWorld() ? GetWorld()->GetFirstPlayerController()->GetPawn() : nullptr;
}