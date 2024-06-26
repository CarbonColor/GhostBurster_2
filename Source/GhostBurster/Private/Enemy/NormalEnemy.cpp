// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/NormalEnemy.h"

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
	
}

// Called every frame
void ANormalEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveCount++;

	//エネミーの状態判断
	Think();
	//状態に基づいた動き
	Move();
}

//☆追加関数
//エネミーの状態判断
void ANormalEnemy::Think()
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
void ANormalEnemy::Move()
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
		}
		break;

	case State::Die:
		EnemyDead();
		break;
	}
}

//ダメージを受ける処理、引数でもらった攻撃力分体力を減らす
void ANormalEnemy::Damage(float damage)
{
	status.HP -= damage;
}