// Fill out your copyright notice in the Description page of Project Settings.


#include "Title/TitleEnemy_Green.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
ATitleEnemy_Green::ATitleEnemy_Green()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
	GhostMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	GhostMesh->SetCollisionProfileName("NoCollision");

	//☆コリジョン
	//スフィアコリジョンの作成
	GhostCollision = CreateDefaultSubobject<USphereComponent>(TEXT("GhostCollision"));
	//GhostCollisionをルートコンポーネントにアタッチする
	GhostCollision->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ATitleEnemy_Green::BeginPlay()
{
	Super::BeginPlay();

	HP = 60 * 3;
	RunningTime = 0;
	InitialLocation = GetActorLocation();
}

// Called every frame
void ATitleEnemy_Green::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	float DeltaHeight = 20.0f * FMath::Sin(1.0f * RunningTime);

	FVector NewLocation = InitialLocation;
	NewLocation.Z += DeltaHeight;

	SetActorLocation(NewLocation);

}

void ATitleEnemy_Green::RecieveEnemyDamage(int DamageAmount)
{
	HP -= DamageAmount;
	GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, FString::Printf(TEXT("Title_Green is Damage (%d)"), HP));
	if (HP <= 0)
	{
		EnemyDead();
	}
}


//プレイヤーのライトの色と敵のライトの色をチェックする関数
bool ATitleEnemy_Green::CheckPlayerLightColor(EFlashlight_Color PlayerColor) const
{
	return PlayerColor == EFlashlight_Color::Green;
}
