// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemys.h"
#include "Engine/World.h"
#include "Player/VRPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemys::AEnemys()
	:
	MoveCount(0),
	Gamefps(60.f),
	bShouldBeenProcessWhenFirstStateTransition(false),
	//構造体
	Status(),
	//列挙型
	State(EState::Appear), EnemyColor(EEnemyColor::White),
	//コンポーネント関係
	DefaultSceneRoot(nullptr), GhostMesh(nullptr), GhostCollision(nullptr), DynamicMaterial(nullptr), SplineComponent(nullptr),
	//サウンド関係
	AppearSound(nullptr), DisappearSound(nullptr),
	//移動関係
	/*CurrentLocation(FVector(0.f, 0.f, 0.f)), GoalLocation(FVector(0.f, 0.f, 0.f)), GoalLocation_World(FVector(0.f, 0.f, 0.f)), bHasEndedMoving(false), Direction(FVector(0.f, 0.f, 0.f)), TotalDistance(0.f), Amplitude(40.f), Frequency(1.f),*/
	MoveTime(1.f), TraveledDistance(0.f), SplineLength(0.f), CountGotInMoveState(0), GoalLocations(), bHasEndedMoving(false),
	//攻撃関係
	bHasEndedAttack(false), AttackUpToTime(0.f),
	//出現関係
	bHasEndedAppear(false), OpacityValue(0.f), TimeSpentInAppear(1)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemys::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEnemys::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//状態の更新
void AEnemys::UpdateState(EState NowState)
{
	if (NowState != this->State)
	{
		this->State = NowState;
		this->MoveCount = 0;
		this->bShouldBeenProcessWhenFirstStateTransition = false;
	}
}

//HPが0になったら消滅させる
void AEnemys::EnemyDead()
{
	//イベントに死亡通知を送る
	// プレイヤーを取得
	AVRPlayerCharacter* Player = Cast<AVRPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (Player)
	{
		//ステージ名を取得
		int Stage = Player->GetStageNumber();
		FString SpawnBPName = FString::Printf(TEXT("EnemysSpawn_BP_C_%d"), Stage + 2);
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Silver, SpawnBPName);

		//該当のEnemySpawnを取得
		TArray<AActor*> Spawners;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Spawner"), Spawners);

		for (AActor* Spawner : Spawners)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, Spawner->GetName());

			if (Spawner->GetName() == SpawnBPName)
			{
				if (UFunction* Func = Spawner->FindFunction(FName("EnemyDeadFunction")))
				{
					Spawner->ProcessEvent(Func, nullptr);
					Player->AddScore(100);
				}
			}
		}
	}

	//敵消滅時の音を鳴らす
	PlayDisappearSound();

	//敵を消滅させる
	this->Destroy();

	return;
}

//現在のFPSを取得する
float AEnemys::GetWorldFPS()
{
	//DeltaTime取得
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	//現在のFPSを計算して取得
	float FPS = 1.f / DeltaTime;

	return FPS;
}

//サウンド関数-----------------------------------------------------------------------------------------------------------------
//敵出現時の音を鳴らす
void AEnemys::PlayAppearSound()
{
	if (this->AppearSound) // 設定されていなかったら鳴らないようにする
	{
		UGameplayStatics::PlaySoundAtLocation(this, AppearSound, GetActorLocation());
	}
}

//敵消滅時の音を鳴らす
void AEnemys::PlayDisappearSound()
{
	if (DisappearSound) // 設定されていなかったら鳴らないようにする
	{
		UGameplayStatics::PlaySoundAtLocation(this, DisappearSound, GetActorLocation());
	}
}

//Setter関数-------------------------------------------------------------------------------------------------------------------
//HPの設定用関数
void AEnemys::SetHP(const int HPValue)
{
	this->Status.HP = HPValue;
}

//攻撃までの時間設定用関数
void AEnemys::SetAttackUpToTime(const float SetTime)
{
	this->AttackUpToTime = SetTime;
}

//目標座標の設定用関数
void AEnemys::SetGoalLocations(const TArray<FVector>& SetLocations)
{
	this->GoalLocations = SetLocations;
}

//移動時間の設定用
void AEnemys::SetMoveTime(const float SetTime)
{
	this->MoveTime = SetTime;
}

//生成されたときの設定用関数
void AEnemys::SetInitialData(const int HP, const float AttackUpToTimeValue, const TArray<FVector>& SetLocations, const float MoveTimeValue)
{
	this->SetHP(HP);
	this->SetAttackUpToTime(AttackUpToTimeValue);
	this->SetGoalLocations(SetLocations);
	this->SetMoveTime(MoveTimeValue);
}