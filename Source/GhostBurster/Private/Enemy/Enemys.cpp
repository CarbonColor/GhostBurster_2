// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemys.h"
#include "Engine/World.h"
#include "Player/VRPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemys::AEnemys()
	:
	MoveCount(0),
	Gamefps(60.f),
	bShouldBeenProcessWhenFirstStateTransition(false),
	//�\����
	Status(),
	//�񋓌^
	State(EState::Appear), EnemyColor(EEnemyColor::White),
	//�R���|�[�l���g�֌W
	DefaultSceneRoot(nullptr), GhostMesh(nullptr), GhostCollision(nullptr), DynamicMaterial(nullptr), SplineComponent(nullptr),
	//�T�E���h�֌W
	AppearSound(nullptr), DisappearSound(nullptr),
	//�ړ��֌W
	/*CurrentLocation(FVector(0.f, 0.f, 0.f)), GoalLocation(FVector(0.f, 0.f, 0.f)), GoalLocation_World(FVector(0.f, 0.f, 0.f)), bHasEndedMoving(false), Direction(FVector(0.f, 0.f, 0.f)), TotalDistance(0.f), Amplitude(40.f), Frequency(1.f),*/
	MoveTime(1.f), TraveledDistance(0.f), SplineLength(0.f), CountGotInMoveState(0), GoalLocations(), bHasEndedMoving(false),
	//�U���֌W
	bHasEndedAttack(false), AttackUpToTime(0.f),
	//�o���֌W
	bHasEndedAppear(false), OpacityValue(0.f), TimeSpentInAppear(1)
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
void AEnemys::UpdateState(EState NowState)
{
	if (NowState != this->State)
	{
		this->State = NowState;
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
		FString SpawnBPName = FString::Printf(TEXT("EnemysSpawn_BP_C_%d"), Stage + 2);
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Silver, SpawnBPName);

		//�Y����EnemySpawn���擾
		TArray<AActor*> Spawners;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Spawner"), Spawners);

		for (AActor* Spawner : Spawners)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, Spawner->GetName());

			if (Spawner->GetName() == SpawnBPName)
			{
				if (UFunction* Func = Spawner->FindFunction(FName("EnemyDeadFunction")))
				{
					Spawner->ProcessEvent(Func, nullptr);
					Player->AddScore(100);
				}
			}
		}
	}

	//�G���Ŏ��̉���炷
	PlayDisappearSound();

	//�G�����ł�����
	this->Destroy();

	return;
}

//���݂�FPS���擾����
float AEnemys::GetWorldFPS()
{
	//DeltaTime�擾
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	//���݂�FPS���v�Z���Ď擾
	float FPS = 1.f / DeltaTime;

	return FPS;
}

//�T�E���h�֐�-----------------------------------------------------------------------------------------------------------------
//�G�o�����̉���炷
void AEnemys::PlayAppearSound()
{
	if (this->AppearSound) // �ݒ肳��Ă��Ȃ��������Ȃ��悤�ɂ���
	{
		UGameplayStatics::PlaySoundAtLocation(this, AppearSound, GetActorLocation());
	}
}

//�G���Ŏ��̉���炷
void AEnemys::PlayDisappearSound()
{
	if (DisappearSound) // �ݒ肳��Ă��Ȃ��������Ȃ��悤�ɂ���
	{
		UGameplayStatics::PlaySoundAtLocation(this, DisappearSound, GetActorLocation());
	}
}

//Setter�֐�-------------------------------------------------------------------------------------------------------------------
//HP�̐ݒ�p�֐�
void AEnemys::SetHP(const int HPValue)
{
	this->Status.HP = HPValue;
}

//�U���܂ł̎��Ԑݒ�p�֐�
void AEnemys::SetAttackUpToTime(const float SetTime)
{
	this->AttackUpToTime = SetTime;
}

//�ڕW���W�̐ݒ�p�֐�
void AEnemys::SetGoalLocations(const TArray<FVector>& SetLocations)
{
	this->GoalLocations = SetLocations;
}

//�ړ����Ԃ̐ݒ�p
void AEnemys::SetMoveTime(const float SetTime)
{
	this->MoveTime = SetTime;
}

//�������ꂽ�Ƃ��̐ݒ�p�֐�
void AEnemys::SetInitialData(const int HP, const float AttackUpToTimeValue, const TArray<FVector>& SetLocations, const float MoveTimeValue)
{
	this->SetHP(HP);
	this->SetAttackUpToTime(AttackUpToTimeValue);
	this->SetGoalLocations(SetLocations);
	this->SetMoveTime(MoveTimeValue);
}