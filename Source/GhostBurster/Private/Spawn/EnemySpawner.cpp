#include "Spawn/EnemySpawner.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogEnemySpawner, Log, All);

AEnemySpawner::AEnemySpawner()
{
    PrimaryActorTick.bCanEverTick = true;
    Tags.Add(FName("Spawner"));

    MaxStageNumber = 6 + 1;
}

void AEnemySpawner::BeginPlay()
{
    Super::BeginPlay();

    //スプラインの取得
    PlayerSpline = Cast<APlayerSplinePath>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerSplinePath::StaticClass()));
    //プレイヤーの取得
    Player = Cast<AVRPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

    //敵の数の初期化
    WaveEnemyCount.SetNum(MaxStageNumber);
    for (int i = 0; i < MaxStageNumber; ++i)
    {
        WaveEnemyCount[i] = 0;
    }

    // 旧スポーンデータ
    //FString FilePath = FPaths::ProjectContentDir() + TEXT("_TeamFolder/map/enemy_spawn_data.csv");
    
    // 新スポーンデータ
    FString SpawnDataPath = "";
    FString StageDataPath = "";

    //// エディター上のパス
    //if (GIsEditor)
    //{
    //    SpawnDataPath = FPaths::ProjectContentDir() + TEXT("GameData/EnemySpawnData.csv");
    //    StageDataPath = FPaths::ProjectContentDir() + TEXT("GameData/StageTimerData.csv");
    //}
    //else
    //{
    //    SpawnDataPath = FPaths::GameSourceDir() + TEXT("EnemySpawnData.csv");
    //    StageDataPath = FPaths::GameSourceDir() + TEXT("StageTimerData.csv");
    //}
    SpawnDataPath = FPaths::ProjectContentDir() + TEXT("GameData/EnemySpawnData.csv");
    StageDataPath = FPaths::ProjectContentDir() + TEXT("GameData/StageTimerData.csv");


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
            //行が空白 || 行の最初が「//」で始まっていたらスルー
            if (Line.IsEmpty() || Line.StartsWith("//"))
            {
                continue;
            }

            //行の最初が「」だったら、一つ前の敵のゴール経由地店を追加
            if (Line.StartsWith("AddLocation"))
            {
                //カンマ区切りで分割
                TArray<FString> Values;
                Line.ParseIntoArray(Values, TEXT(","), true);

                //敵の目標地点
                float GX = FCString::Atof(*Values[1]);
                float GY = FCString::Atof(*Values[2]);
                float GZ = FCString::Atof(*Values[3]);
                ParsedSpawnInfoArray[ParsedSpawnInfoArray.Num() - 1].GoalLocations.Add(FVector(GX, GY, GZ));

                continue;
            }
            else
            {
                //カンマ区切りで分割
                TArray<FString> Values;
                Line.ParseIntoArray(Values, TEXT(","), true);
                //データの変換
                FEnemySpawnInfo SpawnInfo;

                //出現するステージ番号
                SpawnInfo.Wave = FCString::Atoi(*Values[0]);
                WaveEnemyCount[SpawnInfo.Wave]++;

                //出現する敵の種類
                SpawnInfo.Type = Values[1];
                //出現する座標
                float SX = FCString::Atof(*Values[2]);
                float SY = FCString::Atof(*Values[3]);
                float SZ = FCString::Atof(*Values[4]);
                SpawnInfo.StartLocation = FVector(SX, SY, SZ);

                //敵の目標地点
                float GX = FCString::Atof(*Values[5]);
                float GY = FCString::Atof(*Values[6]);
                float GZ = FCString::Atof(*Values[7]);
                SpawnInfo.GoalLocations.Add(FVector(GX, GY, GZ));
                //敵の移動時間
                SpawnInfo.MoveTime = FCString::Atoi(*Values[8]);
                //敵の体力
                SpawnInfo.EnemyHP = FCString::Atoi(*Values[9]);
                //敵の移動が終わってから攻撃するまでの時間
                SpawnInfo.AttackTime = FCString::Atoi(*Values[10]);
                //生成にかかる時間
                SpawnInfo.DelayTime = FCString::Atof(*Values[11]);
                //最後の敵かどうか
                SpawnInfo.LastEnemy = FStringToBool(Values[12]);
                //以下、必要に応じて追加


                ParsedSpawnInfoArray.Add(SpawnInfo);
                //GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Yellow, FString::Printf(TEXT("Enemy Life -%d-"), SpawnInfo.EnemyHP));
            }
        }
    }

    ////デバッグ
    //for (int i = 1; i < MaxStageNumber; ++i)
    //{
    //    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, this->GetName());
    //    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, FString::Printf(TEXT("SpawnEnemyCount:%d -Stage %d- "), WaveEnemyCount[i], i));
    //}


    return ParsedSpawnInfoArray;
}
bool AEnemySpawner::FStringToBool(const FString& String) const
{
    if (String.Equals("TRUE", ESearchCase::IgnoreCase))
    {
        //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("Return True"));
        return true;
    }
    else if (String.Equals("FALSE", ESearchCase::IgnoreCase))
    {
        //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, TEXT("Return False"));
        return false;
    }

    return false;
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
            //行が空白 || 行の最初が「//」で始まっていたらスルー
            if (Line.IsEmpty() || Line.StartsWith("//"))
            {
                continue;
            }

            //カンマ区切りで分割
            TArray<FString> Values;
            Line.ParseIntoArray(Values, TEXT(","), true);

            ParsedStageTimer.Add(FCString::Atoi(*Values[0]));
            //GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Yellow, TEXT("Add Timer Data"));
        }
    }
    return ParsedStageTimer;
}

void AEnemySpawner::LogCurrentEnemyCount(const int32& Wave) const
{
    UE_LOG(LogEnemySpawner, Warning, TEXT("Current EnemyCount: %d"), WaveEnemyCount[Wave]);
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

    TArray<AActor*> BeforeEnemies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemys::StaticClass(), BeforeEnemies);
    for (int i = BeforeEnemies.Num() - 1; i >= 0; --i)
    {
        BeforeEnemies[i]->Destroy();
    }


    //遅延用タイマーハンドルのサイズ調整
    EnemyTimerHandles.SetNum(WaveEnemyCount[Wave]);
    int32 TimerIndex = 0;

    //敵の生成
    for (const FEnemySpawnInfo& SpawnInfo : SpawnInfoArray)
    {
        //同じウェーブ数なら
        if (SpawnInfo.Wave == Wave)
        {
            if (SpawnInfo.DelayTime != 0)
            {
                //タイマーをセット
                GetWorld()->GetTimerManager().SetTimer(
                    EnemyTimerHandles[TimerIndex],
                    [this, SpawnInfo]() {SpawnEnemy(SpawnInfo); },
                    SpawnInfo.DelayTime,
                    false
                );
                //インデックスを進める
                TimerIndex++;
            }
            else
            {
                SpawnEnemy(SpawnInfo);
            }
            LogCurrentEnemyCount(Wave);
            LogAttemptingToSpawn(SpawnInfo.Type, SpawnInfo.StartLocation);
        }
    }
}
void AEnemySpawner::SpawnEnemy(const FEnemySpawnInfo& SpawnInfo)
{
    TSubclassOf<AActor> EnemyClass = nullptr;

    if (SpawnInfo.Type == "White")
    {
        EnemyClass = WhiteEnemyClass;
    }
    else if (SpawnInfo.Type == "Green")
    {
        EnemyClass = GreenEnemyClass;
    }
    else if (SpawnInfo.Type == "Red")
    {
        EnemyClass = RedEnemyClass;
    }
    else if (SpawnInfo.Type == "Blue")
    {
        EnemyClass = BlueEnemyClass;
    }
    else if (SpawnInfo.Type == "Boss")
    {
        EnemyClass = BossEnemyClass;
    }

    if (EnemyClass)
    {
        UE_LOG(LogEnemySpawner, Warning, TEXT("Spawning enemy class: %s"), *EnemyClass->GetName());

        //敵の出現
        AActor* SpawnedEnemy = GetWorld()->SpawnActor<AActor>(EnemyClass, GetActorLocation() + SpawnInfo.StartLocation, FRotator::ZeroRotator);
        //出現した敵のステータス設定
        if (AEnemys* Enemy = Cast<AEnemys>(SpawnedEnemy))
        {
            Enemy->SetInitialData(SpawnInfo.EnemyHP, SpawnInfo.AttackTime, SpawnInfo.GoalLocations, SpawnInfo.MoveTime);
            //Enemy->SetHP(SpawnInfo.EnemyHP);
            //Enemy->SetGoalLocation(SpawnInfo.GoalLocation);
            //Enemy->SetMoveTime(SpawnInfo.MoveTime);
            //Enemy->SetAttackUpToTime(SpawnInfo.AttackTime);

            SpawnEnemies.Add(Enemy);
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
    // 最後の敵だったらステージタイマーの設定
    if (SpawnInfo.LastEnemy)
    {
        if (WaveTime.Num() != 0 && WaveTime[SpawnInfo.Wave - 1] != -1)
        {
            GetWorld()->GetTimerManager().SetTimer(WaveTimerHandle, this, &AEnemySpawner::HandleEnemyCountZero, WaveTime[SpawnInfo.Wave - 1], false);
            GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, FString::Printf(TEXT("Setting Timer : %d second"), WaveTime[SpawnInfo.Wave - 1]));
        }
    }
}

void AEnemySpawner::EnemyDeadFunction()
{
    UE_LOG(LogEnemySpawner, Warning, TEXT("EnemyDeadFunction called"));

    if (PlayerSpline->IsMoving() == false)
    {
        int32 Wave = Player->GetStageNumber();
        WaveEnemyCount[Wave]--;
        LogCurrentEnemyCount(Wave);

        if (WaveEnemyCount[Wave] == 0)
        {
            HandleEnemyCountZero();
        }
    }
}

void AEnemySpawner::HandleEnemyCountZero()
{
    //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("Call Handle -EnemyZero-"));

    if (PlayerSpline)
    {
        UFunction* StartMovementFunction = PlayerSpline->FindFunction(TEXT("StartMovement"));
        if (StartMovementFunction)
        {
            PlayerSpline->ProcessEvent(StartMovementFunction, nullptr);
            Player->NextStage();
        }
    }
    // タイマーをクリア
    GetWorld()->GetTimerManager().ClearTimer(WaveTimerHandle);

    AGameBGM* BGM = Cast<AGameBGM>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameBGM::StaticClass()));
    if (BGM)
    {
        BGM->ChangeBGM();
    }

}
