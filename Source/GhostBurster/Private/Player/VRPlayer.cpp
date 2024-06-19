// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/VRPlayer.h"
#include "Components/SpotLightComponent.h"
#include "Components/InputComponent.h"
//#include "MotionControllerComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"

// Sets default values
AVRPlayer::AVRPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//���[�V�����R���g���[���[�R���|�[�l���g���쐬
	RightHandController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightHandController"));
	RightHandController->SetupAttachment(RootComponent);
	RightHandController->SetTrackingSource(EControllerHand::Right);

	//�E��̃��b�V���R���|�[�l���g���쐬
	RightHandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightHandMesh"));
	RightHandMesh->SetupAttachment(RightHandController);

	//�X�|�b�g���C�g�R���|�[�l���g���쐬
	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(RightHandController);
	SpotLight->SetVisibility(false);	//�ŏ���OFF�ݒ�

	//��_���[�W�񐔂̏�����
	DamageCount = 0;
}

// Called when the game starts or when spawned
void AVRPlayer::BeginPlay()
{
	Super::BeginPlay();

	//�f�o�b�O
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay called"));
}

// Called every frame
void AVRPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AVRPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	//�f�o�b�O
	UE_LOG(LogTemp, Warning, TEXT("SetupPlayerInputComponent called"));

	//�R���g���[���[��A�{�^���Ƀ��C�g��ON/OFF������o�C���h
	PlayerInputComponent->BindAction("ToggleFlashLight", IE_Pressed, this, &AVRPlayer::ToggleFlashLight);

	
	//�f�o�b�O
	if (PlayerInputComponent->GetNumActionBindings() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bindings found %d"), PlayerInputComponent->GetNumActionBindings());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No bindings found"));
	}

	
}

void AVRPlayer::ToggleFlashLight()
{
	if (SpotLight)
	{
		bool NewVisibility = !SpotLight->IsVisible();
		SpotLight->SetVisibility(NewVisibility);		

		//�f�o�b�O
		UE_LOG(LogTemp, Warning, TEXT("ChangeLight"));
	}
	else
	{
		//�f�o�b�O
		UE_LOG(LogTemp, Warning, TEXT("Not ChangeLight"));
	}
}

float AVRPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//��{�N���X�̎������Ăяo��
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//��_���[�W�񐔂����Z
	if (ActualDamage > 0.0f)
	{
		DamageCount++;

		//�f�o�b�O
		UE_LOG(LogTemp, Warning, TEXT("Damage taken. DamageCount: %d"), DamageCount);
	}

	return ActualDamage;
}
