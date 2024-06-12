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

	//HPが0になったら消滅させる
	EnemyDead();

	//出現してから5秒たったら攻撃させる
	Attack();
}

//☆追加関数
//HPが0になったら消滅させる
void ANormalEnemy::EnemyDead()
{
	if (status.HP <= 0)
	{
		this->Destroy();
		//イベントに対して死亡通知を送る

		return;
	}
}

//敵の攻撃処理
void ANormalEnemy::Attack()
{
	if (MoveCount >= Gamefps * 5) //フレーム数が60固定でない場合、変更
	{
		MoveCount = 0;

		//攻撃する
	}
}

//ダメージを受ける処理、引数でもらった攻撃力分体力を減らす
void ANormalEnemy::Damage(float damage)
{
	status.HP -= damage;
}