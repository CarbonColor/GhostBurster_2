// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/VRPlayerCharacter.h"
#include "Components/SpotLightComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"

// Sets default values
AVRPlayerCharacter::AVRPlayerCharacter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//VRRootコンポーネントを作る
	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	RootComponent = VRRoot;

	//スポットライトコンポーネント
	Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
	Flashlight->SetupAttachment(VRRoot);
	Flashlight->ToggleVisibility(false);
}

// Called when the game starts or when spawned
void AVRPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Enhanced Input setup
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC_Flashlight, 0);
		}


		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{
			EnhancedInputComponent->BindAction(IA_Flashlight_OnOff, ETriggerEvent::Triggered, this, &AVRPlayerCharacter::ToggleFlashlight);
		}

	}
}

// Called every frame
void AVRPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
			GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Cyan, TEXT("Screen Message"));
		Flashlight->ToggleVisibility();
	}

}