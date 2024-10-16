// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BossEnemy.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/VRPlayerCharacter.h"
#include "Enemy/NormalEnemy.h"
#include "Enemy/GreenEnemy.h"
#include "Enemy/RedEnemy.h"
#include "Enemy/BlueEnemy.h"

ABossEnemy::ABossEnemy()
	:
	BossState(EBossState::Wait),
	PlayerLocation_BossRoom(FVector(0, 0, 0)),
	PlayerRotation_Z_BossRoom(0.f),
	FinishCount(0.f),
	//�ҋ@�֌W
	ChangingBossColor(EEnemyColor::White), bHasEndedWait(false), bHasFinishedTransparentize(false), bHasFinishedChangeDecidedColor(false), ColorValue(FLinearColor(0, 0, 0)), bHasFinishedShow(false),
	bIsBattleStarted(true),
	//�`���[�W�֌W
	ChargeTime(0.f), bIsTransitionAttack(false), bIsTransitionStan(false), CountUpToAttackStateTransition(0), TimeUpToAttackStateTransition(1), ChargeCount(0), CountUpToAttack(5), StanValue(0), 
	MaxStanValue(5 * AssumptionFPS),
	//�X�^���֌W
	bHasEndedStan(false),
	//�ʏ�G�Ăъ֌W
	bHasEndedEnemyCall(false), CallingEnemyNumber(0),
	//�ʏ�G�̓����ҋ@�֌W
	NowExistsEnemyNumber(0),
	//�ʏ�G�̓�����֌W
	bHasEndedAfterEnemyExpedition(false),
	//�ړ��֌W
	BossGoalLocation(FVector(0.f, 0.f, 0.f)), bHasEndedTeleportation(false)
{
	//Tick��L���ɂ���
	PrimaryActorTick.bCanEverTick = true;

	//�}�e���A���̃I�p�V�e�B�̒l���ő�l�ɂ��Ă���
	this->OpacityValue_Body = 1.f;
	this->OpacityValue_Eye = 1.f;

	//���V�[���R���|�[�l���g
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

			//���}�e���A��--------------------------------------------------------------------------------------------------------
			//�̂̃}�e���A�������[�h
			TObjectPtr<UMaterial> BodyMaterial = LoadObject<UMaterial>(nullptr, TEXT("/Game/_TeamFolder/Enemy/M_Boss"));
			if (BodyMaterial) // null�`�F�b�N
			{
				//�_�C�i�~�b�N�}�e���A���C���X�^���X���쐬
				this->DynamicMaterial_Body = UMaterialInstanceDynamic::Create(BodyMaterial, this);

				//GhostMesh�Ƀ_�C�i�~�b�N�}�e���A����ݒ�
				this->GhostMeshComponent->SetMaterial(0, DynamicMaterial_Body);

				//�����I�p�V�e�B�l��ݒ�
				this->DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Body);
			}
			//�ڂ̃}�e���A�������[�h
			TObjectPtr<UMaterial> EyeMaterial = LoadObject<UMaterial>(nullptr, TEXT("/Game/_TeamFolder/CG/CG_Model/Ghost/M_Ghost_Eye"));
			if (EyeMaterial) // null�`�F�b�N
			{
				//�_�C�i�~�b�N�}�e���A���C���X�^���X���쐬
				this->DynamicMaterial_Eye = UMaterialInstanceDynamic::Create(EyeMaterial, this);

				//GhostMesh�Ƀ_�C�i�~�b�N�}�e���A����ݒ�
				this->GhostMeshComponent->SetMaterial(1, DynamicMaterial_Eye);

				//�����I�p�V�e�B�l��ݒ�
				this->DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Eye);
			}
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

	//���{�X�G�̐ݒ�-----------------------------------------------------------------------------------------------
	this->Status.MaxHP = 300;
	this->Status.HP = Status.MaxHP;
	this->EnemyColor = EEnemyColor::BossColor;
}

void ABossEnemy::BeginPlay()
{
	Super::BeginPlay();

}

void ABossEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickProcess();
}

//----------------------------------------------------------------------------------------
//���ǉ��֐��̏�����
//----------------------------------------------------------------------------------------
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
	EBossState NowState = this->BossState;
	switch (NowState)
	{
	//�ҋ@-----------------------------------------------------------------------------------
	case EBossState::Wait:
		if (bHasEndedWait)
		{
			int StateDecideNumber = FMath::RandRange((int)EBossState::Wait + 1, (int)EBossState::MaxStateCountAtTransitionFromWait - 1); //�ҋ@��ԈȊO�̐������ɂȂ��Ԃ������_���Ō��߂�
			//��ԑJ��
			switch (StateDecideNumber)
			{
			case 1:	// �`���[�W��Ԃ�
				NowState = EBossState::Charge;
				UKismetSystemLibrary::PrintString(this, TEXT("Change Charge"), true, true, FColor::Red, 2.f);
				break;

			case 2:	// �G���G���Ăԏ�Ԃ�
				NowState = EBossState::EnemyCall;
				UKismetSystemLibrary::PrintString(this, TEXT("Change EnemyCall"), true, true, FColor::Green, 2.f);
				break;

			case 3:	// �u�Ԉړ���Ԃ�
				NowState = EBossState::Teleportation;
				UKismetSystemLibrary::PrintString(this, TEXT("Change Teleportation"), true, true, FColor::Blue, 2.f);
				break;
			}
		}
		if (Status.HP <= 0) { NowState = EBossState::Die; }	// ���S��Ԃ�
		break;

	//�`���[�W-------------------------------------------------------------------------------
	case EBossState::Charge:
		if (bIsTransitionAttack) { NowState = EBossState::Attack; }	// �U����Ԃ�
		if (bIsTransitionStan) { NowState = EBossState::Stan; }		// �X�^����Ԃ�
		if (Status.HP <= 0) { NowState = EBossState::Die; }			// ���S��Ԃ�
		break;

	//�U��-----------------------------------------------------------------------------------
	case EBossState::Attack:
		if (this->bHasEndedAttack) { NowState = EBossState::Wait; }	//�ҋ@��Ԃ�
		if (Status.HP <= 0) { NowState = EBossState::Die; }			//���S��Ԃ�
		break;

	//�X�^��---------------------------------------------------------------------------------
	case EBossState::Stan:
		if (bHasEndedStan) { NowState = EBossState::Wait; }			//�ҋ@��Ԃ�
		if (Status.HP <= 0) { NowState = EBossState::Die; }			//���S��Ԃ�
		break;

	//�ʏ�G���Ă�---------------------------------------------------------------------------
	case EBossState::EnemyCall:
		if (bHasEndedEnemyCall) { NowState = EBossState::EnemyExpeditionWait; }	//�ʏ�G�̓����ҋ@��Ԃ�
		if (Status.HP <= 0) { NowState = EBossState::Die; }						//���S��Ԃ�
		break;

	//�ʏ�G�̓����ҋ@-----------------------------------------------------------------------
	case EBossState::EnemyExpeditionWait:
		if (NowExistsEnemyNumber == 0) { NowState = EBossState::AfterEnemyExpedition; }	//�ʏ�G�̓�������
		break;

	//�ʏ�G�̓�����-------------------------------------------------------------------------
	case EBossState::AfterEnemyExpedition:
		if (bHasEndedAfterEnemyExpedition) { NowState = EBossState::Wait; }	//�ҋ@��Ԃ�
		if (Status.HP <= 0) { NowState = EBossState::Die; }					//���S��Ԃ�
		break;

	//�u�Ԉړ�-------------------------------------------------------------------------------
	case EBossState::Teleportation:
		if (MoveCount >= 1 * Gamefps) { NowState = EBossState::Wait; }	//�ҋ@��Ԃ�
		if (Status.HP <= 0) { NowState = EBossState::Die; }				//���S��Ԃ�
		break;
	}

	UpdateState(NowState);
}

//��Ԃ̍X�V
void ABossEnemy::UpdateState(EBossState nowState)
{
	if (nowState != this->BossState)
	{
		this->BossState = nowState;
		this->MoveCount = 0;
		this->bShouldBeenProcessWhenFirstStateTransition = false;
	}
}

//��ԂɊ�Â�������������
void ABossEnemy::ActProcess()
{
	switch (this->BossState)
	{
	//�ҋ@-----------------------------------------------------------------------------------
	case EBossState::Wait:
		//�퓬���n�܂��Ă�����F�̕ύX���s��
		if (bIsBattleStarted)
		{
			//��ԁFWait�J�ڎ��ɂ̂ݍs������
			if (this->bShouldBeenProcessWhenFirstStateTransition == false)
			{
				ProcessJustForFirst_Wait();
			}

			//�F�̕ύX����
			bHasEndedWait = ChangeColor(ChangingBossColor);
		}
		break;

	//�`���[�W-------------------------------------------------------------------------------
	case EBossState::Charge:
		//��ԁFCharge�J�ڎ��ɂ̂ݍs������
		if (this->bShouldBeenProcessWhenFirstStateTransition == false)
		{
			ProcessJustForFirst_Charge();
		}

		//�U���`���[�W�̏���
		ChargeAttack();
		break;

	//�U��-----------------------------------------------------------------------------------
	case EBossState::Attack:
		//��ԁFAttack�J�ڎ��ɂ̂ݍs������
		if (this->bShouldBeenProcessWhenFirstStateTransition == false)
		{
			ProcessJustForFirst_Attack();
		}

		//�U������
		this->bHasEndedAttack = Attack();
		break;

	//�X�^��---------------------------------------------------------------------------------
	case EBossState::Stan:
		//�X�^�����̏���
		bHasEndedStan = Stan();
		break;

	//�ʏ�G�Ăяo��-------------------------------------------------------------------------
	case EBossState::EnemyCall:
		//��ԁFEnemyCall�J�ڎ��ɂ̂ݍs������
		if (this->bShouldBeenProcessWhenFirstStateTransition == false)
		{
			ProcessJustForFirst_EnemyCall();
		}

		//�G�Ăяo���̏���
		bHasEndedEnemyCall = EnemyCall();
		break;

	//�ʏ�G�̓����ҋ@-----------------------------------------------------------------------
	case EBossState::EnemyExpeditionWait:
		break;

	//�ʏ�G�̓�����-------------------------------------------------------------------------
	case EBossState::AfterEnemyExpedition:
		//�ʏ�G�̓������Ԃōs������
		if (MoveCount >= 1 * FMath::RoundToInt(Gamefps)) // Gamefps�ɂ����Ă��鐔�͓G�𐶐�����܂ł̕b��
		{
			AfterEnemyExpeditionProcess();
		}
		break;

	//�u�Ԉړ�-------------------------------------------------------------------------------
	case EBossState::Teleportation:
		/*����*/
		/*�t�@�C���ǂݍ���(�����ňړ��p�^�[�������肳����)*/
		/*�t�@�C������ǂݍ��񂾒l���A�N�^�[�̍��W�ɂ���*/
		//���:Teleportation�J�ڎ��ɂ̂ݍs������
		if (this->bShouldBeenProcessWhenFirstStateTransition == false)
		{
			ProcessJustForFirst_Move();
		}

		//�u�Ԉړ�
		bHasEndedTeleportation = Move();
		break;

	//���S-----------------------------------------------------------------------------------
	case EBossState::Die:
		EnemyDead();
		break;
	}
}

//�v���C���[���{�X�����Ŏ~�܂������A�ʒu�Ɖ�]���擾����
void ABossEnemy::GetPlayerLocationAndRotation(const FVector PlayerLocation, const float PlayerRotation_Z)
{
	PlayerLocation_BossRoom = PlayerLocation;
	PlayerRotation_Z_BossRoom = PlayerRotation_Z;
}

//1�b�J�E���g������true��Ԃ�
bool ABossEnemy::bOneSecondsPassedIs()
{
	FinishCount++;

	if (FinishCount >= FMath::RoundToInt(Gamefps) * 1)
	{
		FinishCount = 0;
		return true;
	}

	return false;
}

//����ԁFWait�̏���---------------------------------------------------------------------------------------------
//��ԁFWait�J�ڎ��ɂ̂ݍs������
void ABossEnemy::ProcessJustForFirst_Wait()
{
	//�ύX����F�����߂�
	do
	{
		ChangingBossColor = (EEnemyColor)FMath::RandRange(1, static_cast<int>(EEnemyColor::ColorTypeCount) - 1);
	} while (ChangingBossColor == this->EnemyColor);

	//�Ă΂��֐��𐧌䂷��ϐ���false�ɂ���
	bHasFinishedTransparentize = false;
	bHasFinishedChangeDecidedColor = false;
	bHasFinishedShow = false;

	//���̊֐���������Ă΂�Ȃ��悤�ɂ���
	this->bShouldBeenProcessWhenFirstStateTransition = true;
}

//�F�̕ύX����
bool ABossEnemy::ChangeColor(const EEnemyColor ChangingColor)
{
	//DeltaTime�̎擾
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	//�����ɂ���
	if (bHasFinishedTransparentize == false)
	{
		bHasFinishedTransparentize = Transparentize(DeltaTime);
	}

	//���߂��F�ɕύX����
	if (bHasFinishedTransparentize == true && bHasFinishedChangeDecidedColor == false)
	{
		bHasFinishedChangeDecidedColor = ChangeDecidedColor(ChangingColor);
	}

	//�\��������
	if (bHasFinishedChangeDecidedColor == true && bHasFinishedShow == false)
	{
		bHasFinishedShow = Show(DeltaTime);
	}

	//�F�̕ύX�����I��
	if (bHasFinishedShow == true)
	{
		//1�b�҂������Ԃ�ύX�ł���悤�ɂ���
		if (bOneSecondsPassedIs())
		{
			return true;
		}
	}

	//�F�̕ύX�������s
	return false;
}

//�����ɂ��鏈��
bool ABossEnemy::Transparentize(const float DeltaTime)
{
	if (DynamicMaterial_Body && DynamicMaterial_Eye) // null�`�F�b�N
	{
		//�I�p�V�e�B�̒l���v�Z
		OpacityValue_Body -= MaxOpacity_Body / TimeUpToTransparency * DeltaTime;	// �̂̃I�p�V�e�B�̌v�Z
		OpacityValue_Eye -= MaxOpacity_Eye / TimeUpToTransparency * DeltaTime;		// �ڂ̃I�p�V�e�B�̌v�Z

		//�o�����I������珈�����I������
		if (OpacityValue_Body <= 0.f && OpacityValue_Eye <= 0.f)
		{
			//�I�p�V�e�B�̒l��0�������Ȃ��悤�ɂ���
			OpacityValue_Body = 0.f;
			OpacityValue_Eye = 0.f;

			//�I�p�V�e�B��ݒ�
			DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Body);
			DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Eye);

			//���̊֐����Ă΂�Ȃ��悤�ɂ���
			return true;
		}

		//�I�p�V�e�B��ݒ�
		DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Body);
		DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Eye);
	}

	//������x���̊֐����Ă�
	return false;
}

//���߂��F�ɕύX����
bool ABossEnemy::ChangeDecidedColor(const EEnemyColor ChangingColor)
{
	//�G�̌��݂̐F����ύX
	this->EnemyColor = ChangingBossColor;

	//�ǂ̐F�ɕύX���邩����
	switch (ChangingColor)
	{
	case EEnemyColor::Green:	//��
		ColorValue = FLinearColor(0.f, 1.f, 0.f);		// �΂̓G�Ɠ����F
		break;

	case EEnemyColor::Red:		//��
		ColorValue = FLinearColor(1.f, 0.f, 0.f);		// �Ԃ̓G�Ɠ����F
		break;

	case EEnemyColor::Blue:		//��
		ColorValue = FLinearColor(0.f, 0.f, 1.f);		// �̓G�Ɠ����F
		break;
	}

	//�F��ύX
	DynamicMaterial_Body->SetVectorParameterValue(FName("BaseColor"), ColorValue);

	//���̊֐����I����܂ł̃J�E���g
	FinishCount++;

	if (FinishCount >= 1 * Gamefps) // Gamefps�ɂ����Ă��鐔�͓����ɂȂ��Ă��鎞��
	{
		//FinishCount���[���N���A����
		FinishCount = 0;

		//���̊֐����Ă΂�Ȃ��悤�ɂ���
		return true;
	}

	//������x���̊֐����Ă�
	return false;
}

//�\��������
bool ABossEnemy::Show(const float DeltaTime)
{
	if (DynamicMaterial_Body && DynamicMaterial_Eye)
	{
		//�I�p�V�e�B�̒l���v�Z
		OpacityValue_Body += MaxOpacity_Body / TimeSpentInAppear * DeltaTime;	// �̂̃I�p�V�e�B�̌v�Z
		OpacityValue_Eye += MaxOpacity_Eye / TimeSpentInAppear * DeltaTime;		// �ڂ̃I�p�V�e�B�̌v�Z

		//�o�����I������珈�����I������
		if (OpacityValue_Body >= MaxOpacity_Body && OpacityValue_Eye >= MaxOpacity_Eye)
		{
			//�I�p�V�e�B�̒l��1�𒴂��Ȃ��悤�ɂ���
			OpacityValue_Body = MaxOpacity_Body;
			OpacityValue_Eye = MaxOpacity_Eye;

			//�I�p�V�e�B��ݒ�
			DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Body);
			DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Eye);

			//�����蔻���t����
			GhostCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

			//��ԑJ�ډ\�ɂ���
			return true;
		}

		//�I�p�V�e�B��ݒ�
		DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Body);
		DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Eye);
	}

	//������x���̊֐����Ă�
	return false;
}

//�{�X�̐퓬�s�����J�n������
void ABossEnemy::BattleStart()
{
	bIsBattleStarted = true;
}

//����ԁFCharge�̏���------------------------------------------------------------------------------------------- 
//��ԁFCharge�J�ڎ��ɂ̂ݍs������ 
void ABossEnemy::ProcessJustForFirst_Charge()
{
	//�J�ڐ�����߂�ϐ���false�ɂ���
	bIsTransitionAttack = false;	// �U����ԂɑJ�ڂ��邩
	bIsTransitionStan = false;		// �X�^����ԂɑJ�ڂ��邩

	//�{�X�̏o�����W���N���A����
	BossGoalLocations.Empty();

	//���݈ʒu���擾
	this->CurrentLocation = GetActorLocation();

	//�{�X�̏u�Ԉړ�������W��ݒ肷��
	for (int i = 0; i < CountUpToAttack; ++i)
	{
		//�{�X�̈ړ��ʒu�����߂郉�W�A���̒l��ݒ�
		float GoalDegrees = PlayerRotation_Z_BossRoom + FMath::FRandRange(-90.f, 90.f);	// ���x�ɂ��邩�v�Z
		float GoalRadians = FMath::DegreesToRadians(GoalDegrees);						// ���W�A���ɕϊ�

		//�{�X�̈ړ��ʒu�����߂�
		float GoalX = (PlayerLocation_BossRoom.X + 500) * FMath::Cos(GoalRadians);
		float GoalY = (PlayerLocation_BossRoom.Y + 500) * FMath::Sin(GoalRadians);
		float GoalZ = CurrentLocation.Z;

		//�{�X�̏o���ʒu��ݒ�
		BossGoalLocations.Add(FVector(GoalX, GoalY, GoalZ));
	}

	//���̊֐���������Ă΂�Ȃ��悤�ɂ���
	this->bShouldBeenProcessWhenFirstStateTransition = true;
}

//�U���`���[�W�̏���
void ABossEnemy::ChargeAttack()
{
	//�`���[�W���I����Ă��Ȃ�������
	if (ChargeCount < CountUpToAttack)
	{
		//���܂����b����������
		if (MoveCount % (FMath::RoundToInt(Gamefps) * 1) == 0) // FMath::RoundToInt(Gamefps)�Ɋ|���Ă���}�W�b�N�i���o�[�͕b����\��
		{
			//�u�Ԉړ����A�`���[�W�񐔂̃J�E���g�𑝂₷
			Teleportation_Charge();
		}
	}

	//�`���[�W������������
	if (ChargeCount == CountUpToAttack)
	{
		//��ԁFAttack�ɑJ�ڂ���܂ł̎��Ԃ��J�E���g����
		CountUpToAttackStateTransition++;

		//�U����ԂɑJ�ڏo����悤�ɂ���
		if (CountUpToAttackStateTransition == TimeUpToAttackStateTransition * FMath::RoundToInt(Gamefps))
		{
			//�U���܂ł̃J�E���g�A�`���[�W�񐔂̃��Z�b�g
			CountUpToAttackStateTransition = 0;	// �J�E���g�̃��Z�b�g
			ChargeCount = 0;					// �`���[�W�񐔂̃��Z�b�g

			//�U����ԂɑJ�ڏo����悤�ɂ���
			bIsTransitionAttack = true;
		}
	}

	//�X�^���l���ő�ɂȂ�����
	if (StanValue >= MaxStanValue)
	{
		//�X�^���l���ő�l�𒴂��Ȃ��悤�ɂ���
		StanValue = MaxStanValue;

		//�X�^����ԂɑJ�ڂ���
		bIsTransitionStan = true;
	}
}

//�u�Ԉړ����Ȃ���`���[�W���s������
void ABossEnemy::Teleportation_Charge()
{
	//���u�Ԉړ�����
	//�ړ�����W��ݒ�(���[���h���W)
	this->GoalLocation_World = BossGoalLocations[ChargeCount];

	//�ړ�������
	SetActorLocation(this->GoalLocation_World);

	//�`���[�W����
	ChargeCount++;
}

//����ԁFAttack�̏���-------------------------------------------------------------------------------------------
//��ԁFAttack�J�ڎ��ɂ̂ݍs������
void ABossEnemy::ProcessJustForFirst_Attack()
{
	//�X�^���l��0�ɂ���
	StanValue = 0;

	//���̊֐���������Ă΂�Ȃ��悤�ɂ���
	this->bShouldBeenProcessWhenFirstStateTransition = true;
}

//�U������
bool ABossEnemy::Attack()
{
	//�U������
	if (MoveCount == 1 * FMath::RoundToInt(Gamefps))
	{
		UKismetSystemLibrary::PrintString(this, TEXT("BossEnemy Attack!"), true, true, FColor::Yellow, 2.f, TEXT("None"));

		//�v���C���[�փ_���[�W��^����
		//�v���C���[�փ_���[�W��^����
		//�v���C���[�̏��擾
		TObjectPtr<AVRPlayerCharacter> Player = Cast<AVRPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		if (Player)
		{
			Player->RecievePlayerDamage();
		}

		//�U���I��(�������Ő��䂵�A�A�j���[�V�������I�������ɂ��邩��)
		return true;
	}

	//������x���̊֐����Ă�
	return false;
}

//����ԁFStan�̏���---------------------------------------------------------------------------------------------
//�X�^������
bool ABossEnemy::Stan()
{
	//�X�^���l�̌���
	StanValue -= FMath::RoundToInt(1 * Gamefps / AssumptionFPS);

	//�X�^���l��0�ɂȂ�����ҋ@��Ԃɖ߂�
	if (StanValue <= 0)
	{
		//�X�^���l��0�����ɂȂ�Ȃ��悤�ɂ���
		StanValue = 0;

		//�X�^����ԏI��
		return true;
	}

	//������x���̊֐����Ă�
	return false;
}

//����ԁFEnemyCall�̏���----------------------------------------------------------------------------------------
//��ԁFEnemyCall�J�ڎ��ɂ̂ݍs������
void ABossEnemy::ProcessJustForFirst_EnemyCall()
{
	//�G�����ɕK�v�ȏ����i�[���Ă���z�����ɂ���
	NormalEnemyPopLocations.Empty();
	CallingEnemyColors.Empty();

	//�ʏ�G�̐����������߂�(2�ȏ�4�ȉ��͈̔�)
	DecideCallingEnemyNumber();

	//�ʏ�G�̏o���ʒu�����߂�
	DecideEnemyPopLocations(CallingEnemyNumber);

	//���F�̓G�𐶐����邩���߂�
	DecideCreateEnemyColor(CallingEnemyNumber);

	//����������֐��𐧌䂷��ϐ���false�ɂ���
	bHasFinishedTransparentize = false;

	//���̊֐����Ă΂���Ԃ̌�ɂ��������J�ڂ��Ȃ���Ԃ𐧌䂷��ϐ���false�ɂ���
	bHasEndedAfterEnemyExpedition = false;

	//���̊֐���������Ă΂�Ȃ��悤�ɂ���
	this->bShouldBeenProcessWhenFirstStateTransition = true;
}

//�ʏ�G�̐����������߂�(2�ȏ�4�ȉ��͈̔�)
void ABossEnemy::DecideCallingEnemyNumber()
{
	CallingEnemyNumber = 2; // �ʏ�G�̐��F2

	//�c��HP��2/3�ȉ��ɂȂ�����
	if (Status.HP <= Status.HP * 2 / 3)
	{
		CallingEnemyNumber = 3; // �ʏ�G�̐��F3
	}

	//�c��HP��1/3�ȉ��ɂȂ�����
	if (Status.HP <= Status.MaxHP / 3)
	{
		CallingEnemyNumber = 4; // �ʏ�G�̐��F4
	}
}

//�ʏ�G�̏o���ʒu�����߂�(�{�X�G����̑��Έʒu)
void ABossEnemy::DecideEnemyPopLocations(const int CallingEnemyNum)
{
	switch (CallingEnemyNum)
	{
	case 2:		//2��
		NormalEnemyPopLocations.Add(FVector(0.f, 300.f, 0.f));		// �{�X����E��300���j�b�g�̈ʒu
		NormalEnemyPopLocations.Add(FVector(0.f, -300.f, 0.f));		// �{�X���獶��300���j�b�g�̈ʒu
		break;

	case 3:		//3��
		NormalEnemyPopLocations.Add(FVector(0.f, 300.f, -50.f));	// �{�X����E��300���j�b�g�A����50���j�b�g�̈ʒu
		NormalEnemyPopLocations.Add(FVector(0.f, -300.f, -50.f));	// �{�X���獶��300���j�b�g�A����50���j�b�g�̈ʒu
		NormalEnemyPopLocations.Add(FVector(0.f, 0.f, 100.f));		// �{�X������100���j�b�g�̈ʒu
		break;

	case 4:		//4��
		NormalEnemyPopLocations.Add(FVector(0.f, 300.f, -100.f));	// �{�X����E��300���j�b�g�A����100���j�b�g�̈ʒu
		NormalEnemyPopLocations.Add(FVector(0.f, -300.f, -100.f));	// �{�X���獶��300���j�b�g�A����100���j�b�g�̈ʒu
		NormalEnemyPopLocations.Add(FVector(0.f, 300.f, 100.f));	// �{�X����E��300���j�b�g�A���100���j�b�g�̈ʒu
		NormalEnemyPopLocations.Add(FVector(0.f, -300.f, 100.f));	// �{�X���獶��300���j�b�g�A���100���j�b�g�̈ʒu
		break;

	default:	//�s���Ȑ�
		for (int i = 0; i < CallingEnemyNum; ++i) // �G�������Ƀ������[���[�N���N����Ȃ��悤�ɂ���
		{
			NormalEnemyPopLocations.AddZeroed();
		}
		break;
	}
}

//���F�̓G����������邩���߂�
void ABossEnemy::DecideCreateEnemyColor(const int CallingEnemyNum)
{
	for (int i = 0; i < CallingEnemyNum; ++i) // ��������G�̐��������[�v����
	{
		//���F�̓G�𐶐����邩�����_���Ō��߂�
		CallingEnemyColors.Add(static_cast<EEnemyColor>(FMath::RandRange(1, static_cast<int>(EEnemyColor::ColorTypeCount) - 1)));
	}
}

//�ʏ�G�Ăя���
bool ABossEnemy::EnemyCall()
{
	//DeltaTime�̎擾
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	//�G���o�����郏�[�v�z�[���̂悤�ȃG�t�F�N�g�̐���

	//����������(Wait�ł̓����������𗬗p)
	if (bHasFinishedTransparentize == false)
	{
		bHasFinishedTransparentize = Transparentize(DeltaTime);
	}

	//��������̏���
	if (bHasFinishedTransparentize == true)
	{
		//���̊֐����I������܂ł̃J�E���g
		FinishCount++;

		if (FinishCount >= 1 * Gamefps) // Gamefps�ɂ����Ă��鐔�͓G�𐶐�����܂ł̕b��
		{
			//FinishCount���[���N���A����
			FinishCount = 0;

			//�ʏ�G�̐���
			CreateEnemies(CallingEnemyNumber, NormalEnemyPopLocations, CallingEnemyColors);

			//�{�X�G�̏�Ԃ�G�̓����ҋ@��ԂɑJ�ڂł���悤�ɂ���
			return true;
		}
	}

	//������x���̊֐����Ă�
	return false;
}

// �ʏ�G�̐�������
void ABossEnemy::CreateEnemies(const int CallingEnemyNum, const TArray<FVector>& PopLocations, const TArray<EEnemyColor>& EnemyColors)
{
	//�{�X�G�̎Q�Ƃ��擾
	TObjectPtr<ABossEnemy> BossEnemy = Cast<ABossEnemy>(UGameplayStatics::GetActorOfClass(GetWorld(), ABossEnemy::StaticClass()));

	if (BossEnemy) // �{�X�G�̎Q�Ƃ�����Ă��邩
	{
		//�ʏ�G�̐���
		for (int i = 0; i < CallingEnemyNum; ++i)
		{
			//��������G�̃C���X�^���X
			TObjectPtr<AEnemys> CallingEnemy = nullptr;

			////���߂��F�̓G�𐶐�����
			switch (EnemyColors[i])
			{
			case EEnemyColor::White:	//��
				CallingEnemy = GetWorld()->SpawnActor<AEnemys>(ANormalEnemy::StaticClass(), FVector(0.f, 0.f, 0.f), FRotator(0.f, 0.f, 0.f));
				break;

			case EEnemyColor::Green:	//��
				CallingEnemy = GetWorld()->SpawnActor<AEnemys>(AGreenEnemy::StaticClass(), FVector(0.f, 0.f, 0.f), FRotator(0.f, 0.f, 0.f));
				break;

			case EEnemyColor::Red:		//��
				CallingEnemy = GetWorld()->SpawnActor<AEnemys>(ARedEnemy::StaticClass(), FVector(0.f, 0.f, 0.f), FRotator(0.f, 0.f, 0.f));
				break;

			case EEnemyColor::Blue:		//��
				CallingEnemy = GetWorld()->SpawnActor<AEnemys>(ABlueEnemy::StaticClass(), FVector(0.f, 0.f, 0.f), FRotator(0.f, 0.f, 0.f));
				break;
			}

			//�{�X�G��e�Ƃ��A�{�X�G����̑��Έʒu�ŏo���ʒu��ݒ肷��
			if (CallingEnemy) // �G�C���X�^���X����������Ă�����
			{
				//�{�X�A�N�^�[��e�Ƃ���
				CallingEnemy->AttachToActor(BossEnemy, FAttachmentTransformRules::KeepRelativeTransform);

				//���Έʒu��ݒ�
				CallingEnemy->SetActorRelativeLocation(PopLocations[i]);
			}

			//�Ă�(��������)�G�̐����J�E���g����
			NowExistsEnemyNumber++;
		}
	}
}

//����ԁFEnemyExpeditionWait�̏���------------------------------------------------------------------------------
//�Ă񂾓G�����񂾂Ƃ��A���̎��񂾂Ƃ��������󂯎��(NowExistsEnemyNumber--����)
void ABossEnemy::ReceiveInfoDeadCalledEnemy()
{
	//���݂̑��݂��Ă���G�̐������炷
	NowExistsEnemyNumber--;
}

//����ԁFAfterEnemyExpedition�̏���-----------------------------------------------------------------------------
//�ʏ�G�̓������Ԃōs������
void ABossEnemy::AfterEnemyExpeditionProcess()
{
	//DeltaTime�̎擾
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	//�{�X�G��\��������(�ҋ@��Ԃ̕\�������𗬗p)
	bHasEndedAfterEnemyExpedition = Show(DeltaTime); // �\�����I������玟�̏�ԂɑJ�ڂ���
}

//����ԁFTeleportation�̏���------------------------------------------------------------------------------------
//���:Teleportation�J�ڎ��ɂ̂ݍs������
void ABossEnemy::ProcessJustForFirst_Move()
{
	//���݈ʒu���擾
	this->CurrentLocation = GetActorLocation();

	//�{�X�̏u�Ԉړ�������W��ݒ肷��
	//�{�X�̈ړ��ʒu�����߂郉�W�A���̒l��ݒ�
	float GoalDegrees = PlayerRotation_Z_BossRoom + FMath::FRandRange(-90.f, 90.f);	// ���x�ɂ��邩�v�Z
	float GoalRadians = FMath::DegreesToRadians(GoalDegrees);						// ���W�A���ɕϊ�

	//�{�X�̈ړ��ʒu�����߂�
	float GoalX = (PlayerLocation_BossRoom.X + 500) * FMath::Cos(GoalRadians);
	float GoalY = (PlayerLocation_BossRoom.Y + 500) * FMath::Sin(GoalRadians);
	float GoalZ = CurrentLocation.Z;

	//�{�X�̏o���ʒu��ݒ�
	BossGoalLocation = FVector(GoalX, GoalY, GoalZ);

	//���̊֐���������Ă΂�Ȃ��悤�ɂ���
	this->bShouldBeenProcessWhenFirstStateTransition = true;
}

//�u�Ԉړ�
bool ABossEnemy::Move()
{
	//�u�Ԉړ�����
	SetActorLocation(this->BossGoalLocation);

	return true;
}

//����ԁFAppear�̏���-------------------------------------------------------------------------------------------

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

/*����*/
/*�]�T����������A�e��ԂɑJ�ڂ���ׂ�bool�^�ϐ����ЂƂ܂Ƃ߂ɂ���*/