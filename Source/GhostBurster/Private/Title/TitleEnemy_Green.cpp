// Fill out your copyright notice in the Description page of Project Settings.


#include "Title/TitleEnemy_Green.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
ATitleEnemy_Green::ATitleEnemy_Green()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//���V�[���R���|�[�l���g-------------------------------------------------------------------------------------------------------
	//�V�[���R���|�[�l���g�̍쐬
	this->DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	if (this->DefaultSceneRoot)
	{
		//�V�[���R���|�[�l���g�����[�g�R���|�[�l���g�ɐݒ�
		RootComponent = this->DefaultSceneRoot;
		//��]�̐ݒ�
		this->DefaultSceneRoot->SetWorldRotation(RotationCorrectionValue);
		//�X�P�[���̐ݒ�
		this->DefaultSceneRoot->SetWorldScale3D(EnemyScale);

		//���X�P���^�����b�V���R���|�[�l���g---------------------------------------------------------------------------------------
		//�X�P���^�����b�V���R���|�[�l���g�̍쐬
		this->GhostMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Ghost"));
		//�X�P���^�����b�V�������[�h
		TObjectPtr<USkeletalMesh> GhostMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/_TeamFolder/CG/CG_Model/Ghost/SKM_TestGhost"));
		if (this->GhostMeshComponent)
		{
			if (GhostMesh)
			{
				//�X�P���^�����b�V���R���|�[�l���g�ɃX�P���^�����b�V����ݒ肷��
				GhostMeshComponent->SetSkeletalMesh(GhostMesh);
			}
			//�X�P���^�����b�V���R���|�[�l���g�����[�g�R���|�[�l���g�ɃA�^�b�`����
			this->GhostMeshComponent->SetupAttachment(RootComponent);
			//�X�P���^�����b�V���̃R���W�����𖳂���
			this->GhostMeshComponent->SetCollisionProfileName("NoCollision");
		}

		//���R���W����-------------------------------------------------------------------------------------------------------------
		//�X�t�B�A�R���W�����̍쐬
		this->GhostCollision = CreateDefaultSubobject<USphereComponent>(TEXT("GhostCollision"));
		if (this->GhostCollision)
		{
			//GhostCollision�����[�g�R���|�[�l���g�ɃA�^�b�`����
			this->GhostCollision->SetupAttachment(RootComponent);
			//GhostCollision�̃R���W�����v���Z�b�g��OverlapAllDynamic�ɂ���
			this->GhostCollision->SetCollisionProfileName("OverlapAllDynamic");
			//GhostCollision�̈ʒu�ݒ�
			this->GhostCollision->SetWorldLocation(FVector(0.f, 0.f, -50.f));
			//GhostCollision�̔��a�ݒ�
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

	//���}�e���A��
	//�̂̃}�e���A�������[�h
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


//�v���C���[�̃��C�g�̐F�ƓG�̃��C�g�̐F���`�F�b�N����֐�
bool ATitleEnemy_Green::CheckPlayerLightColor(EFlashlight_Color PlayerColor) const
{
	return PlayerColor == EFlashlight_Color::Green;
}
