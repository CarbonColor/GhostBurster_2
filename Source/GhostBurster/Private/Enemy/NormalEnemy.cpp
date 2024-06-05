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

}

