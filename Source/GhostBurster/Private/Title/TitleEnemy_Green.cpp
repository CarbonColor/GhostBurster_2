// Fill out your copyright notice in the Description page of Project Settings.


#include "Title/TitleEnemy_Green.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
ATitleEnemy_Green::ATitleEnemy_Green()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//☆シーンコンポーネント-------------------------------------------------------------------------------------------------------
	//シーンコンポーネントの作成
	this->DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	if (this->DefaultSceneRoot)
	{
		//シーンコンポーネントをルートコンポーネントに設定
		RootComponent = this->DefaultSceneRoot;
		//回転の設定
		this->DefaultSceneRoot->SetWorldRotation(RotationCorrectionValue);
		//スケールの設定
		this->DefaultSceneRoot->SetWorldScale3D(EnemyScale);

		//☆スケルタルメッシュコンポーネント---------------------------------------------------------------------------------------
		//スケルタルメッシュコンポーネントの作成
		this->GhostMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Ghost"));
		//スケルタルメッシュをロード
		TObjectPtr<USkeletalMesh> GhostMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/_TeamFolder/CG/CG_Model/Ghost/SKM_TestGhost"));
		if (this->GhostMeshComponent)
		{
			if (GhostMesh)
			{
				//スケルタルメッシュコンポーネントにスケルタルメッシュを設定する
				GhostMeshComponent->SetSkeletalMesh(GhostMesh);
			}
			//スケルタルメッシュコンポーネントをルートコンポーネントにアタッチする
			this->GhostMeshComponent->SetupAttachment(RootComponent);
			//スケルタルメッシュのコリジョンを無くす
			this->GhostMeshComponent->SetCollisionProfileName("NoCollision");
		}

		//☆コリジョン-------------------------------------------------------------------------------------------------------------
		//スフィアコリジョンの作成
		this->GhostCollision = CreateDefaultSubobject<USphereComponent>(TEXT("GhostCollision"));
		if (this->GhostCollision)
		{
			//GhostCollisionをルートコンポーネントにアタッチする
			this->GhostCollision->SetupAttachment(RootComponent);
			//GhostCollisionのコリジョンプリセットをOverlapAllDynamicにする
			this->GhostCollision->SetCollisionProfileName("OverlapAllDynamic");
			//GhostCollisionの位置設定
			this->GhostCollision->SetWorldLocation(FVector(0.f, 0.f, -50.f));
			//GhostCollisionの半径設定
			this->GhostCollision->SetSphereRadius(80.f);
		}
	}

}

// Called when the game starts or when spawned
void ATitleEnemy_Green::BeginPlay()
{
	Super::BeginPlay();

	//HP = 60 * 3;
	//RunningTime = 0;
	//InitialLocation = GetActorLocation();

	//☆マテリアル
	//体のマテリアルをロード
	TObjectPtr<UMaterial> BodyMaterial = LoadObject<UMaterial>(nullptr, TEXT("/Game/_TeamFolder/Title/Material/M_Green"));
	if (BodyMaterial && this->GhostMeshComponent)
	{
		GhostMeshComponent->SetMaterial(0, BodyMaterial);
	}

}

// Called every frame
void ATitleEnemy_Green::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	float DeltaHeight = 10.0f * FMath::Sin(1.0f * RunningTime);

	FVector NewLocation = InitialLocation;
	NewLocation.Z += DeltaHeight;

	SetActorLocation(NewLocation);

}

void ATitleEnemy_Green::RecieveEnemyDamage(int DamageAmount)
{
	HP -= DamageAmount;
	//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, FString::Printf(TEXT("Title_Green is Damage (%d)"), HP));
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
