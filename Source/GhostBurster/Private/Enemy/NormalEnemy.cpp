// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/NormalEnemy.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/VRPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ANormalEnemy::ANormalEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//��SceneComponent
	//SceneComponent�̍쐬
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	//SceneComponent��RootComponent�ɐݒ�
	RootComponent = DefaultSceneRoot;

	//��StaticMeshComponent
	//StaticMeshComponent�̍쐬
	GhostMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ghost"));
	//StaticMesh�����[�h����StaticMeshComponent��StaticMesh�ɐݒ肷��
	UStaticMesh* GMesh = LoadObject<UStaticMesh>(NULL, TEXT("/Engine/BasicShapes/Sphere"), NULL, LOAD_None, NULL);
	GhostMesh->SetStaticMesh(GMesh);
	//StaticMeshComponent��RootComponent�ɃA�^�b�`����
	GhostMesh->SetupAttachment(RootComponent);
	//�X�^�e�B�b�N���b�V���̃R���W�����𖳂���
	GhostMesh->SetCollisionProfileName("NoCollision");

	//���R���W����
	//�X�t�B�A�R���W�����̍쐬
	GhostCollision = CreateDefaultSubobject<USphereComponent>(TEXT("GhostCollision"));
	//GhostCollision�����[�g�R���|�[�l���g�ɃA�^�b�`����
	GhostCollision->SetupAttachment(RootComponent);
	//GhostCollision�̃R���W�����v���Z�b�g��OverlapAllDynamic�ɂ���
	GhostCollision->SetCollisionProfileName("OverlapAllDynamic");
}

// Called when the game starts or when spawned
void ANormalEnemy::BeginPlay()
{
	Super::BeginPlay();

	//�������G�̐ݒ�
	this->enemyColor = EnemyColor::White;

	//���}�e���A��
	//�}�e���A�������[�h
	UMaterial* Material = LoadObject<UMaterial>(NULL, TEXT("/Game/_TeamFolder/Enemy/White"), NULL, LOAD_None, NULL);
	if (Material)
	{
		//�_�C�i�~�b�N�}�e���A���C���X�^���X���쐬
		this->DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);

		//GhostMesh�Ƀ_�C�i�~�b�N�}�e���A����ݒ�
		GhostMesh->SetMaterial(0, DynamicMaterial);

		//�����I�p�V�e�B�l��ݒ�
		this->DynamicMaterial->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);
	}
}

// Called every frame
void ANormalEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickProcess();
}

//���ǉ��֐�
// Tick�ł̏���
void ANormalEnemy::TickProcess()
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
void ANormalEnemy::Think()
{
	State nowState = state;

	switch (nowState)
	{
	case State::Wait:	//�ҋ@
		if (MoveCount >= 1 * Gamefps) { nowState = State::Attack; }	// �U����
		if (Status.HP <= 0) { nowState = State::Die; }				// ���S��
		break;

	case State::Move:	//�ړ�
		if (this->bHasEndedMoving) { nowState = State::Wait; }	// �ҋ@��
		if (Status.HP <= 0) { nowState = State::Die; }			// ���S��
		break;

	case State::Attack:	//�U��
		if (this->bHasEndedAttack) { nowState = State::Wait; }	// �ҋ@��
		if (Status.HP <= 0) { nowState = State::Die; }			// ���S��
		break;

	case State::Appear:	//�o��
		if (this->bHasEndedAppear) { nowState = State::Move; }	// �ړ���
		break;
	}

	UpdateState(nowState);
}

//��ԂɊ�Â�������������
void ANormalEnemy::ActProcess()
{
	switch (state)
	{
	case State::Wait:	//�ҋ@		
		break;

	case State::Move:	//�ړ�
		//���Move�J�ڎ��ɂ̂ݍs������
		if (this->bShouldBeenProcessWhenFirstStateTransition == false)
		{
			this->bShouldBeenProcessWhenFirstStateTransition = ProcessJustForFirst_Move();
		}

		//�ړ�����(�ړ��������I��������ԑJ�ڂ���)
		this->bHasEndedMoving = Move();
		break;

	case State::Attack:	//�U��
		//�U������(�U�����I��������ԑJ�ڂ���)
		this->bHasEndedAttack = this->Attack();
		break;

	case State::Die:	//���S
		EnemyDead();
		break;

	case State::Appear:	//�o��
		//�o������
		this->bHasEndedAppear = this->Appear();
		break;
	}
}

//�_���[�W���󂯂鏈���A�����ł���������l���̗͂����炷
void ANormalEnemy::RecieveEnemyDamage(int DamageAmount, EFlashlight_Color Color)
{
	if (this->state != State::Appear && this->state != State::Die)
	{
		if ((int)Color == (int)this->enemyColor)
		{
			Status.HP -= DamageAmount;
		}
	}
}

//�A�C�e���Ń_���[�W���󂯂鏈���A�����ł���������l���̗͂����炷
void ANormalEnemy::RecieveItemDamage(int DamageAmount)
{
	Status.HP -= DamageAmount;
}

//���Move�J�ڎ��ɂ̂ݍs������
bool ANormalEnemy::ProcessJustForFirst_Move()
{
	//�[���N���A����
	this->TraveledDistance = 0.f;

	// �����ʒu�̐ݒ�
	this->CurrentLocation = GetActorLocation();

	// �ڕW���W�̐ݒ�
	/*�����ړ���������s����悤�ɂȂ������A�O���t�@�C������ǂݍ���Őݒ�*/

	// ���[���h���W�ւ̕ϊ�
	this->GoalLocation_World = this->CurrentLocation + this->GoalLocation;

	// �����x�N�g���̌v�Z
	this->Direction = (this->GoalLocation_World - this->CurrentLocation).GetSafeNormal();

	// ���ړ������̌v�Z
	this->TotalDistance = FVector::Dist(this->CurrentLocation, this->GoalLocation_World);

	// �ړI�n�ɒ����܂ł̎��Ԃɍ����悤�ɑ��x���v�Z
	this->Speed = this->TotalDistance / this->MoveTime;

	// �������I�������true��Ԃ�
	return true;
}

//�ړ�����
bool ANormalEnemy::Move()
{
	//DeltaTime�̎擾
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	//�ړI�n�܂ł̎c�苗�����v�Z
	float RemainingDistance = TotalDistance - TraveledDistance;

	//���݂̑��x�ł̈ړ��������v�Z
	float DeltaDistance = Speed * DeltaTime;

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

	//�����g�Ɋ�Â��ăI�t�Z�b�g���v�Z(�ړI�n�ɏu�Ԉړ����Ē����Ȃ��悤�ɒ�������v�Z)
	float Offset_Z = Amplitude * FMath::Sin(2.0f * PI * (TraveledDistance / TotalDistance));

	// �V�����ʒu���v�Z
	FVector NewLocation = CurrentLocation + (Direction * TraveledDistance);
	NewLocation.Z += Offset_Z;

	// �V�����ʒu�Ɉړ�
	SetActorLocation(NewLocation);

	// �ړI�n�ɓ��B�����珈�����I��
	if (TraveledDistance >= TotalDistance)
	{
		SetActorLocation(this->GoalLocation_World);

		//��ԑJ�ڂł���悤�ɂ���
		return true;
	}

	return false;
}

//�U������
bool ANormalEnemy::Attack()
{
	//�U������
	if (MoveCount >= AttackUpToTime * Gamefps)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("WhiteEnemy Attack!"), true, true, FColor::White, 2.f, TEXT("None"));

		//�v���C���[�փ_���[�W��^����
		//�v���C���[�̏��擾
		AVRPlayerCharacter* Player = Cast<AVRPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		if (Player)
		{
			Player->RecievePlayerDamage();
		}

		//�U���I��(�������Ő��䂵�A�A�j���[�V�������I�������ɂ��邩��)
		return true;
	}

	return false;
}

// �G�o������
bool ANormalEnemy::Appear()
{
	//DeltaTime�̎擾
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	if (DynamicMaterial)
	{
		//�I�p�V�e�B�̒l��ύX
		this->OpacityValue += 1.f / (float)TimeSpentInAppear * DeltaTime;

		//�o�����I������珈�����I������
		if (this->OpacityValue >= 1.f)
		{
			//�I�p�V�e�B�̒l��1�𒴂��Ȃ��悤�ɂ���
			this->OpacityValue = 1.f;

			//�I�p�V�e�B��ݒ�
			this->DynamicMaterial->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);

			//��ԑJ�ډ\�ɂ���
			return true;
		}

		//�I�p�V�e�B��ݒ�
		this->DynamicMaterial->SetScalarParameterValue(FName("Opacity"), this->OpacityValue);
	}

	return false;
}

//����
/*
Move��̓G�̍s�����O���t�@�C���Ő��䂵����
�E�O���t�@�C���̓��e���Z�b�g����֐��̒ǉ�
�EMove��̏�ԑJ�ڏ������O���t�@�C���̓��e�Ō��܂�悤�ɂ���

switch(hoge)
{
	case 0:
		State StateHoge1 = State::Wait;
}
�݂����Ȋ���
*/