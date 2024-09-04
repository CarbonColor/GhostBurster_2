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
}

// Called when the game starts or when spawned
void ATitleEventManager::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ATitleEventManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATitleEventManager::EnemyDeadFunction()
{
	EnemyCount--;
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Call EnemyDeadFunction on Title"));

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
	}
}
void ATitleEventManager::IsUseBuffItem()
{
	bIsUseBuffItem = true;
	if (EnemyCount <= 0 && EventNumber == 3)
	{
		if (PhaseClearSound)
		{
			UGameplayStatics::PlaySound2D(this, PhaseClearSound);
		}
		ThirdEventDispatcher.Broadcast();
		ThirdEventDispatcher.Clear();
	}
}


void ATitleEventManager::FirstEvent()
{
	//敵の生成
	GetWorld()->SpawnActor<ATitleEnemy_White>(WhiteEnemyClass, First_White_Location, FRotator::ZeroRotator);
	GetWorld()->SpawnActor<ATitleEnemy_Green>(GreenEnemyClass, First_Green_Location, FRotator::ZeroRotator);
	GetWorld()->SpawnActor<ATitleEnemy_Red>(RedEnemyClass, First_Red_Location, FRotator::ZeroRotator);
	GetWorld()->SpawnActor<ATitleEnemy_Blue>(BlueEnemyClass, First_Blue_Location, FRotator::ZeroRotator);

	EnemyCount = 4;
}

void ATitleEventManager::SecondEvent()
{
	//敵の生成
	GetWorld()->SpawnActor<ATitleEnemy_White>(WhiteEnemyClass, Second_White_Location1, FRotator::ZeroRotator);
	GetWorld()->SpawnActor<ATitleEnemy_Green>(GreenEnemyClass, Second_Green_Location1, FRotator::ZeroRotator);
	GetWorld()->SpawnActor<ATitleEnemy_Red>(RedEnemyClass, Second_Red_Location1, FRotator::ZeroRotator);
	GetWorld()->SpawnActor<ATitleEnemy_Blue>(BlueEnemyClass, Second_Blue_Location1, FRotator::ZeroRotator);

	GetWorld()->SpawnActor<ATitleEnemy_White>(WhiteEnemyClass, Second_White_Location2, FRotator::ZeroRotator);
	GetWorld()->SpawnActor<ATitleEnemy_Green>(GreenEnemyClass, Second_Green_Location2, FRotator::ZeroRotator);
	GetWorld()->SpawnActor<ATitleEnemy_Red>(RedEnemyClass, Second_Red_Location2, FRotator::ZeroRotator);
	GetWorld()->SpawnActor<ATitleEnemy_Blue>(BlueEnemyClass, Second_Blue_Location2, FRotator::ZeroRotator);

	EnemyCount = 8;

	//アイテム使用状況の初期化
	bIsUseAttackItem = false;
}

void ATitleEventManager::ThirdEvent()
{
	//敵の生成
	ATitleEnemy_White* WhiteEnemy = GetWorld()->SpawnActor<ATitleEnemy_White>(WhiteEnemyClass, Third_White_Location, FRotator::ZeroRotator);
	WhiteEnemy->SetHP(60 * 10);
	WhiteEnemy->SetActorScale3D(FVector(1.5f, 1.5f, 1.5f));

	EnemyCount = 1;

	//宝箱の生成
	GetWorld()->SpawnActor<AActor>(TreasureBox, Third_Treasure_Location1, FRotator::ZeroRotator);
	GetWorld()->SpawnActor<AActor>(TreasureBox, Third_Treasure_Location2, FRotator::ZeroRotator);


	//アイテム使用状況の初期化
	bIsUseBuffItem = false;
}

void ATitleEventManager::FourthEvent()
{
	//イベントタイマーの設定
	GetWorld()->GetTimerManager().SetTimer(StartFourthEventHandle, this, &ATitleEventManager::StartFourthEvent, 1.5f, false);
	GetWorld()->GetTimerManager().SetTimer(EndFourthEventHandle, this, &ATitleEventManager::EndFourthEvent, 4.5f, false);

}
void ATitleEventManager::StartFourthEvent()
{
	GetWorld()->GetTimerManager().ClearTimer(StartFourthEventHandle);
	//余ったアイテムをスコアに変換
	AVRPlayerCharacter* Player = Cast<AVRPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
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