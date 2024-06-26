// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemys.h"
#include "Engine/World.h"

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
	this->Destroy();
	//イベントに死亡通知を送る

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