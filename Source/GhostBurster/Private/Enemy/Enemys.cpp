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

//ó‘Ô‚ÌXV
void AEnemys::UpdateState(State nowState)
{
	if (nowState != this->state)
	{
		this->state = nowState;
		this->MoveCount = 0;
	}
}

//HP‚ª0‚É‚È‚Á‚½‚çÁ–Å‚³‚¹‚é
void AEnemys::EnemyDead()
{
	//ƒCƒxƒ“ƒg‚ÉŽ€–S’Ê’m‚ð‘—‚é

	//“G‚ðÁ–Å‚³‚¹‚é
	this->Destroy();

	return;
}

//Œ»Ý‚ÌFPS‚ðŽæ“¾‚·‚é
float AEnemys::GetWorldFPS()
{
	//ƒQ[ƒ€¢ŠE‚Ìî•ñ‚ðŽæ“¾
	UWorld* World = GetWorld();

	//ƒQ[ƒ€¢ŠE‚Ìî•ñ‚ªŽæ“¾‚Å‚«‚Ä‚¢‚È‚©‚Á‚½‚ç”²‚¯‚é
	if (!World)
	{
		return 60.f; //‚Æ‚è‚ ‚¦‚¸60fps‚Æ‚µ‚Ä‚¨‚­
	}

	//DeltaTimeŽæ“¾
	float DeltaTime = World->GetDeltaSeconds();

	//Œ»Ý‚ÌFPS‚ðŒvŽZ‚µ‚ÄŽæ“¾
	float FPS = 1.f / DeltaTime;

	return FPS;
}

//HP‚ÌÝ’è—pŠÖ”
void AEnemys::SetHP(float HPValue)
{
	this->Status.HP = HPValue;
}

//UŒ‚‚Ü‚Å‚ÌŽžŠÔÝ’è—pŠÖ”
void AEnemys::SetAttackUpToTime(int SetTime)
{
	this->AttackUpToTime = SetTime;
}

//–Ú•WÀ•W‚ÌÝ’è—pŠÖ”
void AEnemys::SetGoalPosition(float SetX, float SetY, float SetZ)
{
	this->Position.GoalX = SetX;
	this->Position.GoalY = SetY;
	this->Position.GoalZ = SetZ;
}

//ˆÚ“®ŽžŠÔ‚ÌÝ’è—p
void AEnemys::SetMoveTime(int SetTime)
{
	this->MoveTime = SetTime;
}

//¶¬‚³‚ê‚½‚Æ‚«‚ÌÝ’è—pŠÖ”
void AEnemys::SetInitialData(float HP, int AttackUpToTimeValue, float GoalX, float GoalY, float GoalZ, int MoveTimeValue)
{
	this->SetHP(HP);
	this->SetAttackUpToTime(AttackUpToTimeValue);
	this->SetGoalPosition(GoalX, GoalY, GoalZ);
	this->SetMoveTime(MoveTimeValue);
}