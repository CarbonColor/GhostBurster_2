// Fill out your copyright notice in the Description page of Project Settings.


#include "Sound/GameBGM.h"

// Sets default values
AGameBGM::AGameBGM()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// VRRootコンポーネントを作る
	DefaultSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComponent"));
	// Rootコンポーネントに設定
	RootComponent = DefaultSceneComponent;

	//オーディオコンポーネントの作成
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = false;     //自動再生を無効にする
	AudioComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AGameBGM::BeginPlay()
{
	Super::BeginPlay();

	BGMIndex = 0;
	FadeTime = 3.5f;

	if (BGM_Cue.Num() <= BGMIndex || BGM_Cue.Num() == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Cue is Null or Over"));
	}
	else
	{
		AudioComponent->SetSound(BGM_Cue[BGMIndex]);
	}
	AudioComponent->Play();
}

void AGameBGM::ChangeBGM()
{
	BGMIndex++;

	if (BGM_Cue.Num() > BGMIndex)
	{
		//BGMのフェードアウト
		AudioComponent->FadeOut(FadeTime, 0.0f);
	
		//Boss直前なら長い間音を止める
		if (BGMIndex == 5)
		{
			//-----------------------------------------------------------------
			//5ステージ目終了～ボスバトル開始までの時間を計測
			//-----------------------------------------------------------------
			GetWorld()->GetTimerManager().SetTimer(FadeEndHandle, this, &AGameBGM::NextBGM, 22.0f, false);
		}
		else
		{
			//2秒後にBGMの切り替え
			GetWorld()->GetTimerManager().SetTimer(FadeEndHandle, this, &AGameBGM::NextBGM, FadeTime, false);
		}
	
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Silver, TEXT("TimerHandle Called"));
	}
	else
	{
		FadeTime = 8.0f;
		//BGMのフェードアウト
		AudioComponent->FadeOut(FadeTime, 0.0f);

		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Silver, TEXT("Final BGM"));
	}
}

void AGameBGM::NextBGM()
{
	GetWorld()->GetTimerManager().ClearTimer(FadeEndHandle);
	//BGMの変更
	AudioComponent->SetSound(BGM_Cue[BGMIndex]);
	//フェードインして再生
	AudioComponent->FadeIn(FadeTime, 1.0f);

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Silver, TEXT("BGM Changed"));
}

//// Called every frame
//void AGameBGM::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}
//
