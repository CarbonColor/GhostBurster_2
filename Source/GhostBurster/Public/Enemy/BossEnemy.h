// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemys.h"
#include "Components/SphereComponent.h"
#include "BossEnemy.generated.h"

/**
 * 
 */
UCLASS()
class GHOSTBURSTER_API ABossEnemy : public AEnemys
{
	GENERATED_BODY()
	
	//���񋓌^
	//�G�̏��
	enum class EBossState
	{
		//�ҋ@����J�ڂ�����--------------------------------------------------------------------------------
		Wait = 0,				//�ҋ@(�������̍s�ɗv�f�𑝂₳�Ȃ�)
		Charge = 1,				//�`���[�W
		EnemyCall = 2,				//�G���G���Ă�
		Teleportation = 3,				//�u�Ԉړ�
		MaxStateCountAtTransitionFromWait = 4 UMETA(Hidden),	//�ҋ@����J�ڂ����Ԃ̎�ނ̐�(���擾�p)
		//�ҋ@����J�ڂ��Ȃ�----------------------------------------------------------------------------------
		Attack,					//�U��
		Stan,					//�X�^��
		EnemyExpeditionWait,	//�ʏ�G�̓����ҋ@
		AfterEnemyExpedition,	//�ʏ�G�̓�����
		Appear,					//�o��
		Die,					//���S
	};
	
	//���ϐ�
	//�G�̏��
	EBossState BossState;
	
	//�{�X�����ł̃v���C���[�̈ʒu�Ɖ�]
	FVector		PlayerLocation_BossRoom;	// �{�X�����ł̃v���C���[�̈ʒu
	float		PlayerRotation_Z_BossRoom;	// �{�X�����ł̃v���C���[�̉�]

	//�������I��������܂ł̃J�E���g
	int	FinishCount;	// �I���܂ł̃J�E���g�A�g������K�����̊֐����Ń[���N���A����

	//���֐�
	//Tick�ł̏���
	virtual void TickProcess() override;
	
	//�G�l�~�[�̏�Ԕ��f
	virtual void Think() override;

	//��Ԃ̍X�V
	void UpdateState(EBossState nowState);

	//��ԂɊ�Â�������������
	virtual void ActProcess() override;

	//1�b�J�E���g������true��Ԃ�
	bool bOneSecondsPassedIs();

	//�}�e���A���֌W------------------------------------------------------------------------------------------
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial_String; // �R�̃_�C�i�~�b�N�}�e���A��
	float								 OpacityValue_String;	 // �R�̃I�p�V�e�B�̒l
	float								 MaxOpacity_String;		 // �R�̃I�p�V�e�B�̍ő�l(0�`1�͈̔�)

	//�A�j���[�V�����֌W--------------------------------------------------------------------------------------
	//���ϐ�
	TObjectPtr<UAnimSequence> DeadAnim;				// ���S��Ԃ̃A�j���[�V����
	TObjectPtr<UAnimSequence> StanAnim;				// �X�^����Ԃ̃A�j���[�V����
	TObjectPtr<UAnimSequence> SummonAnim;			// �G�Ăю��̃A�j���[�V����
	TObjectPtr<UAnimSequence> WarpAnim;				// �u�Ԉړ��A�G�������鎞�̃A�j���|�V����

	//���֐�
	void ChangeBossAnimation(const EBossState PreState, const EBossState NewState);	// ��ԑJ�ڎ��̃A�j���[�V�����̕ύX

	//�ҋ@�֌W------------------------------------------------------------------------------------------------
	//���ϐ�
	EEnemyColor		ChangingBossColor;										// ���ɕω�����F
	bool			bHasEndedWait;											// �ҋ@��Ԃ��I�����邩
	bool			bHasFinishedTransparentize;								// �����ɂȂ�����
	bool			bHasFinishedChangeDecidedColor;							// �F���ω�������
	FLinearColor	ColorValue;												// �F�̒l
	bool			bHasFinishedShow;										// �\�����I�������
	bool			bIsBattleStarted;										// �퓬���J�n������

	//���֐�
	void			ProcessJustForFirst_Wait();								// ��ԁFWait�J�ڎ��ɂ̂ݍs������
	bool			ChangeColor(const EEnemyColor ChangingColor);			// �F�̕ύX����
	bool			Transparentize(const float DeltaTime);					// �����ɂ��鏈��
	bool			ChangeDecidedColor(const EEnemyColor ChangingColor);	// ���߂��F�ɕύX����
	bool			Show(const float DeltaTime);							// �\�����鏈��
	void			BattleStart();											// �{�X�̐퓬�s�����J�n������

	//�`���[�W�֌W-------------------------------------------------------------------------------------------
	//���ϐ�
	float	ChargeTime;						// �`���[�W����
	bool	bIsTransitionAttack;			// �U����ԂɑJ�ڂ��邩
	bool	bIsTransitionStan;				// �X�^����ԂɑJ�ڂ��邩
	int		CountUpToAttackStateTransition;	// ��ԁFAttack�ɑJ�ڂ���܂ł̃J�E���g
	int		TimeUpToAttackStateTransition;	// ��ԁFAttack�ɑJ�ڂ���܂ł̎���
	int		ChargeCount;					// ���݂̃`���[�W��
	int		CountUpToAttack;				// �U���܂ł̃`���[�W��
	int		StanValue;						// ���݂̃{�X�̃X�^���l(�X�^����Ԃ̏����ł��g�p)
	int		MaxStanValue;					// �X�^���l�̍ő�l(�X�^����Ԃ̏����ł��g�p)

	//���֐�
	void	ProcessJustForFirst_Charge();	// ��ԁFCharge�J�ڎ��ɂ̂ݍs������
	void	ChargeAttack();					// �U���`���[�W�̏���
	void	Teleportation_Charge();			// �u�Ԉړ����Ȃ���`���[�W���s������

	//�U���֌W----------------------------------------------------------------------------------------------
	//���֐�
	void		 ProcessJustForFirst_Attack();	// ��ԁFAttack�J�ڎ��ɂ̂ݍs������
	virtual bool Attack() override;				// �U������

	//�X�^���֌W---------------------------------------------------------------------------------------------
	//���ϐ�
	bool bHasEndedStan;	// �X�^����Ԃ��I�����邩

	//���֐�
	bool Stan();		// �X�^������

	//�ʏ�G�Ăъ֌W-----------------------------------------------------------------------------------------
	//���ϐ�
	TArray<FVector>		NormalEnemyPopLocations;	// �G�̏o���ꏊ
	bool				bHasEndedEnemyCall;			// �ʏ�G�Ăя�Ԃ��I�����邩
	int					CallingEnemyNumber;			// �ʏ�G�̐�����
	TArray<EEnemyColor>	CallingEnemyColors;			// ��������G�̐F

	//���֐�
	void				ProcessJustForFirst_EnemyCall();																						// ��ԁFEnemyCall�J�ڎ��ɂ̂ݍs������
	void				DecideCallingEnemyNumber();																								// �ʏ�G�̐����������߂�(2�ȏ�4�ȉ��͈̔�)
	void				DecideEnemyPopLocations(const int CallingEnemyNum);																		// �ʏ�G�̏o���ʒu�����߂�(�{�X�G����̑��Έʒu)
	void				DecideCreateEnemyColor(const int CallingEnemyNum);																		// ���F�̓G����������邩���߂�
	bool				EnemyCall();																											// �ʏ�G�Ăя���
	void				CreateEnemies(const int CallingEnemyNum, const TArray<FVector>& PopLocations, const TArray<EEnemyColor>& EnemyColors);	// �ʏ�G�̐�������

	//�ʏ�G�̓����ҋ@�֌W-----------------------------------------------------------------------------------
	//���ϐ�
	int		NowExistsEnemyNumber;												// ���ݑ��݂��Ă���Ă񂾓G�̐��A��̓|���ꂽ��1����

	//���֐�
public:
	void	ReceiveInfoDeadCalledEnemy();										// �Ă񂾓G�����ł����Ƃ��A���̏��ł����Ƃ��������󂯎��(NowExistsEnemyNumber--����)

private:
	//�ʏ�G�̓�����֌W-------------------------------------------------------------------------------------
	//���ϐ�
	bool bHasEndedAfterEnemyExpedition;	// �ʏ�G�̓������Ԃ̏������I�����邩

	//���֐�
	void AfterEnemyExpeditionProcess();	// �ʏ�G�̓������Ԃōs������

	//�ړ��֌W-----------------------------------------------------------------------------------------------
	//���ϐ� 
	TArray<FVector> BossGoalLocations;						// �{�X�̕����ӏ��̖ړI���W
	FVector			BossGoalLocation;						// �{�X�̖ړI���W
	bool			bHasEndedTeleportation;					// �u�Ԉړ����I�������true 

	//���֐�
	virtual void	ProcessJustForFirst_Move() override;	// ���Move�J�ڎ��ɂ̂ݍs������
	virtual bool	Move() override;						// �ړ�

protected:
	virtual void BeginPlay() override;

public:
	//�R���X�g���N�^
	ABossEnemy();

	//Tick
	virtual void Tick(float DeltaTime) override;

	//���֐��錾
	//�_���[�W���󂯂鏈���A�����ł�������U���͕��̗͂����炷
	virtual void RecieveEnemyDamage(int DamageAmount) override;

	//�v���C���[�̃��C�g�̐F�ƓG�̃��C�g�̐F���`�F�b�N���鏈��
	virtual bool CheckPlayerLightColor(EFlashlight_Color PlayerColor) const override;

	//�v���C���[���{�X�����Ŏ~�܂������A�ʒu�Ɖ�]���擾����
	void GetPlayerLocationAndRotation(const FVector PlayerLocation, const float PlayerRotation_Z);
};
