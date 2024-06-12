// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/NormalEnemy.h"
#include "Kismet/KismetSystemLibrary.h"

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
}

// Called when the game starts or when spawned
void ANormalEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANormalEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveCount++;

	//HP��0�ɂȂ�������ł�����
	EnemyDead();

	//�o�����Ă���5�b��������U��������
	Attack();
}

//���ǉ��֐�
//HP��0�ɂȂ�������ł�����
void ANormalEnemy::EnemyDead()
{
	if (status.HP <= 0)
	{
		this->Destroy();
		//�C�x���g�ɑ΂��Ď��S�ʒm�𑗂�

		return;
	}
}

//�G�̍U������
void ANormalEnemy::Attack()
{
	if (MoveCount >= Gamefps * 5) //�t���[������60�Œ�łȂ��ꍇ�A�ύX
	{
		MoveCount = 0;

		//�U������
	}
}

//�_���[�W���󂯂鏈���A�����ł�������U���͕��̗͂����炷
void ANormalEnemy::Damage(float damage)
{
	status.HP -= damage;
}