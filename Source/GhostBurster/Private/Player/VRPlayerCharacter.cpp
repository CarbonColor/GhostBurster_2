// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/VRPlayerCharacter.h"
#include "Components/SpotLightComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MotionControllerComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Enemy/NormalEnemy.h"
#include "TimerManager.h"

// Sets default values
AVRPlayerCharacter::AVRPlayerCharacter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//�R���W�����֌W�Ȃ���ɐ�������悤�ɂ���
	this->SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	//VRRoot�R���|�[�l���g�����
	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	//Root�R���|�[�l���g�ɐݒ�
	RootComponent = VRRoot;

	//���[�V�����R���g���[���[�R���|�[�l���g(�E��)�����
	MotionController_Right = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController_Right"));
	//VRRoot�R���|�[�l���g�ɃA�^�b�`����
	MotionController_Right->SetupAttachment(VRRoot);

	//�X�|�b�g���C�g�R���|�[�l���g�����
	Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
	//�E��ɃA�^�b�`����
	Flashlight->SetupAttachment(MotionController_Right);
	//��������
	//Flashlight->SetVisibility(false);
	Flashlight->SetVisibility(true);
	//���̋����E�͈͂̒���������
	Flashlight->SetIntensity(6250.0f);	// Unitless��Ԃł̐��l
	Flashlight->SetAttenuationRadius(800.0f);
	Flashlight->SetOuterConeAngle(25.0f);


	//�X�^�e�B�b�N���b�V���R���|�[�l���g�����
	LightCollision = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightCollision"));
	//�����蔻��p�̃��b�V����ǂݍ���� LightCollision �ɐݒ肷��
	UStaticMesh* Mesh = LoadObject<UStaticMesh>(NULL, TEXT("/Game/_TeamFolder/Player/SM_Cone"), NULL, LOAD_None, NULL);
	LightCollision->SetStaticMesh(Mesh);
	//���C�g�ɃA�^�b�`����
	LightCollision->SetupAttachment(Flashlight);
	//���b�V���������Ȃ��悤�ɂ�����
	LightCollision->SetVisibility(false);
	//�ʒu�E�T�C�Y�E�����̒���������
	LightCollision->SetRelativeLocation(FVector(400.0f, 0.0f, 0.0f));
	LightCollision->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));	//�� FRotator �� (Y, Z, X) �̏�
	LightCollision->SetRelativeScale3D(FVector(8.0f, 8.0f, 8.0f));
	//�����蔻��̃��\�b�h���o�C���h
	LightCollision->OnComponentBeginOverlap.AddDynamic(this, &AVRPlayerCharacter::OnConeBeginOverlap);
	LightCollision->OnComponentEndOverlap.AddDynamic(this, &AVRPlayerCharacter::OnConeEndOverlap);

	//���C�g�̐F��ݒ肷��
	Flashlight_Color = EFlashlight_Color::White;
	//�U���͂�ݒ肷��
	Attack = 1;
	//�_���[�W�J�E���g������������
	DamageCount = 0;
	//���G��Ԃ�false�ɂ���
	DamageNow = false;

}

// Called when the game starts or when spawned
void AVRPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Enhanced Input setup
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		//�}�b�s���O�R���e�L�X�g�̃Z�b�g
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC_Flashlight, 0);
		}
		//�e�A�N�V�����̃o�C���h
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{
			EnhancedInputComponent->BindAction(IA_Flashlight_OnOff, ETriggerEvent::Triggered, this, &AVRPlayerCharacter::ToggleFlashlight);
			EnhancedInputComponent->BindAction(IA_Flashlight_ChangeColor, ETriggerEvent::Triggered, this, &AVRPlayerCharacter::ChangeColorFlashlight);
		}
	}
}

// Called every frame
void AVRPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//�_���[�W��^���鏈��
	for (AActor* Enemy : OverlappingEnemies)
	{
		if (Enemy && Enemy->GetClass()->ImplementsInterface(UDamageInterface::StaticClass()))
		{
			IDamageInterface* DamageInterface = Cast<IDamageInterface>(Enemy);
			if (DamageInterface)
			{
				DamageInterface->RecieveEnemyDamage(Attack);
			}
		}
	}
}

// Called to bind functionality to input
void AVRPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	//{
	//	EnhancedInputComponent->BindAction(IA_Flashlight_OnOff, ETriggerEvent::Triggered, this, &AVRPlayerCharacter::ToggleFlashlight);
	//}

}

void AVRPlayerCharacter::ToggleFlashlight(const FInputActionValue& value)
{
	bool bIsPressed = value.Get<bool>();

	if (bIsPressed)
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, TEXT("Light ON/OFF"));
		Flashlight->ToggleVisibility();
	}
}

void AVRPlayerCharacter::ChangeColorFlashlight(const FInputActionValue& value)
{
	bool bIsPressed = value.Get<bool>();

	if (bIsPressed)
	{
		//���C�g�̐F��ێ�����ϐ���ύX
		switch (Flashlight_Color)
		{
		case EFlashlight_Color::White:
			Flashlight_Color = EFlashlight_Color::Green;
			break;

		case EFlashlight_Color::Green:
			Flashlight_Color = EFlashlight_Color::Red;
			break;

		case EFlashlight_Color::Red:
			Flashlight_Color = EFlashlight_Color::Blue;
			break;

		case EFlashlight_Color::Blue:
			Flashlight_Color = EFlashlight_Color::White;
			break;
		}
		//���C�g�̐F��ύX
		SettingFlashlightColor();

		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, TEXT("Light ChangeColor"));
	}
}

void AVRPlayerCharacter::SettingFlashlightColor()
{
	switch (Flashlight_Color)
	{
	case EFlashlight_Color::White:
		Flashlight->SetLightColor(FColor::White);
		break;

	case EFlashlight_Color::Green:
		Flashlight->SetLightColor(FColor::Green);
		break;

	case EFlashlight_Color::Red:
		Flashlight->SetLightColor(FColor::Red);
		break;

	case EFlashlight_Color::Blue:
		Flashlight->SetLightColor(FColor::Blue);
		break;
	}
}


void AVRPlayerCharacter::OnConeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, TEXT("Light BeginOverlap Called"));
	
	//�ڐG�����A�N�^�[���I�o�P���ǂ������肷��
	if (const ANormalEnemy* enemy = Cast<ANormalEnemy>(OtherActor))
	{
		OverlappingEnemies.Add(OtherActor);
		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, TEXT("Enemy is Overlapping"));
	}
}

void AVRPlayerCharacter::OnConeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, TEXT("Light EndOverlap Called"));

	//�I�o�P���R���W�������甲�������ǂ������肷��
	if (const ANormalEnemy* enemy = Cast<ANormalEnemy>(OtherActor))
	{
		OverlappingEnemies.Remove(OtherActor);
		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, TEXT("Enemy is not Overlapping"));
	}
}

void AVRPlayerCharacter::RecievePlayerDamage()
{
	if (DamageNow == false)
	{
		//�_���[�W�񐔂𑝂₷
		DamageCount++;
		//���G��Ԃɂ���
		DamageNow = true;
		//5�b��ɖ��G��Ԃ�������悤�ɃZ�b�g����
		FTimerManager& TimerManager = GetWorldTimerManager();
		TimerManager.SetTimer(TimerHandle, this, &AVRPlayerCharacter::EndTimeHandle, 5.0f, false);

		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Green, TEXT("Player Damage"));
	}
}

void AVRPlayerCharacter::EndTimeHandle()
{
	DamageNow = false;
	GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Green, TEXT("End of InvincibleTime"));
}