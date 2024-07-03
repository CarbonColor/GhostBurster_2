// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GreenEnemy.h"
#include "Kismet/KismetSystemLibrary.h"

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

	//☆マテリアル
	//マテリアルをロードしてGhostMeshに設定する
	UMaterial* Material = LoadObject<UMaterial>(NULL, TEXT("/Game/_Teamfolder/Enemy/Green"), NULL, LOAD_None, NULL);
	GhostMesh->SetMaterial(0, Material);

	//☆コリジョン
	//スフィアコリジョンの作成
	GhostCollision = CreateDefaultSubobject<USphereComponent>(TEXT("GhostCollision"));
	//GhostCollisionをルートコンポーネントにアタッチする
	GhostCollision->SetupAttachment(RootComponent);
}

void AGreenEnemy::BeginPlay()
{
	Super::BeginPlay();

	//緑の敵の設定
	this->status.HP = 100;
	this->EnemyColor = EFlashlight_Color::Green;
}

void AGreenEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//現在のFPSを取得
	Gamefps = GetWorldFPS();

	MoveCount++;

	//エネミーの状態判断
	Think();
	//状態に基づいた動き
	Move();
}

//エネミーの状態判断
void AGreenEnemy::Think()
{
	State nowState = state;
	switch (nowState)
	{
	case State::Stand:	//立っている
		if (MoveCount >= 60 * 5 * Gamefps / 60) { nowState = State::Attack; }
		if (status.HP <= 0) { nowState = State::Die; }
		break;

	case State::Move:	//動く
		if (MoveCount >= 60 * 5 * Gamefps / 60) { nowState = State::Attack; }
		if (status.HP <= 0) { nowState = State::Die; }
		break;

	case State::Attack:	//攻撃
		if (MoveCount >= 30 * Gamefps / 60) { nowState = State::Stand; }
		if (status.HP <= 0) { nowState = State::Die; }
		break;
	}

	UpdateState(nowState);
}

//状態に基づいた動きをする
void AGreenEnemy::Move()
{
	switch (state)
	{
	case State::Stand:	//立っている		
		break;

	case State::Move:	//動く
		break;

	case State::Attack:	//攻撃
		if (MoveCount == 15 * Gamefps / 60) //15の部分は攻撃モーションに合わせて変更する
		{
			//攻撃する
			UKismetSystemLibrary::PrintString(this, TEXT("GreenEnemy Attack!"), true, true, FColor::Green, 2.f, TEXT("None"));
		}
		break;

	case State::Die:
		EnemyDead();
		break;
	}
}

//ダメージを受ける処理、引数でもらった攻撃力分体力を減らす
void AGreenEnemy::RecieveEnemyDamage(int DamageAmount)
{
	status.HP -= DamageAmount;
}