// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BossEnemy.h"
#include "Kismet/KismetSystemLibrary.h"

ABossEnemy::ABossEnemy()
{
	//Tick��L���ɂ���
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
	UMaterial* Material = LoadObject<UMaterial>(NULL, TEXT("/Game/_Teamfolder/Enemy/Boss"), NULL, LOAD_None, NULL);
	GhostMesh->SetMaterial(0, Material);

	//���R���W����
	//�X�t�B�A�R���W�����̍쐬
	GhostCollision = CreateDefaultSubobject<USphereComponent>(TEXT("BossCollision"));
	//GhostCollision�����[�g�R���|�[�l���g�ɃA�^�b�`����
	GhostCollision->SetupAttachment(RootComponent);

}

void ABossEnemy::BeginPlay()
{
	Super::BeginPlay();

	//�΂̓G�̐ݒ�
	this->Status.HP = 300;
	this->enemyColor = EnemyColor::White;
}

void ABossEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickProcess();
}

//���ǉ��֐�
//Tick�ł̏���
void ABossEnemy::TickProcess()
{
	//���݂�FPS���擾
	Gamefps = GetWorldFPS();

	MoveCount++;

	//�G�l�~�[�̏�Ԕ��f
	Think();
	//��ԂɊ�Â�������
	ActProcess();
}

//�G�l�~�[�̏�Ԕ��f
void ABossEnemy::Think()
{
	State nowState = state;
	switch (nowState)
	{
	case State::Wait:	//�����Ă���
		if (MoveCount >= 60 * Gamefps / 60) { nowState = State::Attack; }
		if (Status.HP <= 0) { nowState = State::Die; }
		break;

	case State::Move:	//����
		if (MoveCount >= 60 * 5 * Gamefps / 60) { nowState = State::Attack; }
		if (Status.HP <= 0) { nowState = State::Die; }
		break;

	case State::Attack:	//�U��
		if (MoveCount >= 60 * Gamefps / 60) { nowState = State::Wait; }
		if (Status.HP <= 0) { nowState = State::Die; }
		break;
	}

	UpdateState(nowState);
}

//��ԂɊ�Â�������������
void ABossEnemy::ActProcess()
{
	switch (state)
	{
	case State::Wait:	//�����Ă���		
		break;

	case State::Move:	//����
		break;

	case State::Attack:	//�U��
		if (MoveCount == AttackUpToTime * Gamefps / 60) //15�̕����͍U�����[�V�����ɍ��킹�ĕύX����
		{
			//�U������
			UKismetSystemLibrary::PrintString(this, TEXT("BossEnemy Attack!"), true, true, FColor::Yellow, 2.f, TEXT("None"));
		}
		break;

	case State::Die:
		EnemyDead();
		break;
	}
}

//�_���[�W���󂯂鏈���A�����ł�������U���͕��̗͂����炷
void ABossEnemy::RecieveEnemyDamage(int DamageAmount, EFlashlight_Color Color)
{
	//���C�g�̐F�ƓG�̐F����v�����Ƃ������_���[�W���󂯂�
	if ((int)Color == (int)this->enemyColor)
	{
		Status.HP -= DamageAmount;
	}
}
