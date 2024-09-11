// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/DamageInterface.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

#include "Enemys.generated.h"

UCLASS(Abstract)
class GHOSTBURSTER_API AEnemys : public AActor, public IDamageInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemys();

	//���z�f�X�g���N�^
	virtual ~AEnemys() {}
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//���\����
	//�X�e�[�^�X
	struct FStatus
	{
		FStatus() : HP(60), Speed(0.f) {}

		int		HP;		// �S�[�X�g�̗̑�
		float	Speed;	// �ړI�n�܂ł̈ړ����x
	};

	//���񋓌^
	//�G�̏��
	enum class EState
	{
		Wait,	//�ҋ@
		Move,	//�ړ�
		Attack,	//�U��
		Die,	//���S
		Appear,	//�o��
	};

	enum class EEnemyColor : uint8
	{
		White = 0,
		Green = 1,
		Red = 2,
		Blue = 3,
	};

	//���ϐ�
	//�G�̍s������p�̃J�E���g
	int MoveCount;

	//��x�����s��������������
	bool bOnceDoProcessBeenIs;

	//���֐�
	//Tick�ł̏���
	virtual void TickProcess() PURE_VIRTUAL(AEnemys::TickProcess, );

	//�G�l�~�[�̏�Ԕ��f
	virtual void Think() PURE_VIRTUAL(AEnemys::Think, );

	//��Ԃ̍X�V
	void UpdateState(EState NowState);

	//��ԂɊ�Â�������������
	virtual void ActProcess() PURE_VIRTUAL(AEnemys::ActProcess, );

	//FPS�֌W----------------------------------------------------------------------------------------------------------------------
	//���ϐ�
	float	Gamefps;		// �Q�[����fps���l��ݒ�
	int		AssumptionFPS;	// �z��FPS

	//���֐�
	float GetWorldFPS();	// ���݂�FPS���擾����

	//�\���̕ϐ�-------------------------------------------------------------------------------------------------------------------
	FStatus Status;

	//�񋓌^�ϐ�-------------------------------------------------------------------------------------------------------------------
	EState State;
	EEnemyColor EnemyColor;

	//�R���|�[�l���g�֌W-----------------------------------------------------------------------------------------------------------
	//���ϐ�
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent>				DefaultSceneRoot;		// �V�[���R���|�[�l���g
	UPROPERTY(EditAnywhere)
	TObjectPtr<USkeletalMeshComponent>		GhostMeshComponent;		// �X�P���^�����b�V���R���|�[�l���g
	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent>			GhostCollision;			// �R���W����
	TObjectPtr<UMaterialInstanceDynamic>	DynamicMaterial_Body;	// �̂̃_�C�i�~�b�N�}�e���A��
	TObjectPtr<UMaterialInstanceDynamic>	DynamicMaterial_Eye;	// �ڂ̃_�C�i�~�b�N�}�e���A��

	//�A�j���[�V�����֌W-----------------------------------------------------------------------------------------------------------
	//���ϐ�
	TObjectPtr<UAnimSequence>	DefaultAnim;	// ����̃A�j���[�V�������g�p���Ȃ���Ԃ̃A�j���[�V����
	TObjectPtr<UAnimSequence>	AttackAnim;		// �U����Ԃ̃A�j���[�V����
	int							AttackTiming;	// �U���̃^�C�~���O(�t���[��)�A�A�j���[�V�����ɍ��킹�Đ��l�����߂�

	void ChangeAnimation(const EState PreState, const EState NewState);	// �A�j���[�V�����̕ύX

	//�T�E���h�֌W-----------------------------------------------------------------------------------------------------------------
	//���ϐ�
	TObjectPtr<USoundCue> AppearSound;		// �G�o�����̉�
	TObjectPtr<USoundCue> DisappearSound;	// �G���Ŏ��̉� 

	//���֐�
	void PlayAppearSound();					// �G�o�����̉���炷
	void PlayDisappearSound();				// �G���Ŏ��̉���炷

	//�ړ��֌W---------------------------------------------------------------------------------------------------------------------
	//���ϐ�
	float			MoveTime;			// �S�[�X�g�̈ړ��ɂ����鎞��(�b)
	float			TraveledDistance;	// ����܂łɐi�񂾋���
	FVector			CurrentLocation;	// �G�̌��݂̍��W
	TArray<FVector> GoalLocations;		// �G�̈ړ�����W(���[�J�����W�A�����͍l�����Ȃ�)
	int				MovingTimesCount;	// ����ڂ̈ړ����A�P��ڂ�0�Ƃ���(�ړ���ԂɑJ�ڂ����񐔂ŃJ�E���g)
	FVector			GoalLocation_World;	// �G�̈ړ�����W(���[���h���W)
	FVector			Direction;			// GoalLocation�֌������P�ʃx�N�g��
	float			TotalDistance;		// �J�n�ʒu����ړI�n�܂ł̒�������
	float			Amplitude;			// �U��
	float			Frequency;			// �g�̑���
	bool			bHasEndedMoving;	// �ړ����I��������

	//���֐�
	virtual void ProcessJustForFirst_Move() PURE_VIRTUAL(AEnemys::ProcessJustForFirst_Move, );	// ���Move�J�ڎ��ɂ̂ݍs������
	virtual bool Move() PURE_VIRTUAL(AEnemys::Move, return false;);								// �ړ�����

	//�U���֌W---------------------------------------------------------------------------------------------------------------------
	//���ϐ�
	bool	bHasEndedAttack;											// �U�����I��������
	float	AttackUpToTime;												// �S�[�X�g�̍U���܂ł̎���(�b)

	//���֐�
	virtual bool Attack() PURE_VIRTUAL(AEnemys::Move, return false;);	// �U������

	//���S�֌W---------------------------------------------------------------------------------------------------------------------
	//���ϐ�
	bool bIsDestroy;	// ���X�ɓ����ɂ��鏈�����I�����邩

	//���֐�
	void EnemyDead();				// HP��0�ɂȂ�������ł�����
	void ProcessDoOnce_EnemyDead();	// EnemyDead�ň�x�����s������
	bool Transparentize_Dead();		// ���S���̏��X�ɓ����ɂ��鏈��

	//�o���֌W---------------------------------------------------------------------------------------------------------------------
	//���ϐ� 
	bool	bHasEndedAppear;	// �o�����I��������
	float	OpacityValue;		// �I�p�V�e�B�̒l
	int		TimeSpentInAppear;	// �o������̂ɂ����鎞��

	//���֐�
	void ProcessJustForFirst_Appear();	// ��ԁFAppear�ōŏ��Ɉ�x�������鏈��
	bool Appear();						// �G�o������

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Setter�֐�-------------------------------------------------------------------------------------------------------------------
	void SetHP(const int HPValue);								// HP�̐ݒ�p�֐�
	void SetAttackUpToTime(const float SetTime);				// �U����ԂɂȂ�܂ł̎��Ԑݒ�p�֐�
	void SetGoalLocations(const TArray<FVector>& SetLocations);	// �ڕW���W�̐ݒ�p�֐�
	void SetMoveTime(const float SetTime);						// �ړ����Ԃ̐ݒ�p
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void SetInitialData(const int HP, const float AttackUpToTimeValue, const TArray<FVector>& SetLocations, const float MoveTimeValue);	// �������ꂽ�Ƃ��̐ݒ�p�֐�
	virtual bool CheckPlayerLightColor(EFlashlight_Color PlayerColor) const PURE_VIRTUAL(AEnemys::GetEnemyColor, return false;);		// �v���C���[�̃��C�g�̐F�ƓG�̃��C�g�̐F���`�F�b�N����֐�
};