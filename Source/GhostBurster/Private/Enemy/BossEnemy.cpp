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
	//�X�^�e�B�b�N���b�V���̃R���W�����𖳂���
	GhostMesh->SetCollisionProfileName("NoCollision");

	//���}�e���A��
	//�}�e���A�������[�h����GhostMesh�ɐݒ肷��
	UMaterial* Material = LoadObject<UMaterial>(NULL, TEXT("/Game/_Teamfolder/Enemy/Boss"), NULL, LOAD_None, NULL);
	GhostMesh->SetMaterial(0, Material);

	//���R���W����
	//�X�t�B�A�R���W�����̍쐬
	GhostCollision = CreateDefaultSubobject<USphereComponent>(TEXT("BossCollision"));
	//GhostCollision�����[�g�R���|�[�l���g�ɃA�^�b�`����
	GhostCollision->SetupAttachment(RootComponent);
	//GhostCollision�̃R���W�����v���Z�b�g��OverlapAllDynamic�ɂ���
	GhostCollision->SetCollisionProfileName("OverlapAllDynamic");
}

void ABossEnemy::BeginPlay()
{
	Super::BeginPlay();

	//�{�X�G�̐ݒ�
	this->Status.HP = 300;
	this->EnemyColor = EEnemyColor::White;
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
	EState NowState = this->State;
	switch (NowState)
	{
	case EState::Wait:	//�����Ă���
		if (MoveCount >= 60 * Gamefps / 60) { NowState = EState::Attack; }
		if (Status.HP <= 0) { NowState = EState::Die; }
		break;

	case EState::Move:	//����
		if (MoveCount >= 60 * 5 * Gamefps / 60) { NowState = EState::Attack; }
		if (Status.HP <= 0) { NowState = EState::Die; }
		break;

	case EState::Attack:	//�U��
		if (MoveCount >= 60 * Gamefps / 60) { NowState = EState::Wait; }
		if (Status.HP <= 0) { NowState = EState::Die; }
		break;
	}

	UpdateState(NowState);
}

//��ԂɊ�Â�������������
void ABossEnemy::ActProcess()
{
	switch (this->State)
	{
	case EState::Wait:	//�����Ă���		
		break;

	case EState::Move:	//����
		break;

	case EState::Attack:	//�U��
		if (MoveCount == AttackUpToTime * Gamefps / 60) //15�̕����͍U�����[�V�����ɍ��킹�ĕύX����
		{
			//�U������
			UKismetSystemLibrary::PrintString(this, TEXT("BossEnemy Attack!"), true, true, FColor::Yellow, 2.f, TEXT("None"));
		}
		break;

	case EState::Die:
		EnemyDead();
		break;
	}
}

//�_���[�W���󂯂鏈���A�����ł�������U���͕��̗͂����炷
void ABossEnemy::RecieveEnemyDamage(int DamageAmount)
{
	//���C�g�̐F�ƓG�̐F����v�����Ƃ������_���[�W���󂯂�
	Status.HP -= DamageAmount;
}

//�v���C���[�̃��C�g�̐F�ƓG�̃��C�g�̐F���`�F�b�N����֐�
bool ABossEnemy::CheckPlayerLightColor(EFlashlight_Color PlayerColor) const
{
	return (int)PlayerColor == (int)this->EnemyColor;
}