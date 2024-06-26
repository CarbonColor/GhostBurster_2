// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/DamageInterface.h"
#include "Enemys.generated.h"

UCLASS(Abstract)
class GHOSTBURSTER_API AEnemys : public AActor, public IDamageInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemys();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//���\����
	//�X�e�[�^�X
	struct Status
	{
		float HP = 1;	//�S�[�X�g�̗̑�
	};
	Status status;

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

	//�G�̐F���
	enum class EnemyColor : uint8
	{
		White = 0,
		Green = 1,
		Red	  = 2,
		Blue  = 3,
	};
	EnemyColor EColor = EnemyColor::White;

	//���ϐ��錾
	int MoveCount = 0;	//�S�[�X�g�̍s������p�̃J�E���g
	int Gamefps = 60;	//�Q�[����fps���l��ݒ�

	//���֐��錾
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

};
