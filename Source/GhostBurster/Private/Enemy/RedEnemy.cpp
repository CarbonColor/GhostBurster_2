// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/RedEnemy.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/VRPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

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
	//�X�^�e�B�b�N���b�V���̃R���W�����𖳂���
	GhostMesh->SetCollisionProfileName("NoCollision");

	//���R���W����
	//�X�t�B�A�R���W�����̍쐬
	GhostCollision = CreateDefaultSubobject<USphereComponent>(TEXT("GhostCollision"));
	//GhostCollision�����[�g�R���|�[�l���g�ɃA�^�b�`����
	GhostCollision->SetupAttachment(RootComponent);
	//GhostCollision�̃R���W�����v���Z�b�g��OverlapAllDynamic�ɂ���
	GhostCollision->SetCollisionProfileName("OverlapAllDynamic");

	//���T�E���h-----------------------------------------------------------------------------------------------------------------
	AppearSound = LoadObject<USoundCue>(nullptr, TEXT("/Game/_TeamFolder/Sound/SE/SE_GhostAppear_2_Cue"));	//�o�����̉��ݒ�
	DisappearSound = LoadObject<USoundCue>(nullptr, TEXT("/Game/_TeamFolder/Sound/SE/SE_GhostDead_2_Cue"));	//���Ŏ��̉��ݒ�
}

void ARedEnemy::BeginPlay()
{
	Super::BeginPlay();

	//���Ԃ̓G�̐ݒ�
	this->EnemyColor = EEnemyColor::Red;

	//���}�e���A��
	//�}�e���A�������[�h
	UMaterial* Material = LoadObject<UMaterial>(NULL, TEXT("/Game/_TeamFolder/Enemy/Red"), NULL, LOAD_None, NULL);
	if (Material)
	{
		//�_�C�i�~�b�N�}�e���A���C���X�^���X���쐬
		this->DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);

		//GhostMesh�Ƀ_�C�i�~�b�N�}�e���A����ݒ�
		GhostMesh->SetMaterial(0, DynamicMaterial);

		//�����I�p�V�e�B�l��ݒ�
		this->DynamicMaterial->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);
	}
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
	EState NowState = this->State;
	switch (NowState)
	{
	case EState::Wait:	//�ҋ@
		if (MoveCount >= 1 * Gamefps) { NowState = EState::Attack; }	// �U����
		if (Status.HP <= 0) { NowState = EState::Die; }				// ���S��
		break;

	case EState::Move:	//�ړ�
		if (this->bHasEndedMoving) { NowState = EState::Wait; }	// �ҋ@��
		if (Status.HP <= 0) { NowState = EState::Die; }			// ���S��
		break;

	case EState::Attack:	//�U��
		if (this->bHasEndedAttack) { NowState = EState::Wait; }	// �ҋ@��
		if (Status.HP <= 0) { NowState = EState::Die; }			// ���S��
		break;

	case EState::Appear:	//�o��
		if (this->bHasEndedAppear) { NowState = EState::Move; }	// �ړ���
		break;
	}

	UpdateState(NowState);
}

//��ԂɊ�Â�������������
void ARedEnemy::ActProcess()
{
	switch (this->State)
	{
	case EState::Wait:	//�ҋ@		
		break;

	case EState::Move:	//�ړ�
		//���Move�J�ڎ��ɂ̂ݍs������
		if (this->bShouldBeenProcessWhenFirstStateTransition == false)
		{
			ProcessJustForFirst_Move();
		}

		//�ړ�����(�ړ��������I��������ԑJ�ڂ���)
		this->bHasEndedMoving = Move();
		break;

	case EState::Attack:	//�U��
		//�U������
		this->bHasEndedAttack = this->Attack();
		break;

	case EState::Die:
		EnemyDead();
		break;

	case EState::Appear:	//�o��
		//���Move�J�ڎ��ɂ̂ݍs������
		if (this->bShouldBeenProcessWhenFirstStateTransition == false)
		{
			ProcessJustForFirst_Appear();
		}

		//�o������
		this->bHasEndedAppear = this->Appear();
		break;
	}
}

//�_���[�W���󂯂鏈���A�����ł�������U���͕��̗͂����炷
void ARedEnemy::RecieveEnemyDamage(int DamageAmount)
{
	if (this->State != EState::Appear && this->State != EState::Die)
	{
		Status.HP -= DamageAmount;
	}
}

//�v���C���[�̃��C�g�̐F�ƓG�̃��C�g�̐F���`�F�b�N����֐�
bool ARedEnemy::CheckPlayerLightColor(EFlashlight_Color PlayerColor) const
{
	return (int)PlayerColor == (int)this->EnemyColor;
}

//���Move�J�ڎ��ɂ̂ݍs������
void ARedEnemy::ProcessJustForFirst_Move()
{
	//�[���N���A����
	this->TraveledDistance = 0.f;

	// �����ʒu�̐ݒ�
	this->CurrentLocation = GetActorLocation();

	// �ڕW���W�̐ݒ�
	/*�����ړ���������s����悤�ɂȂ������A�O���t�@�C������ǂݍ���Őݒ�*/

	// ���[���h���W�ւ̕ϊ�
	this->GoalLocation_World = this->CurrentLocation + this->GoalLocation;

	// �����x�N�g���̌v�Z
	this->Direction = (this->GoalLocation_World - this->CurrentLocation).GetSafeNormal();

	// ���ړ������̌v�Z
	this->TotalDistance = FVector::Dist(this->CurrentLocation, this->GoalLocation_World);

	// �ړI�n�ɒ����܂ł̎��Ԃɍ����悤�ɑ��x���v�Z
	this->Speed = this->TotalDistance / this->MoveTime;

	//�����񏈗����s���Ȃ��悤�ɂ���
	this->bShouldBeenProcessWhenFirstStateTransition = true;
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
		SetActorLocation(this->GoalLocation_World);

		//��ԑJ�ڂł���悤�ɂ���
		return true;
	}

	return false;
}

//�U������
bool ARedEnemy::Attack()
{
	//�U������
	if (MoveCount >= AttackUpToTime * Gamefps)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("RedEnemy Attack!"), true, true, FColor::Red, 2.f, TEXT("None"));

		//�v���C���[�փ_���[�W��^����
		//�v���C���[�̏��擾
		AVRPlayerCharacter* Player = Cast<AVRPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		if (Player)
		{
			Player->RecievePlayerDamage();
		}

		//�U���I��(�������Ő��䂵�A�A�j���[�V�������I�������ɂ��邩��)
		return true;
	}

	return false;
}

//�o���֌W---------------------------------------------------------------------------------------------------------------------
//��ԁFAppear�ōŏ��Ɉ�x�������鏈��
void ARedEnemy::ProcessJustForFirst_Appear()
{
	//�G�o�����̉���炷
	PlayAppearSound();

	//�����񏈗����s���Ȃ��悤�ɂ���
	this->bShouldBeenProcessWhenFirstStateTransition = true;
}

//�G�o������
bool ARedEnemy::Appear()
{
	//DeltaTime�̎擾
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	if (DynamicMaterial)
	{
		//�I�p�V�e�B�̒l��ύX
		this->OpacityValue += 1.f / (float)TimeSpentInAppear * DeltaTime;

		//�o�����I������珈�����I������
		if (this->OpacityValue >= 1.f)
		{
			//�I�p�V�e�B�̒l��1�𒴂��Ȃ��悤�ɂ���
			this->OpacityValue = 1.f;

			//�I�p�V�e�B��ݒ�
			this->DynamicMaterial->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);

			//��ԑJ�ډ\�ɂ���
			return true;
		}

		//�I�p�V�e�B��ݒ�
		this->DynamicMaterial->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);
	}

	return false;
}