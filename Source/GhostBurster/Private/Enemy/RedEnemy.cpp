// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/RedEnemy.h"
#include "Kismet/KismetSystemLibrary.h"

ARedEnemy::ARedEnemy()
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
	UMaterial* Material = LoadObject<UMaterial>(NULL, TEXT("/Game/_Teamfolder/Enemy/Red"), NULL, LOAD_None, NULL);
	GhostMesh->SetMaterial(0, Material);

	//���R���W����
	//�X�t�B�A�R���W�����̍쐬
	GhostCollision = CreateDefaultSubobject<USphereComponent>(TEXT("GhostCollision"));
	//GhostCollision�����[�g�R���|�[�l���g�ɃA�^�b�`����
	GhostCollision->SetupAttachment(RootComponent);

	//���Ԃ̓G�̐ݒ�
	this->Status.HP = 100;
	this->enemyColor = EnemyColor::Red;
}

void ARedEnemy::BeginPlay()
{
	Super::BeginPlay();

}

void ARedEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickProcess();
}

//���ǉ��֐�
//Tick�ł̏���
void ARedEnemy::TickProcess()
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
void ARedEnemy::Think()
{
	State nowState = state;
	switch (nowState)
	{
	case State::Wait:	//�����Ă���
		if (MoveCount >= 60 * Gamefps / 60) { nowState = State::Move; }
		if (Status.HP <= 0) { nowState = State::Die; }
		break;

	case State::Move:	//����
		if (this->bHasEndedMoving) { nowState = State::Attack; }
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
void ARedEnemy::ActProcess()
{
	switch (state)
	{
	case State::Wait:	//�����Ă���		
		break;

	case State::Move:	//����
		//���Move�J�ڎ��ɂ̂ݍs������
		if (MoveCount == 0)
		{
			ProcessJustForFirst_Move();
		}

		//�ړ�����(�ړ��������I��������ԑJ�ڂ���)
		this->bHasEndedMoving = Move();
		break;

	case State::Attack:	//�U��
		if (MoveCount == AttackUpToTime * Gamefps / 60) //15�̕����͍U�����[�V�����ɍ��킹�ĕύX����
		{
			//�U������
			UKismetSystemLibrary::PrintString(this, TEXT("RedEnemy Attack!"), true, true, FColor::Red, 2.f, TEXT("None"));
		}
		break;

	case State::Die:
		EnemyDead();
		break;
	}
}

//�_���[�W���󂯂鏈���A�����ł�������U���͕��̗͂����炷
void ARedEnemy::RecieveEnemyDamage(int DamageAmount, EFlashlight_Color Color)
{
	if ((int)Color == (int)this->enemyColor)
	{
		Status.HP -= DamageAmount;
	}
}

//���Move�J�ڎ��ɂ̂ݍs������
void ARedEnemy::ProcessJustForFirst_Move()
{
	// �����ʒu�̐ݒ�
	CurrentLocation = GetActorLocation();

	// �����x�N�g���̌v�Z
	Direction = (GoalLocation - CurrentLocation).GetSafeNormal();

	// ���ړ������̌v�Z
	TotalDistance = FVector::Dist(CurrentLocation, GoalLocation);

	// �ړI�n�ɒ����܂ł̎��Ԃɍ����悤�ɑ��x���v�Z
	Speed = TotalDistance / this->MoveTime;
}

//�ړ�����
bool ARedEnemy::Move()
{
	//DeltaTime�̎擾
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	//�ړI�n�܂ł̎c�苗�����v�Z
	float RemainingDistance = TotalDistance - TraveledDistance;

	//���݂̑��x�ł̈ړ��������v�Z
	float DeltaDistance = Speed * DeltaTime;

	//�ړI�n�ɋ߂Â���������A�c��̋��������i�ނ悤�ɒ���
	if (DeltaDistance >= RemainingDistance)
	{
		DeltaDistance = RemainingDistance;
		TraveledDistance = TotalDistance;
	}
	else
	{
		TraveledDistance += DeltaDistance;
	}

	//�����g�Ɋ�Â��ăI�t�Z�b�g���v�Z(�ړI�n�ɏu�Ԉړ����Ē����Ȃ��悤�ɒ�������v�Z)
	float Offset_Z = Amplitude * FMath::Sin(2.0f * PI * (TraveledDistance / TotalDistance));

	// �V�����ʒu���v�Z
	FVector NewLocation = CurrentLocation + (Direction * TraveledDistance);
	NewLocation.Z += Offset_Z;

	// �V�����ʒu�Ɉړ�
	SetActorLocation(NewLocation);

	// �ړI�n�ɓ��B�����珈�����I��
	if (TraveledDistance >= TotalDistance)
	{
		SetActorLocation(GoalLocation);

		//��ԑJ�ڂł���悤�ɂ���
		return true;
	}

	return false;
}