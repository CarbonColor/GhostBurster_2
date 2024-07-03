// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawn1.h"
#include "Enemys.h" 
#include "Components/BoxComponent.h" // ��Ƃ���BoxComponent���g�p����ꍇ�̃C���N���[�h
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemySpawn1::AEnemySpawn1()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemySpawn1::BeginPlay()
{
    Super::BeginPlay();

    // �C�x���g�f�B�X�p�b�`���[�̃o�C���h
    OnSpawnEnemy1.AddDynamic(this, &AEnemySpawn1::HandleSpawnEvent);

    // �Ⴆ�΁ABoxComponent���g�p���ăR���W�����C�x���g��ݒ肷��ꍇ�̗�
    UBoxComponent* BoxComponent = FindComponentByClass<UBoxComponent>();
    if (BoxComponent)
    {
        BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemySpawn1::HandleSpawnEvent);
    }
}

void AEnemySpawn1::HandleSpawnEvent()
{
    // �X�|�[���ʒu�Ɖ�]�̐ݒ��
    FVector SpawnLocation = GetActorLocation(); // ���g�̈ʒu���X�|�[���ʒu�Ƃ���
    FRotator SpawnRotation = GetActorRotation(); // ���g�̉�]���X�|�[����]�Ƃ���

    // �X�|�[������Enemy�̐��ƈʒu����
    int32 NumEnemiesToSpawn = 3; // ��Ƃ���3�̃X�|�[��������
    SpawnEnemies(SpawnLocation, SpawnRotation, NumEnemiesToSpawn);
}

void AEnemySpawn1::SpawnEnemies(FVector SpawnLocation, FRotator SpawnRotation, int32 NumEnemies)
{
    UWorld* World = GetWorld();
    if (World)
    {
        for (int32 i = 0; i < NumEnemies; ++i)
        {
            // �X�|�[���ʒu�����������ꍇ�A�����_���ȃI�t�Z�b�g�������邱�Ƃ��ł��܂�
            FVector AdjustedSpawnLocation = SpawnLocation + FVector(7750.f, -2090.f + i * 800.f, 780.f); // ��Ƃ��č�����100.f�����₷

            // Enemy���X�|�[�������鏈��
            AEnemy* NewEnemy = World->SpawnActor<AEnemy>(AEnemy::StaticClass(), AdjustedSpawnLocation, SpawnRotation);
            if (NewEnemy)
            {
                // �X�|�[���������̒ǉ�����
            }
            else
            {
                // �X�|�[�����s���̏���
            }
        }
    }
}
