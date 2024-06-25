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

	//コリジョン関係なく常に生成するようにする
	this->SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

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
	//Flashlight->SetVisibility(false);
	Flashlight->SetVisibility(true);
	//光の強さ・範囲の調整をする
	Flashlight->SetIntensity(6250.0f);	// Unitless状態での数値
	Flashlight->SetAttenuationRadius(800.0f);
	Flashlight->SetOuterConeAngle(25.0f);


	//スタティックメッシュコンポーネントを作る
	LightCollision = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightCollision"));
	//当たり判定用のメッシュを読み込んで LightCollision に設定する
	UStaticMesh* Mesh = LoadObject<UStaticMesh>(NULL, TEXT("/Game/_TeamFolder/Player/SM_Cone"), NULL, LOAD_None, NULL);
	LightCollision->SetStaticMesh(Mesh);
	//ライトにアタッチする
	LightCollision->SetupAttachment(Flashlight);
	//メッシュを見えないようにさせる
	LightCollision->SetVisibility(false);
	//位置・サイズ・向きの調整をする
	LightCollision->SetRelativeLocation(FVector(400.0f, 0.0f, 0.0f));
	LightCollision->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));	//※ FRotator は (Y, Z, X) の順
	LightCollision->SetRelativeScale3D(FVector(8.0f, 8.0f, 8.0f));
	//当たり判定のメソッドをバインド
	LightCollision->OnComponentBeginOverlap.AddDynamic(this, &AVRPlayerCharacter::OnConeBeginOverlap);
	LightCollision->OnComponentEndOverlap.AddDynamic(this, &AVRPlayerCharacter::OnConeEndOverlap);

	//ライトの色を設定する
	Flashlight_Color = EFlashlight_Color::White;
	//攻撃力を設定する
	Attack = 1;
	//ダメージカウントを初期化する
	DamageCount = 0;
	//無敵状態をfalseにする
	DamageNow = false;

}

// Called when the game starts or when spawned
void AVRPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Enhanced Input setup
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		//マッピングコンテキストのセット
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC_Flashlight, 0);
		}
		//各アクションのバインド
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

	//ダメージを与える処理
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
		//ライトの色を保持する変数を変更
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
		//ライトの色を変更
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
	
	//接触したアクターがオバケかどうか判定する
	if (const ANormalEnemy* enemy = Cast<ANormalEnemy>(OtherActor))
	{
		OverlappingEnemies.Add(OtherActor);
		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, TEXT("Enemy is Overlapping"));
	}
}

void AVRPlayerCharacter::OnConeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, TEXT("Light EndOverlap Called"));

	//オバケがコリジョンから抜けたかどうか判定する
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
		//ダメージ回数を増やす
		DamageCount++;
		//無敵状態にする
		DamageNow = true;
		//5秒後に無敵状態が消えるようにセットする
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