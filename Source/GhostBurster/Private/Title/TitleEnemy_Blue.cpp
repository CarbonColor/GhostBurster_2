// Fill out your copyright notice in the Description page of Project Settings.


#include "Title/TitleEnemy_Blue.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
ATitleEnemy_Blue::ATitleEnemy_Blue()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//SceneComponent�̍쐬
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	//SceneComponent��RootComponent�ɐݒ�
	RootComponent = DefaultSceneRoot;

	//��StaticMeshComponent
	//StaticMeshComponent�̍쐬
	GhostMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ghost"));
	//StaticMesh�����[�h����StaticMeshComponent��StaticMesh�ɐݒ肷��
	UStaticMesh* GMesh = LoadObject<UStaticMesh>(NULL, TEXT("/Engine/BasicShapes/Sphere"), NULL, LOAD_None, NULL);
	GhostMesh->SetStaticMesh(GMesh);
	//StaticMeshComponent��RootComponent�ɃA�^�b�`����
	GhostMesh->SetupAttachment(RootComponent);
	GhostMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));


	//���R���W����
	//�X�t�B�A�R���W�����̍쐬
	GhostCollision = CreateDefaultSubobject<USphereComponent>(TEXT("GhostCollision"));
	//GhostCollision�����[�g�R���|�[�l���g�ɃA�^�b�`����
	GhostCollision->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ATitleEnemy_Blue::BeginPlay()
{
	Super::BeginPlay();

	HP = 60 * 3;
	RunningTime = 0;
	InitialLocation = GetActorLocation();

}

// Called every frame
void ATitleEnemy_Blue::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	float DeltaHeight = 20.0f * FMath::Sin(1.0f * RunningTime);

	FVector NewLocation = InitialLocation;
	NewLocation.Z += DeltaHeight;

	SetActorLocation(NewLocation);

}

void ATitleEnemy_Blue::RecieveEnemyDamage(int DamageAmount, EFlashlight_Color Color)
{
	if (Color == EFlashlight_Color::Blue)
	{
		HP -= DamageAmount;
		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, FString::Printf(TEXT("Title_Blue is Damage (%d)"), HP));
		if (HP <= 0)
		{
			EnemyDead();
		}
	}
}

void ATitleEnemy_Blue::RecieveItemDamage(int DamageAmount)
{
	HP -= DamageAmount;
}