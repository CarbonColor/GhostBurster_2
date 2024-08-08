// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemys.h"
#include "Engine/World.h"
#include "Player/VRPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemys::AEnemys()
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
void AEnemys::UpdateState(State nowState)
{
	if (nowState != this->state)
	{
		this->state = nowState;
		this->MoveCount = 0;
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
		FString SpawnBPName = FString::Printf(TEXT("EnemySpawn_BP_C_%d"), Stage);
		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Purple, SpawnBPName);

		//該当のEnemySpawnを取得
		TArray<AActor*> Spawners;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Spawner"), Spawners);

		for (AActor* Spawner : Spawners)
		{
			GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Purple, Spawner->GetName());

			if (Spawner->GetName() == SpawnBPName)
			{
				if (UFunction* Func = Spawner->FindFunction(FName("EnemyDeadFunction")))
				{
					Spawner->ProcessEvent(Func, nullptr);
				}
			}
		}
	}

	//敵を消滅させる
	this->Destroy();

	return;
}

//現在のFPSを取得する
float AEnemys::GetWorldFPS()
{
	//ゲーム世界の情報を取得
	UWorld* World = GetWorld();

	//ゲーム世界の情報が取得できていなかったら抜ける
	if (!World)
	{
		return 60.f; //とりあえず60fpsとしておく
	}

	//DeltaTime取得
	float DeltaTime = World->GetDeltaSeconds();

	//現在のFPSを計算して取得
	float FPS = 1.f / DeltaTime;

	return FPS;
}

//HPの設定用関数
void AEnemys::SetHP(float HPValue)
{
	this->Status.HP = HPValue;
}

//攻撃までの時間設定用関数
void AEnemys::SetAttackUpToTime(int SetTime)
{
	this->AttackUpToTime = SetTime;
}

//目標座標の設定用関数
void AEnemys::SetGoalPosition(float SetX, float SetY, float SetZ)
{
	this->Position.GoalX = SetX;
	this->Position.GoalY = SetY;
	this->Position.GoalZ = SetZ;
}

//移動時間の設定用
void AEnemys::SetMoveTime(int SetTime)
{
	this->MoveTime = SetTime;
}

//生成されたときの設定用関数
void AEnemys::SetInitialData(float HP, int AttackUpToTimeValue, float GoalX, float GoalY, float GoalZ, int MoveTimeValue)
{
	this->SetHP(HP);
	this->SetAttackUpToTime(AttackUpToTimeValue);
	this->SetGoalPosition(GoalX, GoalY, GoalZ);
	this->SetMoveTime(MoveTimeValue);
}