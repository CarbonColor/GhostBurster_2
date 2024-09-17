// Fill out your copyright notice in the Description page of Project Settings.


#include "Title/TitleEventManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATitleEventManager::ATitleEventManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	EnemyCount = 0;
	EventNumber = 1;

	//�󔠂̃N���X�擾
	TreasureBox = LoadObject<UClass>(nullptr, TEXT("/Game/_TeamFolder/Player/Blueprint/BP_TreasureBox.BP_TreasureBox_C"));

}

// Called when the game starts or when spawned
void ATitleEventManager::BeginPlay()
{
	Super::BeginPlay();

	//�v���C���[�̎擾
	Player = Cast<AVRPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	bCanChangeLight = false;
	bCanUseAttackItem = false;
	bCanUseBuffItem = false;

}

// Called every frame
void ATitleEventManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATitleEventManager::EnemyDeadFunction()
{
	EnemyCount--;
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Call EnemyDeadFunction on Title"));

	if (EnemyCount <= 0)
	{
		switch (EventNumber)
		{
		case 1:
			if (PhaseClearSound)
			{
				UGameplayStatics::PlaySound2D(this, PhaseClearSound);
			}
			FirstEventDispatcher.Broadcast();
			FirstEventDispatcher.Clear();
			//���͂̐���
			bCanChangeLight = false;
			break;

		case 2:
			if (bIsUseAttackItem)
			{
				if (PhaseClearSound)
				{
					UGameplayStatics::PlaySound2D(this, PhaseClearSound);
				}
				SecondEventDispatcher.Broadcast();
				SecondEventDispatcher.Clear();
				//���͂̐���
				bCanUseAttackItem = false;
			}
			break;

		case 3:
			if (bIsUseBuffItem)
			{
				if (PhaseClearSound)
				{
					UGameplayStatics::PlaySound2D(this, PhaseClearSound);
				}
				ThirdEventDispatcher.Broadcast();
				ThirdEventDispatcher.Clear();
				//���͂̐���
				bCanChangeLight = false;
			}
			break;

		default:
			break;
		}
	}
}
void ATitleEventManager::IsUseAttackItem()
{
	bIsUseAttackItem = true;
	if (EnemyCount <= 0 && EventNumber == 2)
	{
		if (PhaseClearSound)
		{
			UGameplayStatics::PlaySound2D(this, PhaseClearSound);
		}
		SecondEventDispatcher.Broadcast();
		SecondEventDispatcher.Clear();
		//���͂̐���
		bCanUseAttackItem = false;
	}
}
void ATitleEventManager::IsUseBuffItem()
{
	bIsUseBuffItem = true;
	//���͂̐���
	bCanChangeLight = true;
}


void ATitleEventManager::FirstEvent()
{
	//�G�̐���
	//GetWorld()->SpawnActor<ATitleEnemy_White>(WhiteEnemyClass, First_White_Location, FRotator::ZeroRotator);
	GetWorld()->SpawnActor<ATitleEnemy_Green>(GreenEnemyClass, First_Green_Location, FRotator::ZeroRotator);
	GetWorld()->SpawnActor<ATitleEnemy_Red>(RedEnemyClass, First_Red_Location, FRotator::ZeroRotator);
	GetWorld()->SpawnActor<ATitleEnemy_Blue>(BlueEnemyClass, First_Blue_Location, FRotator::ZeroRotator);

	EnemyCount = 3;

	//SE�̍Đ�
	if (EnemySpawnSound)
	{
		UGameplayStatics::PlaySound2D(this, EnemySpawnSound);
	}

	//���͂̐���
	bCanChangeLight = true;
}

void ATitleEventManager::SecondEvent()
{
	//�G�̐���
	//GetWorld()->SpawnActor<ATitleEnemy_White>(WhiteEnemyClass, Second_White_Location1, FRotator::ZeroRotator);
	GetWorld()->SpawnActor<ATitleEnemy_Green>(GreenEnemyClass, Second_Green_Location1, FRotator::ZeroRotator);
	GetWorld()->SpawnActor<ATitleEnemy_Red>(RedEnemyClass, Second_Red_Location1, FRotator::ZeroRotator);
	GetWorld()->SpawnActor<ATitleEnemy_Blue>(BlueEnemyClass, Second_Blue_Location1, FRotator::ZeroRotator);

	//GetWorld()->SpawnActor<ATitleEnemy_White>(WhiteEnemyClass, Second_White_Location2, FRotator::ZeroRotator);
	GetWorld()->SpawnActor<ATitleEnemy_Green>(GreenEnemyClass, Second_Green_Location2, FRotator::ZeroRotator);
	GetWorld()->SpawnActor<ATitleEnemy_Red>(RedEnemyClass, Second_Red_Location2, FRotator::ZeroRotator);
	GetWorld()->SpawnActor<ATitleEnemy_Blue>(BlueEnemyClass, Second_Blue_Location2, FRotator::ZeroRotator);

	EnemyCount = 6;

	//�A�C�e���g�p�󋵂̏�����
	bIsUseAttackItem = false;

	//SE�̍Đ�
	if (EnemySpawnSound)
	{
		UGameplayStatics::PlaySound2D(this, EnemySpawnSound);
	}

	for (int i = 0; i < 3; ++i)
	{
		Player->AddItem();
	}

	//���͂̐���
	bCanUseAttackItem = true;

}

void ATitleEventManager::ThirdEvent()
{
	//�G�̐���
	ATitleEnemy_Green* GreenEnemy1 = GetWorld()->SpawnActor<ATitleEnemy_Green>(GreenEnemyClass, Third_Green_Location1, FRotator::ZeroRotator);
	GreenEnemy1->SetHP(60 * 10 * 5);
	GreenEnemy1->SetActorScale3D(FVector(0.35f, 0.35f, 0.35f));
	ATitleEnemy_Green* GreenEnemy2 = GetWorld()->SpawnActor<ATitleEnemy_Green>(GreenEnemyClass, Third_Green_Location2, FRotator::ZeroRotator);
	GreenEnemy2->SetHP(60 * 10 * 5);
	GreenEnemy2->SetActorScale3D(FVector(0.35f, 0.35f, 0.35f));

	EnemyCount = 2;

	//�󔠂̐���
	Box1 = GetWorld()->SpawnActor<ATreasureBox>(TreasureBox, Third_Treasure_Location1, FRotator::ZeroRotator);
	Box2 = GetWorld()->SpawnActor<ATreasureBox>(TreasureBox, Third_Treasure_Location2, FRotator::ZeroRotator);

	//�A�C�e���g�p�󋵂̏�����
	bIsUseBuffItem = false;

	//SE�̍Đ�
	if (EnemySpawnSound)
	{
		UGameplayStatics::PlaySound2D(this, EnemySpawnSound);
	}

	//���͂̐���
	bCanUseBuffItem = true;
}

void ATitleEventManager::FourthEvent()
{
	//�C�x���g�^�C�}�[�̐ݒ�
	GetWorld()->GetTimerManager().SetTimer(StartFourthEventHandle, this, &ATitleEventManager::StartFourthEvent, 1.5f, false);
	GetWorld()->GetTimerManager().SetTimer(EndFourthEventHandle, this, &ATitleEventManager::EndFourthEvent, 4.5f, false);

	//�󔠂�����
	if (Box1)
	{
		Box1->Destroy();
	}
	if (Box2)
	{
		Box2->Destroy();
	}
	if (Box3)
	{
		Box3->Destroy();
	}
}
void ATitleEventManager::StartFourthEvent()
{
	GetWorld()->GetTimerManager().ClearTimer(StartFourthEventHandle);
	//�]�����A�C�e�����X�R�A�ɕϊ�
	Player->ChangeScore();
}
void ATitleEventManager::EndFourthEvent()
{
	GetWorld()->GetTimerManager().ClearTimer(EndFourthEventHandle);
	FourthEventDispatcher.Broadcast();
}

void ATitleEventManager::NextEventNumber()
{
	EventNumber++;
}

void ATitleEventManager::TreasureBoxSpawn()
{
	Box3 = GetWorld()->SpawnActor<ATreasureBox>(TreasureBox, TBS_Treasure_Location, FRotator::ZeroRotator);
}

bool ATitleEventManager::GetCanChangeLight()
{
	return bCanChangeLight;
}
bool ATitleEventManager::GetCanUseAttackItem()
{
	return bCanUseAttackItem;
}
bool ATitleEventManager::GetCanUseBuffItem()
{
	return bCanUseBuffItem;
}