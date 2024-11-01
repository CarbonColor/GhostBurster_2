// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BossEnemy.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/VRPlayerCharacter.h"
#include "Enemy/NormalEnemy.h"
#include "Enemy/GreenEnemy.h"
#include "Enemy/RedEnemy.h"
#include "Enemy/BlueEnemy.h"
#include "Spawn/EnemySpawner.h"

ABossEnemy::ABossEnemy()
	:
	BossState(EBossState::Wait),
	PlayerLocation_BossRoom(FVector(0, 0, 0)),
	PlayerRotation_Z_BossRoom(0.f),
	FinishCount(0.f),
	//�}�e���A���֌W
	DynamicMaterial_String(nullptr), OpacityValue_String(0.f), MaxOpacity_String(1.f), EyeValueChangeStartingRate(40.f),
	//�A�j���[�V�����֌W
	DeadAnim(nullptr), StanAnim(nullptr), SummonAnim(nullptr), WarpAnim(nullptr), ActProcessingWithDoAnimationChangeDoIs(true),
	//�ҋ@�֌W
	ChangingBossColor(EEnemyColor::White), bHasEndedWait(false), bHasFinishedTransparentize(false), bHasFinishedChangeDecidedColor(false), ColorValue(FLinearColor(0, 0, 0)), bHasFinishedShow(false),
	bIsBattleStarted(false), TimeUpToColorChange(1.5f),
	//�`���[�W�֌W
	ChargeTime(0.f), bIsTransitionAttack(false), bIsTransitionStan(false), CountUpToAttackStateTransition(0), TimeUpToAttackStateTransition(1), ChargeCount(0), CountUpToAttack(5), StanValue(0), 
	MaxStanValue(5 * AssumptionFPS),
	//�X�^���֌W
	bHasEndedStan(false),
	//�ʏ�G�Ăъ֌W
	bHasEndedEnemyCall(false), CallingEnemyNumber(0), EnemyCallTiming(240),
	//�ʏ�G�̓����ҋ@�֌W
	NowExistsEnemyNumber(0),
	//�ʏ�G�̓�����֌W
	bHasEndedAfterEnemyExpedition(false),
	//�ړ��֌W
	BossGoalLocation(FVector(0.f, 0.f, 0.f)), bHasEndedTeleportation(false), DegreeLimit_Min(-60), DegreeLimit_Max(60),
	//���S�֌W
	InRate_Destroy(1.5f)
{
	//Tick��L���ɂ���
	PrimaryActorTick.bCanEverTick = true;

	//�X�P�[����ݒ�
	EnemyScale = FVector(20.f, 20.f, 20.f);

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
		//�X�P�[�����R���|�[�l���g�ɓK��
		DefaultSceneRoot->SetWorldScale3D(EnemyScale);

		//���X�P���^�����b�V���R���|�[�l���g------------------------------------------------------------------------------------------
		//�X�P���^�����b�V���R���|�[�l���g�̍쐬
		this->GhostMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Ghost"));
		//�X�P���^�����b�V�������[�h
		TObjectPtr<USkeletalMesh> GhostMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/_TeamFolder/CG/CG_Model/Boss/ghost_boss_modeling"));
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
			//�R�̃}�e���A�������[�h
			TObjectPtr<UMaterial> StringMaterial = LoadObject<UMaterial>(nullptr, TEXT("/Game/_TeamFolder/Enemy/M_StringColor"));
			if (StringMaterial)
			{
				//�_�C�i�~�b�N�}�e���A���C���X�^���X���쐬
				DynamicMaterial_String = UMaterialInstanceDynamic::Create(StringMaterial, this);

				//GhostMesh�Ƀ_�C�i�~�b�N�}�e���A����ݒ�
				this->GhostMeshComponent->SetMaterial(2, DynamicMaterial_String);

				//�����I�p�V�e�B�l��ݒ�
				DynamicMaterial_String->SetScalarParameterValue(FName("Opacity"), OpacityValue_String);
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
			//GhostCollision�̑��΍��W��ݒ�
			GhostCollision->SetRelativeLocation(FVector(0.f, 0.f, 4.5f));
			//GhostCollision�̔��a��ݒ�
			GhostCollision->SetSphereRadius(5.f);
		}
	}

	//���A�j���[�V����-------------------------------------------------------------------------------------------------------------
	this->DefaultAnim = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/_TeamFolder/CG/CG_Model/Boss/ghost_boss_idle_Anim"));	// ����̃A�j���[�V�������g�p���Ȃ���Ԃ̃A�j���[�V���� ��{���[�v�Đ�����
	this->AttackAnim = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/_TeamFolder/CG/CG_Model/Boss/ghost_boss_attack_Anim"));	// �U����Ԃ̃A�j���[�V����								���[�v�Đ����Ȃ�
	DeadAnim = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/_TeamFolder/CG/CG_Model/Boss/ghost_boss_dead_Anim"));				// ���S��Ԃ̃A�j���[�V����								���[�v�Đ����Ȃ�
	StanAnim = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/_TeamFolder/CG/CG_Model/Boss/ghost_boss_stun_Anim"));				// �X�^����Ԃ̃A�j���[�V����							���[�v�Đ����Ȃ�
	SummonAnim = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/_TeamFolder/CG/CG_Model/Boss/ghost_boss_summon_Anim"));			// �G�Ăю��̃A�j���[�V����								���[�v�Đ����Ȃ�
	WarpAnim = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/_TeamFolder/CG/CG_Model/Boss/ghost_boss_warp_Anim"));				// �u�Ԉړ����̃A�j���|�V����							���[�v�Đ����Ȃ�

	//���T�E���h-------------------------------------------------------------------------------------------------------------------
	AppearSound = LoadObject<USoundCue>(nullptr, TEXT("/Game/_TeamFolder/Sound/SE/SE_BossAppear_Cue"));		//�o�����̉��ݒ�
	DisappearSound = LoadObject<USoundCue>(nullptr, TEXT("/Game/_TeamFolder/Sound/SE/SE_BossDead_Cue"));	//���Ŏ��̉��ݒ�

	//���{�X�G�̐ݒ�---------------------------------------------------------------------------------------------------------------
	this->EnemyColor = EEnemyColor::BossColor;	// �����̐F�ݒ�
	this->AttackTiming = 85;					// �U������̃^�C�~���O�ݒ�(�t���[����)
	this->Status.Score = 1000;
}

void ABossEnemy::BeginPlay()
{
	Super::BeginPlay();

	//�f�t�H���g�̃A�j���[�V�������J�n����
	if (this->DefaultAnim) // null�`�F�b�N
	{
		GhostMeshComponent->PlayAnimation(DefaultAnim, true);
	}
}

void ABossEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	this->FacePlayerHowTo(); // �v���C���[�̕���������

	//�퓬���J�n�����珈�����n�߂�
	if (bIsBattleStarted)
	{
		TickProcess();
	}
}

//----------------------------------------------------------------------------------------
//���ǉ��֐��̏�����
//----------------------------------------------------------------------------------------
//Tick�ł̏���
void ABossEnemy::TickProcess()
{
	//���݂�FPS���擾
	Gamefps = GetWorldFPS();

	//���S���ɂ͍s��Ȃ�����
	if (Status.HP > 0)
	{
		//�����𖳂���
		if (DynamicMaterial_Body && DynamicMaterial_Eye) // null�`�F�b�N
		{
			this->DynamicMaterial_Body->SetScalarParameterValue(FName("Emissive"), 0.f);
			this->DynamicMaterial_Eye->SetScalarParameterValue(FName("Emissive"), 0.f);
		}
	}

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
				/*UKismetSystemLibrary::PrintString(this, TEXT("Change Charge"), true, true, FColor::Red, 2.f);*/
				break;

			case 2:	// �G���G���Ăԏ�Ԃ�
				NowState = EBossState::EnemyCall;
				/*UKismetSystemLibrary::PrintString(this, TEXT("Change EnemyCall"), true, true, FColor::Green, 2.f);*/
				break;

			case 3:	// �u�Ԉړ���Ԃ�
				NowState = EBossState::Teleportation;
				/*UKismetSystemLibrary::PrintString(this, TEXT("Change Teleportation"), true, true, FColor::Blue, 2.f);*/
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
		if (Status.HP <= 0) { NowState = EBossState::Die; }
		break;

	//�ʏ�G�̓�����-------------------------------------------------------------------------
	case EBossState::AfterEnemyExpedition:
		if (bHasEndedAfterEnemyExpedition) { NowState = EBossState::Wait; }	//�ҋ@��Ԃ�
		if (Status.HP <= 0) { NowState = EBossState::Die; }					//���S��Ԃ�
		break;

	//�u�Ԉړ�-------------------------------------------------------------------------------
	case EBossState::Teleportation:
		if (bHasEndedTeleportation) { NowState = EBossState::Wait; }	//�ҋ@��Ԃ�
		if (Status.HP <= 0) { NowState = EBossState::Die; }				//���S��Ԃ�
		break;
	}

	UpdateState(NowState);
}

//��Ԃ̍X�V
void ABossEnemy::UpdateState(EBossState NowState)
{
	if (NowState != this->BossState)
	{
		this->BossState = NowState;
		this->MoveCount = 0;
		this->bShouldBeenProcessWhenFirstStateTransition = false;
		FinishCount = 0;

		//��ԕω����̃A�j���[�V�����ύX
		AnimationChangeAtStateChange(NowState);
	}
}

//��ԂɊ�Â�������������
void ABossEnemy::ActProcess()
{
	switch (this->BossState)
	{
	//�ҋ@-----------------------------------------------------------------------------------
	case EBossState::Wait:
		//��ԁFWait�J�ڎ��ɂ̂ݍs������
		if (this->bShouldBeenProcessWhenFirstStateTransition == false)
		{
			ProcessJustForFirst_Wait();
		}

		//�F�ύX�܂ł̎��Ԃ��o����������
		if (MoveCount >= FMath::RoundToInt(TimeUpToColorChange * Gamefps))
		{
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
		//���S���̓���������܂ł̎��Ԃ��A�j���[�V�����ɍ������l�ɕύX����
		TimeUpToTransparency = 2;

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

//�A�j���[�V�����֌W---------------------------------------------------------------------------------------------
//�A�j���[�V�����̕ύX
void ABossEnemy::AnimationChangeAtStateChange(const EBossState NowState)
{
	//��Ԃɍ��킹���A�j���[�V�����ύX
	if (GhostMeshComponent)
	{
		switch (NowState)
		{
		case EBossState::Attack:
			if (AttackAnim) // null�`�F�b�N
			{
				GhostMeshComponent->PlayAnimation(AttackAnim, false);
			}
			break;

		case EBossState::Stan:
			if (StanAnim)
			{
				GhostMeshComponent->PlayAnimation(StanAnim, false);
			}
			break;

		case EBossState::EnemyCall:
			if (SummonAnim)
			{
				GhostMeshComponent->PlayAnimation(SummonAnim, false);
			}
			break;

		case EBossState::Die:
			if (DeadAnim)
			{
				GhostMeshComponent->PlayAnimation(DeadAnim, false);
			}
			break;

		default:	// ����̃A�j���[�V�������Ȃ��ꍇ
			if (DefaultAnim) // null�`�F�b�N
			{
				GhostMeshComponent->PlayAnimation(DefaultAnim, true);
			}
			break;

		//�A�j���[�V�������~�߂���-------------------------------------------------------------------------
		case EBossState::EnemyExpeditionWait:
			GhostMeshComponent->Stop();
			break;

		//�A�j���[�V�������g�p���Ȃ����-------------------------------------------------------------------------

		}
	}
}

void ABossEnemy::ActProcessingWithAnimationChange(const EBossState NowState)
{
	//��Ԃɍ��킹���A�j���[�V�����ύX
	if (GhostMeshComponent)
	{
		switch (NowState)
		{
		case EBossState::Charge:	// �`���[�W��Ԃ̃e���|�[�g�̃A�j���[�V�����ɕύX
			if (WarpAnim) // null�`�F�b�N
			{
				GhostMeshComponent->PlayAnimation(WarpAnim, false);
			}

			//�`���[�W���I����Ă�����f�t�H���g�̃A�j���[�V�����ɕύX
			if (ChargeCount == CountUpToAttack)
			{
				if (DefaultAnim) // null�`�F�b�N
				{
					GhostMeshComponent->PlayAnimation(DefaultAnim, true);
				}
			}
			break;

		case EBossState::Teleportation:
			if (WarpAnim) // null�`�F�b�N
			{
				GhostMeshComponent->PlayAnimation(WarpAnim, false);
			}
			break;
		}
	}
	

	ActProcessingWithDoAnimationChangeDoIs = false;
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

	//���̏�ԂɑJ�ڂ��Ȃ��悤�ɂ���
	bHasEndedWait = false;

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
	if (DynamicMaterial_Body && DynamicMaterial_Eye && DynamicMaterial_String) // null�`�F�b�N
	{
		//�̂̃I�p�V�e�B�̒l���v�Z
		OpacityValue_Body -= MaxOpacity_Body / TimeUpToTransparency * DeltaTime;			// �̂̃I�p�V�e�B�̌v�Z
		/*OpacityValue_String -= MaxOpacity_String / TimeUpToTransparency * DeltaTime;*/	// �R�̃I�p�V�e�B�̌v�Z

		//�ځE���̃I�p�V�e�B�̒l���v�Z(�����ɓ���������ƌ�����̂������Ă��܂�)
		if (OpacityValue_Body <= MaxOpacity_Body / EyeValueChangeStartingRate)
		{
			OpacityValue_Eye -= MaxOpacity_Eye / (TimeUpToTransparency / EyeValueChangeStartingRate) * DeltaTime;	// �ځE���̃I�p�V�e�B�̌v�Z
		}

		//�o�����I������珈�����I������
		if (OpacityValue_Body <= 0.f && OpacityValue_Eye <= 0.f)
		{
			//�I�p�V�e�B�̒l��0�������Ȃ��悤�ɂ���
			OpacityValue_Body = 0.f;
			OpacityValue_Eye = 0.f;
			/*OpacityValue_String = 0.f;*/

			//�I�p�V�e�B��ݒ�
			DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Body);
			DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Eye);
			/*DynamicMaterial_String->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_String);*/

			//�����蔻��𖳂���
			if (GhostCollision) // null�`�F�b�N
			{
				GhostCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}

			//���̊֐����Ă΂�Ȃ��悤�ɂ���
			return true;
		}

		//�I�p�V�e�B��ݒ�
		DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Body);
		DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Eye);
		/*DynamicMaterial_String->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_String);*/
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
		OpacityValue_Body += MaxOpacity_Body / TimeSpentInAppear * DeltaTime;								// �̂̃I�p�V�e�B�̌v�Z
		OpacityValue_Eye += MaxOpacity_Eye / (TimeSpentInAppear / EyeValueChangeStartingRate) * DeltaTime;	// �ځE���̃I�p�V�e�B�̌v�Z
		/*OpacityValue_String += MaxOpacity_String / TimeUpToTransparency * DeltaTime;*/					// �R�̃I�p�V�e�B�̌v�Z

		//�o�����I������珈�����I������
		if (OpacityValue_Body >= MaxOpacity_Body && OpacityValue_Eye >= MaxOpacity_Eye)
		{
			//�I�p�V�e�B�̒l��1�𒴂��Ȃ��悤�ɂ���
			OpacityValue_Body = MaxOpacity_Body;
			OpacityValue_Eye = MaxOpacity_Eye;
			/*OpacityValue_String = MaxOpacity_String;*/

			//�I�p�V�e�B��ݒ�
			DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Body);
			DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Eye);
			/*DynamicMaterial_String->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_String);*/

			//�����蔻���t����
			if (GhostCollision)
			{
				GhostCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}

			//��ԑJ�ډ\�ɂ���
			return true;
		}

		//�I�p�V�e�B��ݒ�
		DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Body);
		DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Eye);
		/*DynamicMaterial_String->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_String);*/
	}

	//������x���̊֐����Ă�
	return false;
}

//�{�X�̐퓬�s�����J�n������
void ABossEnemy::BattleStart()
{
	bIsBattleStarted = true;

	//�퓬�J�n�̐���炷
	PlayAppearSound();
}

//����ԁFCharge�̏���------------------------------------------------------------------------------------------- 
//��ԁFCharge�J�ڎ��ɂ̂ݍs������ 
void ABossEnemy::ProcessJustForFirst_Charge()
{
	//�J�ڐ�����߂�ϐ���false�ɂ���
	bIsTransitionAttack = false;	// �U����ԂɑJ�ڂ��邩
	bIsTransitionStan = false;		// �X�^����ԂɑJ�ڂ��邩

	//�`���[�W�񐔂�0�ɂ���
	ChargeCount = 0;

	//�{�X�̏o�����W���N���A����
	BossGoalLocations.Empty();

	//���݈ʒu���擾
	this->CurrentLocation = GetActorLocation();

	//�{�X�̏u�Ԉړ�������W��ݒ肷��
	for (int i = 0; i < CountUpToAttack; ++i)
	{
		//�{�X�̈ړ��ʒu�����߂郉�W�A���̒l��ݒ�
		float GoalDegrees = PlayerRotation_Z_BossRoom + FMath::FRandRange(DegreeLimit_Min, DegreeLimit_Max);	// ���x�ɂ��邩�v�Z
		float GoalRadians = FMath::DegreesToRadians(GoalDegrees);												// ���W�A���ɕϊ�

		//�{�X�̈ړ��ʒu�����߂�
		float GoalX = (PlayerLocation_BossRoom.X + 500) * FMath::Sin(GoalRadians);
		float GoalY = (PlayerLocation_BossRoom.Y + 500) * FMath::Cos(GoalRadians);
		float GoalZ = CurrentLocation.Z;

		//�{�X�̏o���ʒu��ݒ�
		BossGoalLocations.Add(FVector(GoalX, GoalY, GoalZ));
	}

	//�A�j���[�V�������n�܂�悤�ɂ���
	ActProcessingWithDoAnimationChangeDoIs = true;

	//���̊֐���������Ă΂�Ȃ��悤�ɂ���
	this->bShouldBeenProcessWhenFirstStateTransition = true;
}

//�U���`���[�W�̏���
void ABossEnemy::ChargeAttack()
{
	//�`���[�W���I����Ă��Ȃ�������
	if (ChargeCount < CountUpToAttack)
	{
		if (ActProcessingWithDoAnimationChangeDoIs)
		{
			//�e���|�[�g�̃A�j���[�V����
			ActProcessingWithAnimationChange(BossState);
		}

		//�A�j���[�V�������I��������u�Ԉړ����Ȃ���`���[�W���s���������s��
		if (!GhostMeshComponent->IsPlaying())
		{
			Teleportation_Charge();
		}
	}

	//�`���[�W������������
	if (ChargeCount == CountUpToAttack)
	{
		//�ҋ@�̃A�j���[�V�����ɕύX
		ActProcessingWithAnimationChange(BossState);

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

	//
	ActProcessingWithDoAnimationChangeDoIs = true;
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
	if (GhostMeshComponent && AttackAnim) // null�`�F�b�N
	{
		//�U������
		if (MoveCount == FMath::RoundToInt(AttackTiming * Gamefps / AssumptionFPS))
		{
			/*UKismetSystemLibrary::PrintString(this, TEXT("BossEnemy Attack!"), true, true, FColor::Yellow, 2.f, TEXT("None"));*/

			//�v���C���[�̏��擾
			TObjectPtr<AVRPlayerCharacter> Player = Cast<AVRPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
			//�v���C���[�փ_���[�W��^����
			if (Player)
			{
				Player->RecievePlayerDamage();
			}
		}
		//�A�j���[�V�������I�������U����ԏI��
		if (!GhostMeshComponent->IsPlaying())
		{
			return true;
		}
	}
	else // ���f���A�A�j���[�V������null�ȏꍇ�U�������I������
	{
		return true;
	}

	//������x���̊֐����Ă�
	return false;
}

//����ԁFStan�̏���---------------------------------------------------------------------------------------------
//�X�^������
bool ABossEnemy::Stan()
{
	if (GhostMeshComponent && StanAnim) // null�`�F�b�N
	{
		//�A�j���[�V�������I�������ҋ@��Ԃɖ߂�
		if (!GhostMeshComponent->IsPlaying())
		{
			//�X�^���l��0�ɂ���
			StanValue = 0;

			//�X�^����ԏI��
			return true;
		}
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
		NormalEnemyPopLocations.Add(FVector(300.f, 0.f, 0.f));		// �{�X����E��300���j�b�g�̈ʒu
		NormalEnemyPopLocations.Add(FVector(-300.f, 0.f, 0.f));		// �{�X���獶��300���j�b�g�̈ʒu
		break;

	case 3:		//3��
		NormalEnemyPopLocations.Add(FVector(300.f, 0.f, -50.f));	// �{�X����E��300���j�b�g�A����50���j�b�g�̈ʒu
		NormalEnemyPopLocations.Add(FVector(-300.f, 0.f, -50.f));	// �{�X���獶��300���j�b�g�A����50���j�b�g�̈ʒu
		NormalEnemyPopLocations.Add(FVector(0.f, 0.f, 100.f));		// �{�X������100���j�b�g�̈ʒu
		break;

	case 4:		//4��
		NormalEnemyPopLocations.Add(FVector(300.f, 0.f, -100.f));	// �{�X����E��300���j�b�g�A����100���j�b�g�̈ʒu
		NormalEnemyPopLocations.Add(FVector(-300.f, 0.f, -100.f));	// �{�X���獶��300���j�b�g�A����100���j�b�g�̈ʒu
		NormalEnemyPopLocations.Add(FVector(300.f, 0.f, 100.f));	// �{�X����E��300���j�b�g�A���100���j�b�g�̈ʒu
		NormalEnemyPopLocations.Add(FVector(-300.f, 0.f, 100.f));	// �{�X���獶��300���j�b�g�A���100���j�b�g�̈ʒu
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

	//�A�j���[�V�����ɍ��킹�ēG�̐������s��
	if (MoveCount == FMath::RoundToInt(EnemyCallTiming * Gamefps / AssumptionFPS))
	{
		//�ʏ�G�̐���
		CreateEnemies(CallingEnemyNumber, NormalEnemyPopLocations, CallingEnemyColors);
	}

	//�A�j���[�V�������I��������
	if (!GhostMeshComponent->IsPlaying())
	{
		//����������(Wait�ł̓����������𗬗p)
		if (bHasFinishedTransparentize == false)
		{
			bHasFinishedTransparentize = Transparentize(DeltaTime);
		}
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

			//�o������G��|����܂ł̏Ǝ˕b��
			float DefeatUpToSeconds = 0.f;

			//���߂��F�̓G�𐶐�����
			switch (EnemyColors[i])
			{
			case EEnemyColor::White:	//��
				CallingEnemy = GetWorld()->SpawnActor<AEnemys>(ANormalEnemy::StaticClass(), FVector(0.f, 0.f, 0.f), FRotator(0.f, 0.f, 0.f));
				break;

			case EEnemyColor::Green:	//��
				DefeatUpToSeconds = 2.f;
				CallingEnemy = GetWorld()->SpawnActor<AEnemys>(AGreenEnemy::StaticClass(), FVector(0.f, 0.f, 0.f), FRotator(0.f, 0.f, 0.f));
				CallingEnemy->SetHP(AssumptionFPS * 10 * DefeatUpToSeconds);							// HP�̐ݒ�@10�̓f�t�H���g�̃v���C���[�̍U����
				CallingEnemy->SetTimeFromWaitToStateTransition(CallingEnemyNum * DefeatUpToSeconds);	// �U���܂ł̎��Ԃ̐ݒ�
				break;

			case EEnemyColor::Red:		//��
				DefeatUpToSeconds = 2.f;
				CallingEnemy = GetWorld()->SpawnActor<AEnemys>(ARedEnemy::StaticClass(), FVector(0.f, 0.f, 0.f), FRotator(0.f, 0.f, 0.f));
				CallingEnemy->SetHP(AssumptionFPS * 10 * DefeatUpToSeconds);							// HP�̐ݒ�@10�̓f�t�H���g�̃v���C���[�̍U����
				CallingEnemy->SetTimeFromWaitToStateTransition(CallingEnemyNum * DefeatUpToSeconds);	// �U���܂ł̎��Ԃ̐ݒ�
				break;

			case EEnemyColor::Blue:		//��
				DefeatUpToSeconds = 3.5f;
				CallingEnemy = GetWorld()->SpawnActor<AEnemys>(ABlueEnemy::StaticClass(), FVector(0.f, 0.f, 0.f), FRotator(0.f, 0.f, 0.f));
				CallingEnemy->SetHP(AssumptionFPS * 10 * DefeatUpToSeconds);
				break;
			}

			//�{�X�G��e�Ƃ��A�{�X�G����̑��Έʒu�ŏo���ʒu��ݒ肷��
			if (CallingEnemy) // �G�C���X�^���X����������Ă�����
			{
				//�{�X�A�N�^�[��e�Ƃ���
				CallingEnemy->AttachToActor(BossEnemy, FAttachmentTransformRules::KeepWorldTransform);

				//�e�̈ʒu�Ɖ�]���擾
				FVector		BossPosition = GetActorLocation();	// �{�X�G�̌��݂̈ʒu
				FRotator	BossRotation = GetActorRotation();	// �{�X�G�̌��݂̉�]

				//���Έʒu���v�Z
				FVector NewLocation = BossPosition + BossRotation.RotateVector(PopLocations[i]);

				//���Έʒu��ݒ�
				CallingEnemy->SetActorLocation(NewLocation);
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
	float GoalDegrees = PlayerRotation_Z_BossRoom + FMath::FRandRange(DegreeLimit_Min, DegreeLimit_Max);	// ���x�ɂ��邩�v�Z
	float GoalRadians = FMath::DegreesToRadians(GoalDegrees);						// ���W�A���ɕϊ�

	//�{�X�̈ړ��ʒu�����߂�
	float GoalX = (PlayerLocation_BossRoom.X + 500) * FMath::Sin(GoalRadians);
	float GoalY = (PlayerLocation_BossRoom.Y + 500) * FMath::Cos(GoalRadians);
	float GoalZ = CurrentLocation.Z;

	//�{�X�̏o���ʒu��ݒ�
	BossGoalLocation = FVector(GoalX, GoalY, GoalZ);

	//�A�j���[�V�������n�܂�悤�ɂ���
	ActProcessingWithDoAnimationChangeDoIs = true;

	//���̊֐���������Ă΂�Ȃ��悤�ɂ���
	this->bShouldBeenProcessWhenFirstStateTransition = true;
}

//�u�Ԉړ�
bool ABossEnemy::Move()
{
	//�e���|�[�g�̃A�j���[�V����
	if (ActProcessingWithDoAnimationChangeDoIs)
	{
		ActProcessingWithAnimationChange(BossState);
	}

	//�A�j���[�V�������I��������
	if (!GhostMeshComponent->IsPlaying())
	{
		//�u�Ԉړ�����
		SetActorLocation(this->BossGoalLocation);

		return true;
	}
	
	return false;
}

//�����S�֌W-----------------------------------------------------------------------------------------------
//���S���̏��X�ɓ����ɂ��鏈��
void ABossEnemy::EnemyDead()
{
	//EnemyDead�ň�x�����s������
	if (bOnceDoProcessBeenIs == false)
	{
		ProcessDoOnce_EnemyDead();
	}

	//���X�ɓ����ɂ���
	bIsDestroy = Transparentize_Dead();

	//�G�����ł�����
	if (bIsDestroy)
	{
		if (FinishCount == 0)
		{
			//�Z�b�g�^�C�}�[�֐���2�x�ʂ�Ȃ��悤�ɂ���
			FinishCount++;

			//���S��n�b�x�点�Ă���A�N�^�[�̏��łƂ���Ɠ����ɍs��������������
			GetWorld()->GetTimerManager().SetTimer(DestroyTimerHundle, this, &ABossEnemy::ProcessAtDestroy, InRate_Destroy);

			//�f�o�b�O
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("Set Timer"));
		}
	}
}

//���S���̏��X�ɓ����ɂ��鏈��
bool ABossEnemy::Transparentize_Dead()
{
	//DeltaTime�̎擾
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	if (DynamicMaterial_Body && DynamicMaterial_Eye && DynamicMaterial_String) // null�`�F�b�N
	{
		//�̂̃I�p�V�e�B�̒l���v�Z
		OpacityValue_Body -= MaxOpacity_Body / TimeUpToTransparency * DeltaTime;			// �̂̃I�p�V�e�B�̌v�Z
		/*OpacityValue_String -= MaxOpacity_String / TimeUpToTransparency * DeltaTime;*/	// �R�̃I�p�V�e�B�̌v�Z

		//�ځE���̃I�p�V�e�B�̒l���v�Z(�����ɓ���������ƌ�����̂������Ă��܂�)
		if (OpacityValue_Body <= MaxOpacity_Body / EyeValueChangeStartingRate)
		{
			OpacityValue_Eye -= MaxOpacity_Eye / (TimeUpToTransparency / EyeValueChangeStartingRate) * DeltaTime;	// �ځE���̃I�p�V�e�B�̌v�Z
		}

		//�o�����I������珈�����I������
		if (OpacityValue_Body <= 0.f && OpacityValue_Eye <= 0.f)
		{
			//�I�p�V�e�B�̒l��0�������Ȃ��悤�ɂ���
			OpacityValue_Body = 0.f;
			OpacityValue_Eye = 0.f;
			/*OpacityValue_String = 0.f;*/

			//�I�p�V�e�B��ݒ�
			DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Body);
			DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Eye);
			/*DynamicMaterial_String->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_String);*/

			//�����蔻��𖳂���
			if (GhostCollision) // null�`�F�b�N
			{
				GhostCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}

			//���̊֐����Ă΂��悤�ɂ���
			return true;
		}

		//�I�p�V�e�B��ݒ�
		DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Body);
		DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Eye);
		/*DynamicMaterial_String->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_String);*/
	}

	//������x���̊֐����Ă�
	return false;
}

// ���Ŏ��ɍs������
void ABossEnemy::ProcessAtDestroy()
{
	//�X�|�i�[���擾
	TObjectPtr<AEnemySpawner> Spawner = Cast<AEnemySpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnemySpawner::StaticClass()));
	if (Spawner)
	{
		Spawner->HandleEnemyCountZero(true);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Spawner Null"));
	}

	//���ł�����
	this->Destroy();
}

//�_���[�W���󂯂鏈���A�����ł�������U���͕��̗͂����炷
void ABossEnemy::RecieveEnemyDamage(int DamageAmount)
{
	if (BossState != EBossState::EnemyExpeditionWait && this->State != EState::Die)
	{
		Status.HP -= DamageAmount;

		this->ChangeEmissiveValue();
	}
}

//�v���C���[�̃��C�g�̐F�ƓG�̃��C�g�̐F���`�F�b�N����֐�
bool ABossEnemy::CheckPlayerLightColor(EFlashlight_Color PlayerColor) const
{
	return (int)PlayerColor == (int)this->EnemyColor;
}

/*����*/
/*�]�T����������A�e��ԂɑJ�ڂ���ׂ�bool�^�ϐ����ЂƂ܂Ƃ߂ɂ���*/