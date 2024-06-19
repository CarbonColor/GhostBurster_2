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

	//モーションコントローラーコンポーネントを作成
	RightHandController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightHandController"));
	RightHandController->SetupAttachment(RootComponent);
	RightHandController->SetTrackingSource(EControllerHand::Right);

	//右手のメッシュコンポーネントを作成
	RightHandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightHandMesh"));
	RightHandMesh->SetupAttachment(RightHandController);

	//スポットライトコンポーネントを作成
	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(RightHandController);
	SpotLight->SetVisibility(false);	//最初はOFF設定

	//被ダメージ回数の初期化
	DamageCount = 0;
}

// Called when the game starts or when spawned
void AVRPlayer::BeginPlay()
{
	Super::BeginPlay();

	//デバッグ
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
	
	//デバッグ
	UE_LOG(LogTemp, Warning, TEXT("SetupPlayerInputComponent called"));

	//コントローラーのAボタンにライトのON/OFF操作をバインド
	PlayerInputComponent->BindAction("ToggleFlashLight", IE_Pressed, this, &AVRPlayer::ToggleFlashLight);

	
	//デバッグ
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

		//デバッグ
		UE_LOG(LogTemp, Warning, TEXT("ChangeLight"));
	}
	else
	{
		//デバッグ
		UE_LOG(LogTemp, Warning, TEXT("Not ChangeLight"));
	}
}

float AVRPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//基本クラスの実装を呼び出す
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//被ダメージ回数を加算
	if (ActualDamage > 0.0f)
	{
		DamageCount++;

		//デバッグ
		UE_LOG(LogTemp, Warning, TEXT("Damage taken. DamageCount: %d"), DamageCount);
	}

	return ActualDamage;
}
