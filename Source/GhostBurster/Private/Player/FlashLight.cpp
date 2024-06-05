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

	//���[�V�����R���g���[���[�R���|�[�l���g���쐬���č\������
	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	MotionController->SetupAttachment(RootComponent);

	//�X�|�b�g���C�g�R���|�[�l���g���쐬���č\������
	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(MotionController);
	SpotLight->SetVisibility(false);	//�ŏ��̓I�t���
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

	//VR�R���g���[���[��A�{�^����ToggleFlashLight�@�\���o�C���h����
	PlayerInputComponent->BindAction("ToggleFlashLight", IE_Pressed, this, &AFlashLight::ToggleFlashLight);
}

void AFlashLight::ToggleFlashLight()
{
	if (SpotLight)
	{
		SpotLight->SetVisibility(!SpotLight->IsVisible());
	}
}

