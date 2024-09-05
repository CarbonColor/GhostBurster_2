// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BossEnemy.h"
#include "Kismet/KismetSystemLibrary.h"

ABossEnemy::ABossEnemy()
{
	//Tickを有効にする
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
	//スタティックメッシュのコリジョンを無くす
	GhostMesh->SetCollisionProfileName("NoCollision");

	//☆マテリアル
	//マテリアルをロードしてGhostMeshに設定する
	UMaterial* Material = LoadObject<UMaterial>(NULL, TEXT("/Game/_Teamfolder/Enemy/Boss"), NULL, LOAD_None, NULL);
	GhostMesh->SetMaterial(0, Material);

	//☆コリジョン
	//スフィアコリジョンの作成
	GhostCollision = CreateDefaultSubobject<USphereComponent>(TEXT("BossCollision"));
	//GhostCollisionをルートコンポーネントにアタッチする
	GhostCollision->SetupAttachment(RootComponent);
	//GhostCollisionのコリジョンプリセットをOverlapAllDynamicにする
	GhostCollision->SetCollisionProfileName("OverlapAllDynamic");
}

void ABossEnemy::BeginPlay()
{
	Super::BeginPlay();

	//ボス敵の設定
	this->Status.HP = 300;
	this->EnemyColor = EEnemyColor::White;
}

void ABossEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickProcess();
}

//☆追加関数
//Tickでの処理
void ABossEnemy::TickProcess()
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
void ABossEnemy::Think()
{
	EState NowState = this->State;
	switch (NowState)
	{
	case EState::Wait:	//立っている
		if (MoveCount >= 60 * Gamefps / 60) { NowState = EState::Attack; }
		if (Status.HP <= 0) { NowState = EState::Die; }
		break;

	case EState::Move:	//動く
		if (MoveCount >= 60 * 5 * Gamefps / 60) { NowState = EState::Attack; }
		if (Status.HP <= 0) { NowState = EState::Die; }
		break;

	case EState::Attack:	//攻撃
		if (MoveCount >= 60 * Gamefps / 60) { NowState = EState::Wait; }
		if (Status.HP <= 0) { NowState = EState::Die; }
		break;
	}

	UpdateState(NowState);
}

//状態に基づいた動きをする
void ABossEnemy::ActProcess()
{
	switch (this->State)
	{
	case EState::Wait:	//立っている		
		break;

	case EState::Move:	//動く
		break;

	case EState::Attack:	//攻撃
		if (MoveCount == AttackUpToTime * Gamefps / 60) //15の部分は攻撃モーションに合わせて変更する
		{
			//攻撃する
			UKismetSystemLibrary::PrintString(this, TEXT("BossEnemy Attack!"), true, true, FColor::Yellow, 2.f, TEXT("None"));
		}
		break;

	case EState::Die:
		EnemyDead();
		break;
	}
}

//ダメージを受ける処理、引数でもらった攻撃力分体力を減らす
void ABossEnemy::RecieveEnemyDamage(int DamageAmount)
{
	//ライトの色と敵の色が一致したときだけダメージを受ける
	Status.HP -= DamageAmount;
}

//プレイヤーのライトの色と敵のライトの色をチェックする関数
bool ABossEnemy::CheckPlayerLightColor(EFlashlight_Color PlayerColor) const
{
	return (int)PlayerColor == (int)this->EnemyColor;
}