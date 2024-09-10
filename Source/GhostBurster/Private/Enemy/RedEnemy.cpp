// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/RedEnemy.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/VRPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

ARedEnemy::ARedEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	//���V�[���R���|�[�l���g--------------------------------------------------------------------------------------------------------
	//�V�[���R���|�[�l���g�̍쐬
	this->DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	if (this->DefaultSceneRoot)
	{
		//�V�[���R���|�[�l���g�����[�g�R���|�[�l���g�ɐݒ�
		RootComponent = this->DefaultSceneRoot;

		//���X�P���^�����b�V���R���|�[�l���g------------------------------------------------------------------------------------------
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

		//���R���W����-----------------------------------------------------------------------------------------------------------------
		//�X�t�B�A�R���W�����̍쐬
		GhostCollision = CreateDefaultSubobject<USphereComponent>(TEXT("GhostCollision"));
		if (GhostCollision)
		{
			//GhostCollision�����[�g�R���|�[�l���g�ɃA�^�b�`����
			GhostCollision->SetupAttachment(RootComponent);
			//GhostCollision�̃R���W�����v���Z�b�g��OverlapAllDynamic�ɂ���
			GhostCollision->SetCollisionProfileName("OverlapAllDynamic");
		}
	}

	//���A�j���[�V����-------------------------------------------------------------------------------------------------------------
	this->DefaultAnim = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/_TeamFolder/CG/CG_Model/Ghost/Anim_Idle"));			// ����̃A�j���[�V�������g�p���Ȃ���Ԃ̃A�j���[�V����
	this->AttackAnim = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/_TeamFolder/CG/CG_Model/Ghost/Test_GhostAttack"));	// �U����Ԃ̃A�j���[�V����

	//���T�E���h-------------------------------------------------------------------------------------------------------------------
	AppearSound = LoadObject<USoundCue>(nullptr, TEXT("/Game/_TeamFolder/Sound/SE/SE_GhostAppear_2_Cue"));	//�o�����̉��ݒ�
	DisappearSound = LoadObject<USoundCue>(nullptr, TEXT("/Game/_TeamFolder/Sound/SE/SE_GhostDead_2_Cue"));	//���Ŏ��̉��ݒ�
}

void ARedEnemy::BeginPlay()
{
	Super::BeginPlay();

	//���Ԃ̓G�̐ݒ�
	this->EnemyColor = EEnemyColor::Red;

	//���}�e���A��
	//�̂̃}�e���A�������[�h
	TObjectPtr<UMaterial> BodyMaterial = LoadObject<UMaterial>(nullptr, TEXT("/Game/_TeamFolder/Enemy/M_Red"));
	if (BodyMaterial)
	{
		//�_�C�i�~�b�N�}�e���A���C���X�^���X���쐬
		this->DynamicMaterial_Body = UMaterialInstanceDynamic::Create(BodyMaterial, this);

		//GhostMesh�Ƀ_�C�i�~�b�N�}�e���A����ݒ�
		this->GhostMeshComponent->SetMaterial(0, DynamicMaterial_Body);

		//�����I�p�V�e�B�l��ݒ�
		this->DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);
	}
	//�ڂ̃}�e���A�������[�h
	TObjectPtr<UMaterial> EyeMaterial = LoadObject<UMaterial>(nullptr, TEXT("/Game/_TeamFolder/CG/CG_Model/Ghost/M_Ghost_Eye"));
	if (EyeMaterial)
	{
		//�_�C�i�~�b�N�}�e���A���C���X�^���X���쐬
		this->DynamicMaterial_Eye = UMaterialInstanceDynamic::Create(EyeMaterial, this);

		//GhostMesh�Ƀ_�C�i�~�b�N�}�e���A����ݒ�
		this->GhostMeshComponent->SetMaterial(1, DynamicMaterial_Eye);

		//�����I�p�V�e�B�l��ݒ�
		this->DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);
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
		if (MoveCount >= AttackUpToTime * Gamefps) { NowState = EState::Attack; }	// �U����
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
	if (GoalLocations.Num() != 0) // �ڕW�n�_�����邩�m�F
	{
		//�[���N���A����
		this->TraveledDistance = 0.f;

		//�����ʒu�̐ݒ�
		this->CurrentLocation = GetActorLocation();

		//���[���h���W�ւ̕ϊ�
		this->GoalLocation_World = this->CurrentLocation + this->GoalLocations[MovingTimesCount];

		//�����x�N�g���̌v�Z
		this->Direction = (this->GoalLocation_World - this->CurrentLocation).GetSafeNormal();

		//���ړ������̌v�Z
		this->TotalDistance = FVector::Dist(this->CurrentLocation, this->GoalLocation_World);

		//�ړI�n�ɒ����܂ł̎��Ԃɍ����悤�ɑ��x���v�Z
		this->Status.Speed = this->TotalDistance / this->MoveTime;

		//�ړ��񐔂̊m�F
		if (this->MovingTimesCount == this->GoalLocations.Num() - 1) // �ړ��񐔂��i�[���Ă���ϐ��̒l���ڕW���W�̐��Ɠ������Ȃ�
		{
			//�ړ��񐔂̃��Z�b�g
			this->MovingTimesCount = 0;
		}
		else
		{
			//�ړ��񐔂𑝂₷
			this->MovingTimesCount++;
		}
	}

	//�����񏈗����s���Ȃ��悤�ɂ���
	this->bShouldBeenProcessWhenFirstStateTransition = true;
}

//�ړ�����
bool ARedEnemy::Move()
{
	if (GoalLocations.Num() != 0) // �ڕW�n�_�����邩�m�F
	{
		//DeltaTime�̎擾
		float DeltaTime = GetWorld()->GetDeltaSeconds();

		//�ړI�n�܂ł̎c�苗�����v�Z
		float RemainingDistance = TotalDistance - TraveledDistance;

		//���݂̑��x�ł̈ړ��������v�Z
		float DeltaDistance = Status.Speed * DeltaTime;

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

		// �V�����ʒu���v�Z
		FVector NewLocation = CurrentLocation + (Direction * TraveledDistance);

		// �V�����ʒu�Ɉړ�
		SetActorLocation(NewLocation);

		// �ړI�n�ɓ��B�����珈�����I��
		if (TraveledDistance >= TotalDistance)
		{
			SetActorLocation(this->GoalLocation_World);

			//��ԑJ�ڂł���悤�ɂ���
			return true;
		}

		//������x���̊֐����Ă�
		return false;
	}

	// �ڕW�n�_�������������ԑJ�ڂł���悤�ɂ���
	return true;
}

//�U������
bool ARedEnemy::Attack()
{
	if (GhostMeshComponent && AttackAnim) // null�`�F�b�N
	{
		//�U������
		if (MoveCount == FMath::RoundToInt(AttackTiming * Gamefps / AssumptionFPS)) // �G�Ƀ_���[�W��^����
		{
			/*UKismetSystemLibrary::PrintString(this, TEXT("RedEnemy Attack!"), true, true, FColor::Red, 2.f, TEXT("None"));*/

			//�v���C���[�փ_���[�W��^����
			//�v���C���[�̏��擾
			AVRPlayerCharacter* Player = Cast<AVRPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
			if (Player)
			{
				Player->RecievePlayerDamage();
			}
		}
		if (!GhostMeshComponent->IsPlaying()) // �A�j���[�V�������I�������
		{
			//�U���I��(�������Ő��䂵�A�A�j���[�V�������I�������ɂ��邩��)
			return true;
		}
	}
	else
	{
		//�U���I��(�������Ő��䂵�A�A�j���[�V�������I�������ɂ��邩��)
		return true;
	}

	//������x���̊֐����Ă�
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

	if (DynamicMaterial_Body && DynamicMaterial_Eye)
	{
		//�I�p�V�e�B�̒l��ύX
		this->OpacityValue += 1.f / (float)TimeSpentInAppear * DeltaTime;

		//�o�����I������珈�����I������
		if (this->OpacityValue >= 1.f)
		{
			//�I�p�V�e�B�̒l��1�𒴂��Ȃ��悤�ɂ���
			this->OpacityValue = 1.f;

			//�I�p�V�e�B��ݒ�
			this->DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);
			this->DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);

			//��ԑJ�ډ\�ɂ���
			return true;
		}

		//�I�p�V�e�B��ݒ�
		this->DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);
		this->DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);
	}

	return false;
}