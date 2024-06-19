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

	//�G�l�~�[�̏�Ԕ��f
	Think();
	//��ԂɊ�Â�������
	Move();
}

//���ǉ��֐�
//�G�l�~�[�̏�Ԕ��f
void ANormalEnemy::Think()
{
	State nowState = state;
	switch (nowState)
	{
	case State::Stand:	//�����Ă���
		if (MoveCount >= 60 * 5 * Gamefps / 60) { nowState = State::Attack; }
		if (status.HP <= 0) { nowState = State::Die; }
		break;

	case State::Move:	//����
		if (MoveCount >= 60 * 5 * Gamefps / 60) { nowState = State::Attack; }
		if (status.HP <= 0) { nowState = State::Die; }
		break;

	case State::Attack:	//�U��
		if (MoveCount >= 30 * Gamefps / 60) { nowState = State::Stand; }
		if (status.HP <= 0) { nowState = State::Die; }
		break;
	}

	UpdateState(nowState);
}

//��Ԃ̍X�V
void ANormalEnemy::UpdateState(State nowState)
{
	if (nowState != state)
	{
		state = nowState;
		MoveCount = 0;
	}
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
		}
		break;

	case State::Die:
		EnemyDead();
		break;
	}
}

//HP��0�ɂȂ�������ł�����
void ANormalEnemy::EnemyDead()
{
		this->Destroy();
		//�C�x���g�ɑ΂��Ď��S�ʒm�𑗂�

		return;
}

//�_���[�W���󂯂鏈���A�����ł�������U���͕��̗͂����炷
void ANormalEnemy::Damage(float damage)
{
	status.HP -= damage;
}