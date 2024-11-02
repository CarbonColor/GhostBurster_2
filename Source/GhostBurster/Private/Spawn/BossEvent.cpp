// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawn/BossEvent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Player/VRPlayerCharacter.h"

// Sets default values
ABossEvent::ABossEvent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//スポットライトコンポーネントの作成
	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLightComponent"));
	RootComponent = SpotLight;
	//初期設定
	SpotLight->SetVisibility(false);
	SpotLight->SetIntensity(10000.0f);
	SpotLight->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	SpotLight->SetAttenuationRadius(3000.0f);
	SpotLight->SetInnerConeAngle(30.0f);
	SpotLight->SetOuterConeAngle(80.0f);

	static ConstructorHelpers::FObjectFinder<USoundCue> SoundCue(TEXT("/Game/_TeamFolder/Sound/SE/SE_LastLight_Cue.SE_LastLight_Cue"));
	if (SoundCue.Succeeded())
	{
		LightSound = SoundCue.Object;
	}

}

// Called when the game starts or when spawned
void ABossEvent::BeginPlay()
{
	Super::BeginPlay();

	//------------------------------
	TargetIntensity = 10000.0f;
	AddIntensity = TargetIntensity / 20;

	//------------------------------

	BlinkCount = 0;
	UpdateCount = 0;
	
}

void ABossEvent::BossAppearEvent(ABossEnemy* Boss)
{
	BossEnemy = Boss;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABossEvent::BlinkLight, 2.0f, false);
}

void ABossEvent::BlinkLight()
{
	if (BlinkCount < 2)
	{
		//スポットライトの点滅処理
		if (SpotLight)
		{
			SpotLight->SetVisibility(!SpotLight->IsVisible());
		}

		if (SpotLight->IsVisible())
		{
			if (LightSound)
			{
				UGameplayStatics::PlaySound2D(this, LightSound);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("Null"));
			}
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABossEvent::BlinkLight, 0.15f, false);
		}
		else
		{
			BlinkCount++;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABossEvent::BlinkLight, 1.5f, false);
		}
	}
	else
	{
		IncreaseLightIntensity();
	}

}

void ABossEvent::IncreaseLightIntensity()
{
	SpotLight->SetIntensity(0.0f);
	SpotLight->SetVisibility(true);

	if (SpotLight)
	{
		GetWorld()->GetTimerManager().SetTimer(LightHandle, this, &ABossEvent::UpdateIntensity, 0.1f, false);
	}

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() 
		{
			BossEnemy->BattleStart(); 
			//プレイヤーの取得
			AVRPlayerCharacter* Player = Cast<AVRPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
			Player->LightControl_Play();
			Player->ItemControl_Play();
		}, 1.5f, false);
}

void ABossEvent::UpdateIntensity()
{
	float CurrentIntensity = SpotLight->Intensity;
	SpotLight->SetIntensity(CurrentIntensity + AddIntensity);
	if (UpdateCount < 20)
	{
		GetWorld()->GetTimerManager().SetTimer(LightHandle, this, &ABossEvent::UpdateIntensity, 0.1f, false);
	}
	UpdateCount++;
}
