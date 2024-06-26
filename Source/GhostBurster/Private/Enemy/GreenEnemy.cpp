// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GreenEnemy.h"
#include "Kismet/KismetSystemLibrary.h"

AGreenEnemy::AGreenEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	//���V�[���R���|�[�l���g
	//�V�[���R���|�[�l���g�̍쐬
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	//�V�[���R���|�[�l���g�����[�g�R���|�[�l���g�ɐݒ肷��
	RootComponent = DefaultSceneRoot;

	//���X�^�e�B�b�N���b�V��
	//�X�^�e�B�b�N���b�V���̍쐬
	GhostMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GreenGhost"));
	//�X�^�e�B�b�N���b�V�������[�h����GhostMesh�Ƀ��[�h�����X�^�e�B�b�N���b�V����ݒ肷��
	UStaticMesh* GreenGMesh = LoadObject<UStaticMesh>(NULL, TEXT("/Engine/BasicShapes/Sphere"), NULL, LOAD_None, NULL);
	GhostMesh->SetStaticMesh(GreenGMesh);
	//GhostMesh�����[�g�R���|�[�l���g�ɃA�^�b�`����
	GhostMesh->SetupAttachment(RootComponent);

	//���}�e���A��
	//�}�e���A�������[�h����GhostMesh�ɐݒ肷��
	UMaterial* Material = LoadObject<UMaterial>(NULL, TEXT("/Game/_Teamfolder/Enemy/Green"), NULL, LOAD_None, NULL);
	GhostMesh->SetMaterial(0, Material);

	//���R���W����
	//�X�t�B�A�R���W�����̍쐬
	GhostCollision = CreateDefaultSubobject<USphereComponent>(TEXT("GhostCollision"));
	//GhostCollision�����[�g�R���|�[�l���g�ɃA�^�b�`����
	GhostCollision->SetupAttachment(RootComponent);
}

void AGreenEnemy::BeginPlay()
{
	Super::BeginPlay();

	//�΂̓G�̐ݒ�
	this->status.HP = 100;
	this->EColor = EnemyColor::Green;
}

void AGreenEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//���݂�FPS���擾
	Gamefps = GetWorldFPS();

	MoveCount++;

	//�G�l�~�[�̏�Ԕ��f
	Think();
	//��ԂɊ�Â�������
	Move();
}

//�G�l�~�[�̏�Ԕ��f
void AGreenEnemy::Think()
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

//��ԂɊ�Â�������������
void AGreenEnemy::Move()
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
			UKismetSystemLibrary::PrintString(this, TEXT("GreenEnemy Attack!"), true, true, FColor::Green, 2.f, TEXT("None"));
		}
		break;

	case State::Die:
		EnemyDead();
		break;
	}
}

//�_���[�W���󂯂鏈���A�����ł�������U���͕��̗͂����炷
void AGreenEnemy::Damage(float damage)
{
	status.HP -= damage;
}