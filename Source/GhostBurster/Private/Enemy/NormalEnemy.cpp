// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/NormalEnemy.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/VRPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

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
	//スタティックメッシュのコリジョンを無くす
	GhostMesh->SetCollisionProfileName("NoCollision");

	//☆コリジョン
	//スフィアコリジョンの作成
	GhostCollision = CreateDefaultSubobject<USphereComponent>(TEXT("GhostCollision"));
	//GhostCollisionをルートコンポーネントにアタッチする
	GhostCollision->SetupAttachment(RootComponent);
	//GhostCollisionのコリジョンプリセットをOverlapAllDynamicにする
	GhostCollision->SetCollisionProfileName("OverlapAllDynamic");
}

// Called when the game starts or when spawned
void ANormalEnemy::BeginPlay()
{
	Super::BeginPlay();

	//☆白い敵の設定
	this->enemyColor = EnemyColor::White;

	//☆マテリアル
	//マテリアルをロード
	UMaterial* Material = LoadObject<UMaterial>(NULL, TEXT("/Game/_TeamFolder/Enemy/White"), NULL, LOAD_None, NULL);
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
		if (MoveCount >= 1 * Gamefps) { nowState = State::Attack; }	// 攻撃へ
		if (Status.HP <= 0) { nowState = State::Die; }				// 死亡へ
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
void ANormalEnemy::ActProcess()
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
		//攻撃処理(攻撃が終わった後状態遷移する)
		this->bHasEndedAttack = this->Attack();
		break;

	case State::Die:	//死亡
		EnemyDead();
		break;

	case State::Appear:	//出現
		//出現処理
		this->bHasEndedAppear = this->Appear();
		break;
	}
}

//ダメージを受ける処理、引数でもらった数値分体力を減らす
void ANormalEnemy::RecieveEnemyDamage(int DamageAmount, EFlashlight_Color Color)
{
	if (this->state != State::Appear && this->state != State::Die)
	{
		if ((int)Color == (int)this->enemyColor)
		{
			Status.HP -= DamageAmount;
		}
	}
}

//アイテムでダメージを受ける処理、引数でもらった数値分体力を減らす
void ANormalEnemy::RecieveItemDamage(int DamageAmount)
{
	Status.HP -= DamageAmount;
}

//状態Move遷移時にのみ行う処理
bool ANormalEnemy::ProcessJustForFirst_Move()
{
	//ゼロクリアする
	this->TraveledDistance = 0.f;

	// 初期位置の設定
	this->CurrentLocation = GetActorLocation();

	// 目標座標の設定
	/*もし移動が複数回行われるようになった時、外部ファイルから読み込んで設定*/

	// ワールド座標への変換
	this->GoalLocation_World = this->CurrentLocation + this->GoalLocation;

	// 方向ベクトルの計算
	this->Direction = (this->GoalLocation_World - this->CurrentLocation).GetSafeNormal();

	// 総移動距離の計算
	this->TotalDistance = FVector::Dist(this->CurrentLocation, this->GoalLocation_World);

	// 目的地に着くまでの時間に合うように速度を計算
	this->Speed = this->TotalDistance / this->MoveTime;

	// 処理が終わったらtrueを返す
	return true;
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
		SetActorLocation(this->GoalLocation_World);

		//状態遷移できるようにする
		return true;
	}

	return false;
}

//攻撃処理
bool ANormalEnemy::Attack()
{
	//攻撃判定
	if (MoveCount >= AttackUpToTime * Gamefps)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("WhiteEnemy Attack!"), true, true, FColor::White, 2.f, TEXT("None"));

		//プレイヤーへダメージを与える
		//プレイヤーの情報取得
		AVRPlayerCharacter* Player = Cast<AVRPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		if (Player)
		{
			Player->RecievePlayerDamage();
		}

		//攻撃終了(条件式で制御し、アニメーションが終わったらにするかも)
		return true;
	}

	return false;
}

// 敵出現処理
bool ANormalEnemy::Appear()
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

//メモ
/*
Move後の敵の行動を外部ファイルで制御したい
・外部ファイルの内容をセットする関数の追加
・Move後の状態遷移処理を外部ファイルの内容で決まるようにする

switch(hoge)
{
	case 0:
		State StateHoge1 = State::Wait;
}
みたいな感じ
*/