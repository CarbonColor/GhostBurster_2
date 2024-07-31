// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/NormalEnemy.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ANormalEnemy::ANormalEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//��SceneComponent
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

	//���}�e���A��
	//�}�e���A�������[�h����GhostMesh�ɐݒ肷��
	UMaterial* Material = LoadObject<UMaterial>(NULL, TEXT("/Game/_TeamFolder/Enemy/White"), NULL, LOAD_None, NULL);
	GhostMesh->SetMaterial(0, Material);

	//���R���W����
	//�X�t�B�A�R���W�����̍쐬
	GhostCollision = CreateDefaultSubobject<USphereComponent>(TEXT("GhostCollision"));
	//GhostCollision�����[�g�R���|�[�l���g�ɃA�^�b�`����
	GhostCollision->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ANormalEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	//�����G�̐ݒ�
	this->Status.HP = 100;
	this->enemyColor = EnemyColor::White;
}

// Called every frame
void ANormalEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickProcess();
}

//���ǉ��֐�
// Tick�ł̏���
void ANormalEnemy::TickProcess()
{
	//���݂�FPS���擾
	Gamefps = GetWorldFPS();

	MoveCount++;

	//�G�l�~�[�̏�Ԕ��f
	Think();
	//��ԂɊ�Â�������
	Move();
}

//�G�l�~�[�̏�Ԕ��f
void ANormalEnemy::Think()
{
	State nowState = state;

	switch (nowState)
	{
	case State::Stand:	//�����Ă���
		if (MoveCount >= 60 * 5 * Gamefps / 60) { nowState = State::Attack; }
		if (Status.HP <= 0) { nowState = State::Die; }
		break;

	case State::Move:	//����
		if (MoveCount >= 60 * 5 * Gamefps / 60) { nowState = State::Attack; }
		if (Status.HP <= 0) { nowState = State::Die; }
		break;

	case State::Attack:	//�U��
		if (MoveCount >= AttackUpToTime * Gamefps / 60) { nowState = State::Stand; }
		if (Status.HP <= 0) { nowState = State::Die; }
		break;
	}

	UpdateState(nowState);
}

//��ԂɊ�Â�������������
void ANormalEnemy::Move()
{
	switch (state)
	{
	case State::Stand:	//�����Ă���		
		break;

	case State::Move:	//����
		break;

	case State::Attack:	//�U��
		if (MoveCount == 15 * Gamefps / 60) //15�̕����͍U�����[�V�����ɍ��킹�ĕύX����
		{
			//�U������
			UKismetSystemLibrary::PrintString(this, TEXT("WhiteEnemy Attack!"), true, true, FColor::White, 2.f, TEXT("None"));
		}
		break;

	case State::Die:
		EnemyDead();
		break;
	}
}

//�_���[�W���󂯂鏈���A�����ł�������U���͕��̗͂����炷
void ANormalEnemy::RecieveEnemyDamage(int DamageAmount, EFlashlight_Color Color)
{
	if ((int)Color == (int)this->enemyColor)
	{
		Status.HP -= DamageAmount;
	}
}