// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/NormalEnemy.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ANormalEnemy::ANormalEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//☆SceneComponent
	//SceneComponentの作成
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	//SceneComponentをRootComponentに設定
	RootComponent = DefaultSceneRoot;

	//☆StaticMeshComponent
	//StaticMeshComponentの作成
	GhostMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ghost"));
	//StaticMeshをロードしてStaticMeshComponentのStaticMeshに設定する
	UStaticMesh* GMesh = LoadObject<UStaticMesh>(NULL, TEXT("/Engine/BasicShapes/Sphere"), NULL, LOAD_None, NULL);
	GhostMesh->SetStaticMesh(GMesh);
	//StaticMeshComponentをRootComponentにアタッチする
	GhostMesh->SetupAttachment(RootComponent);

	//☆マテリアル
	//マテリアルをロードしてGhostMeshに設定する
	UMaterial* Material = LoadObject<UMaterial>(NULL, TEXT("/Game/_TeamFolder/Enemy/White"), NULL, LOAD_None, NULL);
	GhostMesh->SetMaterial(0, Material);

	//☆コリジョン
	//スフィアコリジョンの作成
	GhostCollision = CreateDefaultSubobject<USphereComponent>(TEXT("GhostCollision"));
	//GhostCollisionをルートコンポーネントにアタッチする
	GhostCollision->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ANormalEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	//白い敵の設定
	this->Status.HP = 100;
	this->enemyColor = EnemyColor::White;
}

// Called every frame
void ANormalEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickProcess();
}

//☆追加関数
// Tickでの処理
void ANormalEnemy::TickProcess()
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
void ANormalEnemy::Think()
{
	State nowState = state;

	switch (nowState)
	{
	case State::Wait:	//待機
		if (MoveCount >= 60 * Gamefps / 60) { nowState = State::Move; }
		if (Status.HP <= 0) { nowState = State::Die; }
		break;

	case State::Move:	//動く
		if (this->bHasEndedMoving) { nowState = State::Attack; }
		if (Status.HP <= 0) { nowState = State::Die; }
		break;

	case State::Attack:	//攻撃
		if (MoveCount >= 60 * Gamefps / 60) { nowState = State::Wait; }
		if (Status.HP <= 0) { nowState = State::Die; }
		break;
	}

	UpdateState(nowState);
}

//状態に基づいた動きをする
void ANormalEnemy::ActProcess()
{
	switch (state)
	{
	case State::Wait:	//立っている		
		break;

	case State::Move:	//動く
		//状態Move遷移時にのみ行う処理
		if (MoveCount == 0)
		{
			ProcessJustForFirst_Move();
		}

		//移動処理(移動処理が終わったら状態遷移する)
		this->bHasEndedMoving = Move();
		break;

	case State::Attack:	//攻撃
		if (MoveCount == AttackUpToTime * Gamefps / 60) //15の部分は攻撃モーションに合わせて変更する
		{
			//攻撃する
			UKismetSystemLibrary::PrintString(this, TEXT("WhiteEnemy Attack!"), true, true, FColor::White, 2.f, TEXT("None"));
		}
		break;

	case State::Die:
		EnemyDead();
		break;
	}
}

//ダメージを受ける処理、引数でもらった攻撃力分体力を減らす
void ANormalEnemy::RecieveEnemyDamage(int DamageAmount, EFlashlight_Color Color)
{
	if ((int)Color == (int)this->enemyColor)
	{
		Status.HP -= DamageAmount;
	}
}

//状態Move遷移時にのみ行う処理
void ANormalEnemy::ProcessJustForFirst_Move()
{
	// 初期位置の設定
	CurrentLocation = GetActorLocation();

	// 方向ベクトルの計算
	Direction = (GoalLocation - CurrentLocation).GetSafeNormal();

	// 総移動距離の計算
	TotalDistance = FVector::Dist(CurrentLocation, GoalLocation);
}

//移動処理
bool ANormalEnemy::Move()
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