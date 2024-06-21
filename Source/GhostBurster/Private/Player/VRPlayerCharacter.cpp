// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/VRPlayerCharacter.h"
#include "Components/SpotLightComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MotionControllerComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"

// Sets default values
AVRPlayerCharacter::AVRPlayerCharacter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//VRRootコンポーネントを作る
	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	//Rootコンポーネントに設定
	RootComponent = VRRoot;

	//モーションコントローラーコンポーネント(右手)を作る
	MotionController_Right = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController_Right"));
	//VRRootコンポーネントにアタッチする
	MotionController_Right->SetupAttachment(VRRoot);

	//スポットライトコンポーネントを作る
	Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
	//右手にアタッチする
	Flashlight->SetupAttachment(MotionController_Right);
	//光を消す
	Flashlight->ToggleVisibility(false);

	//スタティックメッシュコンポーネントを作る
	LightCollision = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightCollision"));
	//当たり判定用のメッシュを読み込んで LightCollision に設定する
	UStaticMesh* Mesh = LoadObject<UStaticMesh>(NULL, TEXT("/Game/_TeamFolder/Player/SM_Cone"), NULL, LOAD_None, NULL);
	LightCollision->SetStaticMesh(Mesh);
	//ライトにアタッチする
	LightCollision->SetupAttachment(Flashlight);
	//メッシュを見えないようにさせる
	//LightCollision->ToggleVisibility(false);
	
	
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