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
		FStatus() : HP(1) {}

		int HP;	//�S�[�X�g�̗̑�
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

	//�Q�[����fps���l��ݒ�
	float Gamefps;

	//�G����ԑJ�ڂ����Ƃ��ɍŏ��ɍs���������s������true
	bool bShouldBeenProcessWhenFirstStateTransition;

	//���֐�
	//Tick�ł̏���
	virtual void TickProcess() PURE_VIRTUAL(AEnemys::TickProcess, );

	//�G�l�~�[�̏�Ԕ��f
	virtual void Think() PURE_VIRTUAL(AEnemys::Think, );

	//��Ԃ̍X�V
	void UpdateState(EState NowState);

	//��ԂɊ�Â�������������
	virtual void ActProcess() PURE_VIRTUAL(AEnemys::ActProcess, );

	//���݂�FPS���擾����
	float GetWorldFPS();

	//�\���̕ϐ�-------------------------------------------------------------------------------------------------------------------
	FStatus Status;

	//�񋓌^�ϐ�-------------------------------------------------------------------------------------------------------------------
	EState State;
	EEnemyColor EnemyColor;

	//�R���|�[�l���g�֌W-----------------------------------------------------------------------------------------------------------
	//���ϐ�
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent>				DefaultSceneRoot;	// �V�[���R���|�[�l���g
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent>		GhostMesh;			// �X�^�e�B�b�N���b�V���R���|�[�l���g
	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent>			GhostCollision;		// �R���W����
	TObjectPtr<UMaterialInstanceDynamic>	DynamicMaterial;	// �_�C�i�~�b�N�}�e���A��
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent>			SplineComponent;	// �X�v���C���R���|�[�l���g

	//�T�E���h�֌W-----------------------------------------------------------------------------------------------------------------
	//���ϐ�
	TObjectPtr<USoundCue> AppearSound;		// �G�o�����̉�
	TObjectPtr<USoundCue> DisappearSound;	// �G���Ŏ��̉� 

	//���֐�
	void PlayAppearSound();					// �G�o�����̉���炷
	void PlayDisappearSound();				// �G���Ŏ��̉���炷

	//�ړ��֌W---------------------------------------------------------------------------------------------------------------------
	//���ϐ�
	float	MoveTime;			// �S�[�X�g�̈ړ��ɂ����鎞��(�b)
	FVector CurrentLocation;	// �G�̌��݂̍��W
	FVector GoalLocation;		// �G�̈ړ�����W(���[�J�����W)
	FVector GoalLocation_World;	// �G�̈ړ�����W(���[���h���W)
	bool	bHasEndedMoving;	// �ړ����I��������
	FVector Direction;			// GoalLocation�֌������P�ʃx�N�g��
	float	TotalDistance;		// �J�n�ʒu����ړI�n�܂ł̒�������
	float	TraveledDistance;	// ����܂łɐi�񂾋���
	float	Amplitude;			// �U��
	float	Frequency;			// �g�̑���
	float	Speed;				// �ړI�n�܂ł̈ړ����x

	//���֐�
	virtual void ProcessJustForFirst_Move() PURE_VIRTUAL(AEnemys::ProcessJustForFirst_Move, );	// ���Move�J�ڎ��ɂ̂ݍs������
	virtual bool Move() PURE_VIRTUAL(AEnemys::Move, return false;);											// �ړ�����

	//�U���֌W---------------------------------------------------------------------------------------------------------------------
	//���ϐ�
	bool	bHasEndedAttack;											// �U�����I��������
	float	AttackUpToTime;												// �S�[�X�g�̍U���܂ł̎���(�b)

	//���֐�
	virtual bool Attack() PURE_VIRTUAL(AEnemys::Move, return false;);	// �U������

	//���S�֌W---------------------------------------------------------------------------------------------------------------------
	void EnemyDead();	// HP��0�ɂȂ�������ł�����

	//�o���֌W---------------------------------------------------------------------------------------------------------------------
	//���ϐ� 
	bool	bHasEndedAppear;	// �o�����I��������
	float	OpacityValue;		// �I�p�V�e�B�̒l
	int		TimeSpentInAppear;	// �o������̂ɂ����鎞��

	//���֐�
	virtual void ProcessJustForFirst_Appear() PURE_VIRTUAL(AEnemys::ProcessJustForFirst_Appear, );	// ��ԁFAppear�ōŏ��Ɉ�x�������鏈��
	virtual bool Appear() PURE_VIRTUAL(AEnemys::Appear, return false;);								// �G�o������

	//�ړ��֌W(Spline�g�p)---------------------------------------------------------------------------------------------------------
	float			NowEnemyLocation;		// ���ݓG������X�v���C����̈ʒu
	float			SplineLength;			// �X�v���C���̑S��
	int				CountGotInMoveState;	// �ړ���ԂɂȂ����񐔂̃J�E���g(����ڂ̈ړ���)
	TArray<FVector> GoalLocations;			// �G�̈ړ�����W(���[�J�����W)

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Setter�֐�-------------------------------------------------------------------------------------------------------------------
	void SetHP(const int HPValue);								// HP�̐ݒ�p�֐�
	void SetAttackUpToTime(const float SetTime);				// �U���܂ł̎��Ԑݒ�p�֐�
	void SetGoalLocations(const TArray<FVector>& SetLocations);	// �ڕW���W�̐ݒ�p�֐�
	void SetMoveTime(const float SetTime);						// �ړ����Ԃ̐ݒ�p
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void SetInitialData(const int HP, const float AttackUpToTimeValue, const TArray<FVector>& SetLocations, const float MoveTimeValue);	// �������ꂽ�Ƃ��̐ݒ�p�֐�
	virtual bool CheckPlayerLightColor(EFlashlight_Color PlayerColor) const PURE_VIRTUAL(AEnemys::GetEnemyColor, return false;);		// �v���C���[�̃��C�g�̐F�ƓG�̃��C�g�̐F���`�F�b�N����֐�
};