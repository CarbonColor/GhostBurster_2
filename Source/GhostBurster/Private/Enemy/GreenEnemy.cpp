// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GreenEnemy.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/VRPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

AGreenEnemy::AGreenEnemy()
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

	//☆コリジョン
	//スフィアコリジョンの作成
	GhostCollision = CreateDefaultSubobject<USphereComponent>(TEXT("GhostCollision"));
	//GhostCollisionをルートコンポーネントにアタッチする
	GhostCollision->SetupAttachment(RootComponent);
}

void AGreenEnemy::BeginPlay()
{
	Super::BeginPlay();

	//☆緑の敵の設定
	this->Status.HP = 100;
	this->enemyColor = EnemyColor::Green;

	//☆マテリアル
	//マテリアルをロード
	UMaterial* Material = LoadObject<UMaterial>(NULL, TEXT("/Game/_TeamFolder/Enemy/Green"), NULL, LOAD_None, NULL);
	if (Material)
	{
		//ダイナミックマテリアルインスタンスを作成
		this->DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);

		//GhostMeshにダイナミックマテリアルを設定
		GhostMesh->SetMaterial(0, DynamicMaterial);

		//初期オパシティ値を設定
		this->DynamicMaterial->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);
	}
}

void AGreenEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickProcess();
}

//☆追加関数
//Tickでの処理
void AGreenEnemy::TickProcess()
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
void AGreenEnemy::Think()
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

	case State::Appear:	//出現
		if (this->bHasEndedAppear) { nowState = State::Move; }	// 移動へ
		break;
	}

	UpdateState(nowState);
}

//状態に基づいた動きをする
void AGreenEnemy::ActProcess()
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

	case State::Appear:	//出現
		//出現処理
		this->bHasEndedAppear = this->Appear();
		break;
	}
}

//ダメージを受ける処理、引数でもらった攻撃力分体力を減らす
void AGreenEnemy::RecieveEnemyDamage(int DamageAmount, EFlashlight_Color Color)
{
	if (this->state != State::Appear)
	{
		if ((int)Color == (int)this->enemyColor)
		{
			Status.HP -= DamageAmount;
		}
	}
}

//アイテムでダメージを受ける処理、引数でもらった数値分体力を減らす
void AGreenEnemy::RecieveItemDamage(int DamageAmount)
{
	Status.HP -= DamageAmount;
}

//状態Move遷移時にのみ行う処理
bool AGreenEnemy::ProcessJustForFirst_Move()
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
bool AGreenEnemy::Move()
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
bool AGreenEnemy::Attack()
{
	//攻撃判定
	if (MoveCount == (int)(AttackUpToTime * Gamefps / 60)) //15の部分は攻撃モーションに合わせて変更する
	{
		UKismetSystemLibrary::PrintString(this, TEXT("GreenEnemy Attack!"), true, true, FColor::Green, 2.f, TEXT("None"));

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

// 敵出現処理
bool AGreenEnemy::Appear()
{
	//DeltaTimeの取得
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	if (DynamicMaterial)
	{
		//オパシティの値を変更
		this->OpacityValue += 1.f / (float)TimeSpentInAppear * DeltaTime;

		//出現が終わったら処理を終了する
		if (this->OpacityValue >= 1.f)
		{
			//オパシティの値が1を超えないようにする
			this->OpacityValue = 1.f;

			//オパシティを設定
			this->DynamicMaterial->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);

			//状態遷移可能にする
			return true;
		}

		//オパシティを設定
		this->DynamicMaterial->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);
	}

	return false;
}