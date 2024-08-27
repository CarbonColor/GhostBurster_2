// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemys.h"
#include "Engine/World.h"
#include "Player/VRPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

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
		this->bShouldBeenProcessWhenFirstStateTransition = false;
	}
}

//HP��0�ɂȂ�������ł�����
void AEnemys::EnemyDead()
{

	//�C�x���g�Ɏ��S�ʒm�𑗂�
	// �v���C���[���擾
	AVRPlayerCharacter* Player = Cast<AVRPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (Player)
	{
		//�X�e�[�W�����擾
		int Stage = Player->GetStageNumber();
		FString SpawnBPName = FString::Printf(TEXT("EnemysSpawn_BP_C_%d"), Stage + 6);
		//GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Silver, SpawnBPName);

		//�Y����EnemySpawn���擾
		TArray<AActor*> Spawners;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Spawner"), Spawners);

		for (AActor* Spawner : Spawners)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Purple, Spawner->GetName());

			if (Spawner->GetName() == SpawnBPName)
			{
				if (UFunction* Func = Spawner->FindFunction(FName("EnemyDeadFunction")))
				{
					Spawner->ProcessEvent(Func, nullptr);
					//GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, FString::Printf(TEXT("Call Spawner%d Function"), Stage));
				}
			}
		}
	}

	//�G�����ł�����
	this->Destroy();

	return;
}

//���݂�FPS���擾����
int AEnemys::GetWorldFPS()
{
	//DeltaTime�擾
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	//���݂�FPS���v�Z���Ď擾
	float FPS = 1.f / DeltaTime;

	return (int)FPS;
}

//HP�̐ݒ�p�֐�
void AEnemys::SetHP(int HPValue)
{
	this->Status.HP = HPValue;
}

//�U���܂ł̎��Ԑݒ�p�֐�
void AEnemys::SetAttackUpToTime(int SetTime)
{
	this->AttackUpToTime = SetTime;
}

//�ڕW���W�̐ݒ�p�֐�
void AEnemys::SetGoalLocation(FVector SetLocation)
{
	this->GoalLocation = SetLocation;
}

//�ړ����Ԃ̐ݒ�p
void AEnemys::SetMoveTime(int SetTime)
{
	this->MoveTime = SetTime;
}

//�������ꂽ�Ƃ��̐ݒ�p�֐�
void AEnemys::SetInitialData(int HP, int AttackUpToTimeValue, FVector SetLocation, int MoveTimeValue)
{
	this->SetHP(HP);
	this->SetAttackUpToTime(AttackUpToTimeValue);
	this->SetGoalLocation(SetLocation);
	this->SetMoveTime(MoveTimeValue);
}