// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/RedEnemy.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/VRPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

ARedEnemy::ARedEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	//☆シーンコンポーネント
	//シーンコンポーネントの作成
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	//シーンコンポーネントをルートコンポーネントに設定する
	RootComponent = DefaultSceneRoot;

	//☆スタティックメッシュ
	//スタティックメッシュの作成
	GhostMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GreenGhost"));
	//スタティックメッシュをロードしてGhostMeshにロードしたスタティックメッシュを設定する
	UStaticMesh* GreenGMesh = LoadObject<UStaticMesh>(NULL, TEXT("/Engine/BasicShapes/Sphere"), NULL, LOAD_None, NULL);
	GhostMesh->SetStaticMesh(GreenGMesh);
	//GhostMeshをルートコンポーネントにアタッチする
	GhostMesh->SetupAttachment(RootComponent);

	//☆マテリアル
	//マテリアルをロードしてGhostMeshに設定する
	UMaterial* Material = LoadObject<UMaterial>(NULL, TEXT("/Game/_Teamfolder/Enemy/Red"), NULL, LOAD_None, NULL);
	GhostMesh->SetMaterial(0, Material);

	//☆コリジョン
	//スフィアコリジョンの作成
	GhostCollision = CreateDefaultSubobject<USphereComponent>(TEXT("GhostCollision"));
	//GhostCollisionをルートコンポーネントにアタッチする
	GhostCollision->SetupAttachment(RootComponent);
}

void ARedEnemy::BeginPlay()
{
	Super::BeginPlay();

	//☆赤の敵の設定
	this->Status.HP = 100;
	this->enemyColor = EnemyColor::Red;
}

void ARedEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickProcess();
}

//☆追加関数
//Tickでの処理
void ARedEnemy::TickProcess()
{
	//現在のFPSを取得
	Gamefps = GetWorldFPS();

	MoveCount++;

	//エネミーの状態判断
	Think();
	//状態に基づいた動き
	ActProcess();
}

//エネミーの状態判断
void ARedEnemy::Think()
{
	State nowState = state;
	switch (nowState)
	{
	case State::Wait:	//待機
		if (MoveCount >= 60 * 3 * Gamefps / 60) { nowState = State::Attack; }	// 攻撃へ
		if (Status.HP <= 0) { nowState = State::Die; }							// 死亡へ
		break;

	case State::Move:	//移動
		if (this->bHasEndedMoving) { nowState = State::Wait; }	// 待機へ
		if (Status.HP <= 0) { nowState = State::Die; }			// 死亡へ
		break;

	case State::Attack:	//攻撃
		if (this->bHasEndedAttack) { nowState = State::Wait; }	// 待機へ
		if (Status.HP <= 0) { nowState = State::Die; }			// 死亡へ
		break;
	}

	UpdateState(nowState);
}

//状態に基づいた動きをする
void ARedEnemy::ActProcess()
{
	switch (state)
	{
	case State::Wait:	//待機		
		break;

	case State::Move:	//移動
		//状態Move遷移時にのみ行う処理
		if (this->bShouldBeenProcessWhenFirstStateTransition == false)
		{
			this->bShouldBeenProcessWhenFirstStateTransition = ProcessJustForFirst_Move();
		}

		//移動処理(移動処理が終わったら状態遷移する)
		this->bHasEndedMoving = Move();
		break;

	case State::Attack:	//攻撃
		//攻撃処理
		this->bHasEndedAttack = this->Attack();
		break;

	case State::Die:
		EnemyDead();
		break;
	}
}

//ダメージを受ける処理、引数でもらった攻撃力分体力を減らす
void ARedEnemy::RecieveEnemyDamage(int DamageAmount, EFlashlight_Color Color)
{
	if ((int)Color == (int)this->enemyColor)
	{
		Status.HP -= DamageAmount;
	}
}

//状態Move遷移時にのみ行う処理
bool ARedEnemy::ProcessJustForFirst_Move()
{
	// 初期位置の設定
	CurrentLocation = GetActorLocation();

	// 方向ベクトルの計算
	Direction = (GoalLocation - CurrentLocation).GetSafeNormal();

	// 総移動距離の計算
	TotalDistance = FVector::Dist(CurrentLocation, GoalLocation);

	// 目的地に着くまでの時間に合うように速度を計算
	Speed = TotalDistance / this->MoveTime;

	// 処理が終わったらtrueを返す
	return true;
}

//移動処理
bool ARedEnemy::Move()
{
	//DeltaTimeの取得
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	//目的地までの残り距離を計算
	float RemainingDistance = TotalDistance - TraveledDistance;

	//現在の速度での移動距離を計算
	float DeltaDistance = Speed * DeltaTime;

	//目的地に近づきすぎたら、残りの距離だけ進むように調整
	if (DeltaDistance >= RemainingDistance)
	{
		DeltaDistance = RemainingDistance;
		TraveledDistance = TotalDistance;
	}
	else
	{
		TraveledDistance += DeltaDistance;
	}

	//正弦波に基づいてオフセットを計算(目的地に瞬間移動して着かないように調整する計算)
	float Offset_Z = Amplitude * FMath::Sin(2.0f * PI * (TraveledDistance / TotalDistance));

	// 新しい位置を計算
	FVector NewLocation = CurrentLocation + (Direction * TraveledDistance);
	NewLocation.Z += Offset_Z;

	// 新しい位置に移動
	SetActorLocation(NewLocation);

	// 目的地に到達したら処理を終了
	if (TraveledDistance >= TotalDistance)
	{
		SetActorLocation(GoalLocation);

		//状態遷移できるようにする
		return true;
	}

	return false;
}

//攻撃処理
bool ARedEnemy::Attack()
{
	//攻撃判定
	if (MoveCount == (int)(AttackUpToTime * Gamefps / 60)) //15の部分は攻撃モーションに合わせて変更する
	{
		UKismetSystemLibrary::PrintString(this, TEXT("RedEnemy Attack!"), true, true, FColor::Red, 2.f, TEXT("None"));

		//プレイヤーへダメージを与える
		//プレイヤーの情報取得
		AVRPlayerCharacter* Player = Cast<AVRPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		if (Player)
		{
			Player->RecievePlayerDamage();
		}
	}
	//攻撃終了
	else if (MoveCount == (int)(TimeUpToAttackEnd * Gamefps / 60))
	{
		return true;
	}

	return false;
}