// Fill out your copyright notice in the Description page of Project Settings.

#include "Spawn.h"
#include "Enemy.h" 

// Sets default values
ASpawn::ASpawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	static ConstructorHelpers::FClassFinder<AEnemy> EnemyClassFinder(TEXT("/Game/EnemyBP"));
	DefaultEnemyClass = EnemyClassFinder.Class;
}

void ASpawner::BeginPlay()
{
    Super::BeginPlay();

    // �C�x���g�f�B�X�p�b�`���[���o�C���h
    OnSpawn.Broadcast();
}

void ASpawner::SpawnEnemies(FVector SpawnLocation, FRotator SpawnRotation, int32 NumEnemies)
{
    UWorld* World = GetWorld();
    if (World)
    {
        for (int32 i = 0; i < NumEnemies; ++i)
        {
            // �X�|�[���ʒu�����������ꍇ�A�����_���ȃI�t�Z�b�g�������邱�Ƃ��ł��܂�
            FVector AdjustedSpawnLocation = SpawnLocation + FVector(0.f, 0.f, i * 100.f); // ��Ƃ��č�����100.f�����₷

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