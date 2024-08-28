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
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

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

    //�X�v���C���̎擾
    PlayerSpline = Cast<APlayerSplinePath>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerSplinePath::StaticClass()));

    // ���X�|�[���f�[�^
    //FString FilePath = FPaths::ProjectContentDir() + TEXT("_TeamFolder/map/enemy_spawn_data.csv");
    // �V�X�|�[���f�[�^
    FString SpawnDataPath = FPaths::ProjectContentDir() + TEXT("_TeamFolder/map/EnemySpawnData.csv");
    FString StageDataPath = FPaths::ProjectContentDir() + TEXT("_TeamFolder/map/StageTimerData.csv");

    //UE_LOG(LogEnemySpawner, Log, TEXT("Looking for file at: %s"), *FilePath);

    if (FPaths::FileExists(SpawnDataPath) && FPaths::FileExists(StageDataPath))
    {
        LoadSpawnInfoFromCSV(SpawnDataPath, StageDataPath);
    }
    else
    {
        UE_LOG(LogEnemySpawner, Warning, TEXT("CSV file not found at: %s"), *SpawnDataPath);
    }
}

void AEnemySpawner::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

TArray<FEnemySpawnInfo> AEnemySpawner::ParseCSV_SpawnData(const FString& FilePath) const
{
    TArray<FEnemySpawnInfo> ParsedSpawnInfoArray;
    FString FileData;

    if (FFileHelper::LoadFileToString(FileData, *FilePath))
    {
        TArray<FString> Lines;
        FileData.ParseIntoArrayLines(Lines);

        for (const FString& Line : Lines)
        {
            //�s���� || �s�̍ŏ����u//�v�Ŏn�܂��Ă�����X���[
            if (Line.IsEmpty() || Line.StartsWith("//"))
            {
                continue;
            }

            //�J���}��؂�ŕ���
            TArray<FString> Values;
            Line.ParseIntoArray(Values, TEXT(","), true);

            //�f�[�^�̕ϊ�
            FEnemySpawnInfo SpawnInfo;
            //�o������X�e�[�W�ԍ�
            SpawnInfo.Wave = FCString::Atoi(*Values[0]);
            //�o������G�̎��
            SpawnInfo.Type = Values[1];
            //�o��������W
            float SX = FCString::Atof(*Values[2]);
            float SY = FCString::Atof(*Values[3]);
            float SZ = FCString::Atof(*Values[4]);
            SpawnInfo.StartLocation = FVector(SX, SY, SZ);

            //�G�̖ڕW�n�_
            float GX = FCString::Atof(*Values[5]);
            float GY = FCString::Atof(*Values[6]);
            float GZ = FCString::Atof(*Values[7]);
            SpawnInfo.GoalLocation = FVector(GX, GY, GZ);
            //�G�̈ړ�����
            SpawnInfo.MoveTime = FCString::Atoi(*Values[8]);
            //�G�̗̑�
            SpawnInfo.EnemyHP = FCString::Atoi(*Values[9]);
            //�G�̈ړ����I����Ă���U������܂ł̎���
            SpawnInfo.AttackTime = FCString::Atoi(*Values[10]);
            //�ȉ��A�K�v�ɉ����Ēǉ�


            ParsedSpawnInfoArray.Add(SpawnInfo);
            //GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Yellow, TEXT("Add Enemy Data"));
        }
    }

    return ParsedSpawnInfoArray;
}

TArray<int32> AEnemySpawner::ParseCSV_StageTime(const FString& FilePath) const
{
    TArray<int32> ParsedStageTimer;
    FString FileData;

    if (FFileHelper::LoadFileToString(FileData, *FilePath))
    {
        TArray<FString> Lines;
        FileData.ParseIntoArrayLines(Lines);

        for (const FString& Line : Lines)
        {
            //�s���� || �s�̍ŏ����u//�v�Ŏn�܂��Ă�����X���[
            if (Line.IsEmpty() || Line.StartsWith("//"))
            {
                continue;
            }

            //�J���}��؂�ŕ���
            TArray<FString> Values;
            Line.ParseIntoArray(Values, TEXT(","), true);

            ParsedStageTimer.Add(FCString::Atoi(*Values[0]));
            //GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Yellow, TEXT("Add Timer Data"));
        }
    }
    return ParsedStageTimer;
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
            SpawnInfo.Wave, *SpawnInfo.Type, SpawnInfo.StartLocation.X, SpawnInfo.StartLocation.Y, SpawnInfo.StartLocation.Z);
    }
}

void AEnemySpawner::LoadSpawnInfoFromCSV(const FString& SpawnPath, const FString& TimerPath)
{
    if (!FPaths::FileExists(SpawnPath))
    {
        UE_LOG(LogEnemySpawner, Warning, TEXT("CSV file not found: %s"), *SpawnPath);
        return;
    }

    SpawnInfoArray = ParseCSV_SpawnData(SpawnPath);
    WaveTime = ParseCSV_StageTime(TimerPath);

    if (SpawnInfoArray.Num() == 0)
    {
        UE_LOG(LogEnemySpawner, Warning, TEXT("No spawn info parsed from CSV file: %s"), *SpawnPath);
    }
}

void AEnemySpawner::SpawnEnemiesForWave(int32 Wave)
{
    UE_LOG(LogEnemySpawner, Warning, TEXT("SpawnEnemiesForWave called for Wave: %d"), Wave);

    //LoadSpawnInfoFromCSV(FPaths::ProjectContentDir() + TEXT("_TeamFolder/map/enemy_spawn_data.csv"));
    //LogSpawnInfoArray();

    //�G�̐���
    for (const FEnemySpawnInfo& SpawnInfo : SpawnInfoArray)
    {
        if (SpawnInfo.Wave == Wave)
        {
            EnemyCount += 1;
            LogCurrentEnemyCount();
            LogAttemptingToSpawn(SpawnInfo.Type, SpawnInfo.StartLocation);

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

                //�G�̏o��
                AActor* SpawnedEnemy = GetWorld()->SpawnActor<AActor>(EnemyClass, GetActorLocation() + SpawnInfo.StartLocation, FRotator::ZeroRotator);
                //�o�������G�̃X�e�[�^�X�ݒ�
                if (AEnemys* Enemy = Cast<AEnemys>(SpawnedEnemy))
                {
                    Enemy->SetInitialData(SpawnInfo.EnemyHP, SpawnInfo.AttackTime, SpawnInfo.GoalLocation, SpawnInfo.MoveTime);
                    //Enemy->SetHP(SpawnInfo.EnemyHP);
                    //Enemy->SetGoalLocation(SpawnInfo.GoalLocation);
                    //Enemy->SetMoveTime(SpawnInfo.MoveTime);
                    //Enemy->SetAttackUpToTime(SpawnInfo.AttackTime);
                }

                if (SpawnedEnemy)
                {
                    LogSpawnedEnemy(SpawnInfo.Type, SpawnInfo.StartLocation);
                }
                else
                {
                    LogFailedSpawn(SpawnInfo.Type, SpawnInfo.StartLocation);
                }
            }
            else
            {
                LogEnemyClassNotFound(SpawnInfo.Type);
            }
        }
    }
    // �^�C�}�[�̐ݒ�
    if (WaveTime.Num() != 0 && WaveTime[Wave - 1] != -1)
    {
        GetWorld()->GetTimerManager().SetTimer(WaveTimerHandle, this, &AEnemySpawner::HandleEnemyCountZero, WaveTime[Wave - 1], false);
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, FString::Printf(TEXT("Setting Timer : %d second"), WaveTime[Wave - 1]));
    }
}

void AEnemySpawner::EnemyDeadFunction()
{
    UE_LOG(LogEnemySpawner, Warning, TEXT("EnemyDeadFunction called"));

    if (PlayerSpline->IsMoving() == false)
    {
        EnemyCount--;
        LogCurrentEnemyCount();

        if (EnemyCount == 0)
        {
            HandleEnemyCountZero();
        }
    }
}

void AEnemySpawner::HandleEnemyCountZero()
{
    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("Call Handle -EnemyZero-"));

    EnemyCount = 0;
    if (PlayerSpline)
    {
        UFunction* StartMovementFunction = PlayerSpline->FindFunction(TEXT("StartMovement"));
        if (StartMovementFunction)
        {
            PlayerSpline->ProcessEvent(StartMovementFunction, nullptr);
            AVRPlayerCharacter* Player = Cast<AVRPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));
            Player->NextStage();
        }
    }
    // �^�C�}�[���N���A
    GetWorld()->GetTimerManager().ClearTimer(WaveTimerHandle);

    //TArray<AActor*> OverlappingActors;
    //if (PlayerActor)
    //{
    //    PlayerActor->GetOverlappingActors(OverlappingActors);

    //    for (AActor* Actor : OverlappingActors)
    //    {
    //        if (Actor)
    //        {
    //            Actor->Destroy();
    //        }
    //    }
    //}


}

AActor* AEnemySpawner::GetPlayerActor() const
{
    return GetWorld() ? GetWorld()->GetFirstPlayerController()->GetPawn() : nullptr;
}