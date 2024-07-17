// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemys.h"
#include "Engine/World.h"

// Sets default values
AEnemys::AEnemys()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemys::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemys::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//��Ԃ̍X�V
void AEnemys::UpdateState(State nowState)
{
	if (nowState != this->state)
	{
		this->state = nowState;
		this->MoveCount = 0;
	}
}

//HP��0�ɂȂ�������ł�����
void AEnemys::EnemyDead()
{
	this->Destroy();
	//�C�x���g�Ɏ��S�ʒm�𑗂�

	return;
}

//���݂�FPS���擾����
float AEnemys::GetWorldFPS()
{
	//�Q�[�����E�̏����擾
	UWorld* World = GetWorld();

	//�Q�[�����E�̏�񂪎擾�ł��Ă��Ȃ������甲����
	if (!World)
	{
		return 60.f; //�Ƃ肠����60fps�Ƃ��Ă���
	}

	//DeltaTime�擾
	float DeltaTime = World->GetDeltaSeconds();

	//���݂�FPS���v�Z���Ď擾
	float FPS = 1.f / DeltaTime;

	return FPS;
}

//HP�̐ݒ�p�֐�
void AEnemys::SetHP(float HPValue)
{
	this->Status.HP = HPValue;
}

//�U���܂ł̎��Ԑݒ�p�֐�
void AEnemys::SetAttackUpToTime(int SetTime)
{
	this->AttackUpToTime = SetTime;
}

//�ڕW���W�̐ݒ�p�֐�
void AEnemys::SetGoalPosition(float SetX, float SetY, float SetZ)
{
	this->Position.GoalX = SetX;
	this->Position.GoalY = SetY;
	this->Position.GoalZ = SetZ;
}

//�ړ����Ԃ̐ݒ�p
void AEnemys::SetMoveTime(int SetTime)
{
	this->MoveTime = SetTime;
}

//�������ꂽ�Ƃ��̐ݒ�p�֐�
void AEnemys::SetInitialData(float HP, int AttackUpToTimeValue, float GoalX, float GoalY, float GoalZ, int MoveTimeValue)
{
	this->SetHP(HP);
	this->SetAttackUpToTime(AttackUpToTimeValue);
	this->SetGoalPosition(GoalX, GoalY, GoalZ);
	this->SetMoveTime(MoveTimeValue);
}