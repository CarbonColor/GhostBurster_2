// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FlashLight.h"
#include "Components/SpotLightComponent.h"
#include "Components/InputComponent.h"
#include "MotionControllerComponent.h"

// Sets default values
AFlashLight::AFlashLight()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//モーションコントローラーコンポーネントを作成して構成する
	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	MotionController->SetupAttachment(RootComponent);

	//スポットライトコンポーネントを作成して構成する
	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(MotionController);
	SpotLight->SetVisibility(false);	//最初はオフ状態
}

// Called when the game starts or when spawned
void AFlashLight::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFlashLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFlashLight::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//VRコントローラーのAボタンにToggleFlashLight機能をバインドする
	PlayerInputComponent->BindAction("ToggleFlashLight", IE_Pressed, this, &AFlashLight::ToggleFlashLight);
}

void AFlashLight::ToggleFlashLight()
{
	if (SpotLight)
	{
		SpotLight->SetVisibility(!SpotLight->IsVisible());
	}
}

