// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/DamageInterface.h"
#include "Components/SphereComponent.h"
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
		int HP = 1;	//�S�[�X�g�̗̑�
	};
	FStatus Status;

	//���񋓌^
	//�G�̏��
	enum class State
	{
		Wait,	//�ҋ@
		Move,	//�ړ�
		Attack,	//�U��
		Die,	//���S
		Appear,	//�o��
	};
	State state = State::Appear;

	enum class EnemyColor : uint8
	{
		White	= 0,
		Green	= 1,
		Red		= 2,
		Blue	= 3,
	};
	EnemyColor enemyColor = EnemyColor::White;

	//���ϐ��錾
	//SceneComponent�̕ϐ��錾
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	//StaticMeshComponent�̕ϐ��錾
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> GhostMesh;

	//�R���W�����̕ϐ��錾
	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent> GhostCollision;

	//�_�C�i�~�b�N�}�e���A���̕ϐ��錾
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;

	//�G�̍s������p�̃J�E���g
	int MoveCount = 0;

	//�Q�[����fps���l��ݒ�
	float Gamefps = 60.f;

	//�G����ԑJ�ڂ����Ƃ��ɍŏ��ɍs���������s������true
	bool bShouldBeenProcessWhenFirstStateTransition = false;

	//�ړ��֌W
	float	MoveTime = 1.f;							// �S�[�X�g�̈ړ��ɂ����鎞��(�b)
	FVector CurrentLocation = FVector(0, 0, 0);		// �G�̌��݂̍��W
	FVector GoalLocation = FVector(0, 0, 0);		// �G�̈ړ�����W(���[�J�����W)
	FVector GoalLocation_World = FVector(0, 0, 0);	// �G�̈ړ�����W(���[���h���W)
	bool	bHasEndedMoving = false;				// �ړ����I��������
	FVector Direction = FVector(0, 0, 0);			// GoalLocation�֌������P�ʃx�N�g��
	float	TotalDistance = 0.f;					// �J�n�ʒu����ړI�n�܂ł̒�������
	float	TraveledDistance = 0.f;					// ����܂łɐi�񂾋���
	float	Amplitude = 40.0f;						// �U��
	float	Frequency = 1.0f;						// �g�̑���
	float	Speed = 80.0f;							// �ړI�n�܂ł̈ړ����x

	//�U���֌W
	bool	bHasEndedAttack = false;					// �U�����I��������
	float	AttackUpToTime = 0.f;							// �S�[�X�g�̍U���܂ł̎���(�b)

	//���֐��錾
	//Tick�ł̏���
	virtual void TickProcess() PURE_VIRTUAL(AEnemys::TickProcess, );

	//�G�l�~�[�̏�Ԕ��f
	virtual void Think() PURE_VIRTUAL(AEnemys::Think, );

	//��Ԃ̍X�V
	void UpdateState(State nowState);

	//��ԂɊ�Â�������������
	virtual void ActProcess() PURE_VIRTUAL(AEnemys::ActProcess, );

	//HP��0�ɂȂ�������ł�����
	void EnemyDead();

	//���݂�FPS���擾����
	float GetWorldFPS();

	//��ԁFMove�Ŏg���֐�
	virtual bool ProcessJustForFirst_Move() PURE_VIRTUAL(AEnemys::ProcessJustForFirst_Move, return false;);	// ���Move�J�ڎ��ɂ̂ݍs������
	virtual bool Move() PURE_VIRTUAL(AEnemys::Move, return false;);											// �ړ�����

	//�U���֌W
	//��ԁFAttack�Ŏg���֐�
	virtual bool Attack() PURE_VIRTUAL(AEnemys::Move, return false;);										// �U������

	//�o���֌W---------------------------------------------------------------------------------------------------------------------
	//���ϐ� 
	bool	bHasEndedAppear = false;	// �o�����I��������
	float	OpacityValue = 0.f;			// �I�p�V�e�B�̒l
	int		TimeSpentInAppear = 1;		// �o������̂ɂ����鎞��

	//���֐�
	virtual void ProcessJustForFirst_Appear() PURE_VIRTUAL(AEnemys::ProcessJustForFirst_Appear, );	// ��ԁFAppear�ōŏ��Ɉ�x�������鏈��
	virtual bool Appear() PURE_VIRTUAL(AEnemys::Appear, return false;);								// �G�o������

	//�T�E���h�֌W-----------------------------------------------------------------------------------------------------------------
	//���ϐ�
	TObjectPtr<USoundCue> AppearSound;		// �G�o�����̉�
	TObjectPtr<USoundCue> DisappearSound;	// �G���Ŏ��̉� 
	
	//���֐�
	void PlayAppearSound();					// �G�o�����̉���炷
	void PlayDisappearSound();				// �G���Ŏ��̉���炷

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Setter�֐�
	void SetHP(int HPValue);					//HP�̐ݒ�p�֐�
	void SetAttackUpToTime(float SetTime);		//�U���܂ł̎��Ԑݒ�p�֐�
	void SetGoalLocation(FVector SetLocation);	//�ڕW���W�̐ݒ�p�֐�
	void SetMoveTime(float SetTime);			//�ړ����Ԃ̐ݒ�p

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void SetInitialData(int HP, float AttackUpToTimeValue, FVector SetLocation, float MoveTimeValue); //�������ꂽ�Ƃ��̐ݒ�p�֐�

	//�v���C���[�̃��C�g�̐F�ƓG�̃��C�g�̐F���`�F�b�N����֐�
	virtual bool CheckPlayerLightColor(EFlashlight_Color PlayerColor) const PURE_VIRTUAL(AEnemys::GetEnemyColor, return false;);
};