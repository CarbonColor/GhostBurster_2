// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemys.h"
#include "Engine/World.h"
#include "Player/VRPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AEnemys::AEnemys()
	:
	//��ԑJ�ڊ֌W
	MoveCount(0), bOnceDoProcessBeenIs(false), TimeFromWaitToStateTransition(1.f),
	//FPS�֌W
	Gamefps(60.f), AssumptionFPS(60),
	//�\����
	Status(),
	//�񋓌^
	State(EState::Appear), EnemyColor(EEnemyColor::White),
	//�R���|�[�l���g�֌W
	DefaultSceneRoot(nullptr), GhostMeshComponent(nullptr), GhostCollision(nullptr), DynamicMaterial_Body(nullptr), DynamicMaterial_Eye(nullptr), EnemyScale(FVector(0.5f, 0.5f, 0.5f)),
	//�A�j���[�V�����֌W
	DefaultAnim(nullptr), AttackAnim(nullptr), AttackTiming(55),
	//�T�E���h�֌W
	AppearSound(nullptr), DisappearSound(nullptr),
	//�ړ��֌W
	MoveTime(1.f), TraveledDistance(0.f), CurrentLocation(FVector(0.f, 0.f, 0.f)), GoalLocations(), MovingTimesCount(0), GoalLocation_World(FVector(0.f, 0.f, 0.f)), bHasEndedMoving(false),
	Direction(FVector(0.f, 0.f, 0.f)), TotalDistance(0.f), Amplitude(40.f), Frequency(1.f),
	//�U���֌W
	bHasEndedAttack(false),
	//���S�֌W
	bIsDestroy(false), TimeUpToTransparency(0.25f), bIsEscaped(false),
	//�o���֌W
	bHasEndedAppear(false), OpacityValue_Body(0.f), OpacityValue_Eye(0.f), TimeSpentInAppear(1.f), MaxOpacity_Body(1.f), MaxOpacity_Eye(1.f),
	//��]�֌W
	RotationCorrectionValue(FRotator(0.f, -90.f, 0.f))
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
		//�A�j���[�V�����ύX
		ChangeAnimation(State, NowState);

		State = NowState;
		MoveCount = 0;
		bOnceDoProcessBeenIs = false;
	}
}

//FPS�֌W----------------------------------------------------------------------------------------------------------------------
//���݂�FPS���擾����
float AEnemys::GetWorldFPS()
{
	//DeltaTime�擾
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	//���݂�FPS���v�Z���Ď擾
	float FPS = 1.f / DeltaTime;

	return FPS;
}

//���S�֌W---------------------------------------------------------------------------------------------------------------------
//HP��0�ɂȂ�������ł�����
void AEnemys::EnemyDead()
{
	//EnemyDead�ň�x�����s������
	if (bOnceDoProcessBeenIs == false)
	{
		ProcessDoOnce_EnemyDead();
	}

	//���X�ɓ����ɂ���
	bIsDestroy = Transparentize_Dead();

	//�G�����ł�����
	if (bIsDestroy)
	{
		this->Destroy();
	}
}

//EnemyDead�ň�x�����s������
void AEnemys::ProcessDoOnce_EnemyDead()
{
	if (!bIsEscaped) // �������Ă��Ȃ���
	{
		//�C�x���g�Ɏ��S�ʒm�𑗂�
	// �v���C���[���擾
		TObjectPtr<AVRPlayerCharacter> Player = Cast<AVRPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
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
	}
	
	//�����蔻�������
	if (GhostCollision) // null�`�F�b�N
	{
		GhostCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	//�����񏈗����s���Ȃ��悤�ɂ���
	bOnceDoProcessBeenIs = true;
}

//���X�ɓ����ɂ���
bool AEnemys::Transparentize_Dead()
{
	//DeltaTime�̎擾
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	if (DynamicMaterial_Body && DynamicMaterial_Eye)
	{
		//�I�p�V�e�B�̒l���v�Z
		OpacityValue_Body -= MaxOpacity_Body / TimeUpToTransparency * DeltaTime;	// �̂̃I�p�V�e�B�̌v�Z
		OpacityValue_Eye -= MaxOpacity_Eye / TimeUpToTransparency * DeltaTime;		// �ڂ̃I�p�V�e�B�̌v�Z

		//�o�����I������珈�����I������
		if (OpacityValue_Body <= 0.f && OpacityValue_Eye <= 0.f)
		{
			//�I�p�V�e�B�̒l��0�������Ȃ��悤�ɂ���
			OpacityValue_Body = 0.f;
			OpacityValue_Eye = 0.f;

			//�I�p�V�e�B��ݒ�
			DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Body);
			DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Eye);

			//���̊֐����Ă΂�Ȃ��悤�ɂ���
			return true;
		}

		//�I�p�V�e�B��ݒ�
		DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Body);
		DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Eye);
	}

	//������x���̊֐����Ă�
	return false;
}

//�o���֌W---------------------------------------------------------------------------------------------------------------------
//��ԁFAppear�ōŏ��Ɉ�x�������鏈��
void AEnemys::ProcessJustForFirst_Appear()
{
	//�G�o�����̉���炷
	PlayAppearSound();

	//�����񏈗����s���Ȃ��悤�ɂ���
	this->bOnceDoProcessBeenIs = true;
}

//�G�o������
bool AEnemys::Appear()
{
	//DeltaTime�̎擾
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	if (DynamicMaterial_Body && DynamicMaterial_Eye)
	{
		//�I�p�V�e�B�̒l���v�Z
		OpacityValue_Body += MaxOpacity_Body / TimeSpentInAppear * DeltaTime;	// �̂̃I�p�V�e�B�̌v�Z
		OpacityValue_Eye += MaxOpacity_Eye / TimeSpentInAppear * DeltaTime;		// �ڂ̃I�p�V�e�B�̌v�Z

		//�o�����I������珈�����I������
		if (OpacityValue_Body >= MaxOpacity_Body && OpacityValue_Eye >= MaxOpacity_Eye)
		{
			//�I�p�V�e�B�̒l��1�𒴂��Ȃ��悤�ɂ���
			OpacityValue_Body = MaxOpacity_Body;
			OpacityValue_Eye = MaxOpacity_Eye;

			//�I�p�V�e�B��ݒ�
			DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Body);
			DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Eye);

			//�����蔻���t����
			GhostCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

			//��ԑJ�ډ\�ɂ���
			return true;
		}

		//�I�p�V�e�B��ݒ�
		DynamicMaterial_Body->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Body);
		DynamicMaterial_Eye->SetScalarParameterValue(FName("Opacity"), this->OpacityValue_Eye);
	}

	return false;
}

//��]�֌W---------------------------------------------------------------------------------------------------------------------
//�v���C���[�̕���������
void AEnemys::FacePlayerHowTo()
{
	//�v���C���[���擾����
	TObjectPtr<AVRPlayerCharacter> Player = Cast<AVRPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (Player) //null�`�F�b�N
	{
		//�v���C���[�̕������擾����
		FVector PlayerLocation = Player->GetActorLocation();

		//�������v�Z����
		FVector EnemyLocation = GetActorLocation();

		//X����������ɂ�����]���擾
		FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(EnemyLocation, PlayerLocation);

		//��]�̕␳�l�𑫂�
		FRotator SetRotation = NewRotation + RotationCorrectionValue;

		//���������̉�]�𖳂���
		SetRotation.Pitch = 0.f;

		//��]���A�N�^�[�ɓK�p������
		SetActorRotation(SetRotation);
	}
}

//�A�j���[�V�����֌W-----------------------------------------------------------------------------------------------------------
void AEnemys::ChangeAnimation(const EState PreState, const EState NewState)
{
	//��Ԃɍ��킹���A�j���[�V�����ύX
	switch (NewState)
	{
	case EState::Attack: // �U�����
		if (AttackAnim)	// null�`�F�b�N
		{
			GhostMeshComponent->PlayAnimation(AttackAnim, false);
		}
		break;

	default: // ����̃A�j���[�V�������Ȃ����
		if (DefaultAnim) // null�`�F�b�N
		{
			if (PreState == EState::Attack || PreState == EState::Appear) // �ύX�O�̏�Ԃ�����̃A�j���[�V�����������Ă���܂��́A�A�j���[�V�������g�p���Ȃ���Ԃ�������
			{
				GhostMeshComponent->PlayAnimation(DefaultAnim, true);
			}
		}
		break;

	//�A�j���[�V�������g�p���Ȃ����-------------------------------------------------------------------------
	case EState::Appear: // ���̏�Ԃ͓G�̍ŏ��̏�ԂȂ̂ŃA�j���[�V�����̋����I���͕K�v�Ȃ�
		break;

	case EState::Die: // ���̏�Ԃ̌�ɃA�j���[�V�������ύX����邱�Ƃ͂Ȃ��̂�default��if���̕ύX�O�̏�Ԃ͎��S��Ԃ��ǂ����̊m�F�͂��Ȃ��Ă悢
		if (GhostMeshComponent)
		{
			GhostMeshComponent->Stop();
		}
		break;
	}
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
	Status.HP = HPValue;
}

//�U���܂ł̎��Ԑݒ�p�֐�
void AEnemys::SetTimeFromWaitToStateTransition(const float SetTime)
{
	TimeFromWaitToStateTransition = SetTime;
}

//�ڕW���W�̐ݒ�p�֐�
void AEnemys::SetGoalLocations(const TArray<FVector>& SetLocations)
{
	GoalLocations = SetLocations;
}

//�ړ����Ԃ̐ݒ�p
void AEnemys::SetMoveTime(const float SetTime)
{
	MoveTime = SetTime;
}

//�������ꂽ�Ƃ��̐ݒ�p�֐�
void AEnemys::SetInitialData(const int HP, const float SetTime, const TArray<FVector>& SetLocations, const float MoveTimeValue)
{
	SetHP(HP);
	SetTimeFromWaitToStateTransition(SetTime);
	SetGoalLocations(SetLocations);
	SetMoveTime(MoveTimeValue);
}