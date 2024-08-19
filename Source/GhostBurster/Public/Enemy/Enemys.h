// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/DamageInterface.h"
#include "Components/SphereComponent.h"

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
		float HP = 1;	//�S�[�X�g�̗̑�
	};
	FStatus Status;

	//���W
	struct FPosition
	{
		float GoalX = 0;
		float GoalY = 0;
		float GoalZ = 0;
	};
	FPosition Position;
	//���񋓌^
	//�G�̏��
	enum class State
	{
		Stand,
		Move,
		Attack,
		Die,
	};
	State state = State::Stand;

	enum class EnemyColor : uint8
	{
		White = 0,
		Green = 1,
		Red = 2,
		Blue = 3,
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

	int MoveCount = 0;			//�S�[�X�g�̍s������p�̃J�E���g
	int Gamefps = 60;			//�Q�[����fps���l��ݒ�
	int AttackUpToTime = 15;	//�S�[�X�g�̍U���܂ł̎���
	int MoveTime = 0;			//�S�[�X�g�̈ړ��ɂ����鎞��

	//���֐��錾
	//Tick�ł̏���
	virtual void TickProcess() PURE_VIRTUAL(AEnemys::TickProcess, );

	//�G�l�~�[�̏�Ԕ��f
	virtual void Think() PURE_VIRTUAL(AEnemys::Think, );

	//��Ԃ̍X�V
	void UpdateState(State nowState);

	//��ԂɊ�Â�������������
	virtual void Move() PURE_VIRTUAL(AEnemys::Move, );

	//HP��0�ɂȂ�������ł�����
	void EnemyDead();

	//���݂�FPS���擾����
	float GetWorldFPS();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Setter�֐�
	void SetHP(float HPValue);									//HP�̐ݒ�p�֐�
	void SetAttackUpToTime(int SetTime);						//�U���܂ł̎��Ԑݒ�p�֐�
	void SetGoalPosition(float SetX, float SetY, float SetZ);	//�ڕW���W�̐ݒ�p�֐�
	void SetMoveTime(int SetTime);								//�ړ����Ԃ̐ݒ�p

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void SetInitialData(float HP, int AttackUpToTimeValue, float GoalX, float GoalY, float GoalZ, int MoveTimeValue); //�������ꂽ�Ƃ��̐ݒ�p�֐�
};
