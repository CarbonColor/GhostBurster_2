// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/VRPlayerCharacter.h"
#include "Components/SpotLightComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MotionControllerComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Kismet/KismetStringLibrary.h"
#include "Enemy/Enemys.h"
#include "Player/PlayerSplinePath.h"
#include "Kismet/GameplayStatics.h"
#include "Haptics/HapticFeedbackEffect_Base.h"

// Sets default values
AVRPlayerCharacter::AVRPlayerCharacter()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // コリジョン関係なく常に生成するようにする
    this->SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // VRRootコンポーネントを作る
    VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
    // Rootコンポーネントに設定
    RootComponent = VRRoot;

    // モーションコントローラーコンポーネント(右手)を作る
    MotionController_Right = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController_Right"));
    // VRRootコンポーネントにアタッチする
    MotionController_Right->SetupAttachment(VRRoot);

    // スポットライトコンポーネントを作る
    Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
    // 右手にアタッチする
    Flashlight->SetupAttachment(MotionController_Right);
    // 光を消す
    Flashlight->SetVisibility(true);
    // 光の強さ・範囲の調整をする
    Flashlight->SetIntensity(20000.0f);  // Unitless状態での数値
    Flashlight->SetAttenuationRadius(1500.0f);
    Flashlight->SetOuterConeAngle(25.0f);

    //ボックスコリジョンを作る
    PlayerCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("PlayerCollision"));
    //ライトにアタッチしてみる
    PlayerCollision->SetupAttachment(VRRoot);
    PlayerCollision->SetBoxExtent(FVector(-50.0f, 0.0f, 50.0f));
    PlayerCollision->SetCollisionProfileName("PlayerTrigger");

    // スタティックメッシュコンポーネント(ライトコリジョン)を作る
    LightCollision = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightCollision"));
    // 当たり判定用のメッシュを読み込んで LightCollision に設定する
    UStaticMesh* ConeMesh = LoadObject<UStaticMesh>(NULL, TEXT("/Game/_TeamFolder/Player/SM_Cone"), NULL, LOAD_None, NULL);
    LightCollision->SetStaticMesh(ConeMesh);
    // ライトにアタッチする
    LightCollision->SetupAttachment(Flashlight);
    // メッシュを見えないようにさせる
    LightCollision->SetVisibility(false);
    ////コリジョンのプリセットを設定
    //LightCollision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    // 位置・サイズ・向きの調整をする
    LightCollision->SetRelativeLocation(FVector(700.0f, 0.0f, 0.0f));
    LightCollision->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));  // ※ FRotator は (Y, Z, X) の順
    LightCollision->SetRelativeScale3D(FVector(12.5f, 12.5f, 12.5f));
    // 当たり判定のメソッドをバインド
    LightCollision->OnComponentBeginOverlap.AddDynamic(this, &AVRPlayerCharacter::OnConeBeginOverlap);
    LightCollision->OnComponentEndOverlap.AddDynamic(this, &AVRPlayerCharacter::OnConeEndOverlap);

    // ライトの色を設定する
    Flashlight_Color = EFlashlight_Color::White;
    // バッテリーの初期値
    Battery = MaxBattery;
    //ライトのON/OFF切り替えを可能の状態にする
    CanToggleLight = true;
    // 攻撃力を設定する
    Attack = 1;
    // ダメージカウントを初期化する
    DamageCount = 0;
    // 無敵時間の初期化
    DamageNow = false;

    // Tickを止める
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;
    //// Tickを始める
    //PrimaryActorTick.bCanEverTick = true;
    //PrimaryActorTick.bStartWithTickEnabled = true;

    ////Hapticフィードバックのエフェクトを初期化
    //static ConstructorHelpers::FObjectFinder<UHapticFeedbackEffect_Base>HapticEffectObject(TEXT("/Game/_TeamFolder/Player/Input/EnemyDamage"));
    //if (HapticEffectObject.Succeeded())
    //{
    //    HapticEffect = HapticEffectObject.Object;
    //    GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Emerald, TEXT("HapticEffect Initialize"));
    //}
}

// Called when the game starts or when spawned
void AVRPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    //SplinePathActorを取得して設定する
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerSplinePath::StaticClass(), FoundActors);
    if (FoundActors.Num() > 0)
    {
        SplinePathActor = Cast<APlayerSplinePath>(FoundActors[0]);
    }


    // Enhanced Input setup
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (!PlayerController)
        {
            UE_LOG(LogTemp, Warning, TEXT("PlayerController is null"));
        }

        // マッピングコンテキストのセット
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            if (!Subsystem)
            {
                UE_LOG(LogTemp, Warning, TEXT("Subsystem is null"));
            }
            else
            {
                Subsystem->AddMappingContext(IMC_Flashlight, 0);
                GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, TEXT("InputMappingContext"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("EnhancedInputLocalPlayerSubsystem is null"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Controller is not a PlayerController"));
    }
}

// Called every frame
void AVRPlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ダメージを与える処理
    for (AActor* Enemy : OverlappingEnemies)
    {
        if (Enemy && Enemy->GetClass()->ImplementsInterface(UDamageInterface::StaticClass()))
        {
            IDamageInterface* DamageInterface = Cast<IDamageInterface>(Enemy);
            if (DamageInterface)
            {
                DamageInterface->RecieveEnemyDamage(Attack, Flashlight_Color);
                //GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, TEXT("Enemy Damage"));
                //OverlappingEnemies.Remove(Enemy);
            }
        }
    }

    //振動の停止
    if (OverlappingEnemies.Num() == 0)
    {
        StopHapticFeedback();
    }

    // バッテリー操作
    if (Flashlight->GetVisibleFlag())    //ライトON
    {
        Battery--;
        //バッテリーが切れたら
        if (Battery <= 0)
        {
            //ライトを切り替える(OFF化)
            Flashlight->SetVisibility(false);
            //充電切れ直後はライトをつけられない
            CanToggleLight = false;
            GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Silver, TEXT("Battery is empty! You can't use Flashlight!"));
        }
    }
    else if (Flashlight->GetVisibleFlag() == false && Battery < MaxBattery) //ライトOFF
    {
        Battery++;
        //ライトがつけられないとき、バッテリーが満タンになったら
        if (CanToggleLight == false && Battery >= MaxBattery)
        {
            //ライトがつけられるようになる
            CanToggleLight = true;
            GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Silver, TEXT("Battery is fill! You can't use Flashlight!"));
        }
    }
    //if (PreBattery != Battery)
    //{
    //    GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Cyan, UKismetStringLibrary::Conv_IntToString(Battery));
    //}
    PreBattery = Battery;

    //PlayerSplinePathに沿って移動
    if (SplinePathActor)
    {
        FVector NewLocation = SplinePathActor->GetLocationAtCurrentDistance();
        SetActorLocation(NewLocation);
    }


}

// Called to bind functionality to input
void AVRPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // 各アクションのバインド
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        if (!EnhancedInputComponent)
        {
            UE_LOG(LogTemp, Warning, TEXT("EnhancedInputComponent is null"));
        }
        else
        {
            EnhancedInputComponent->BindAction(IA_Flashlight_OnOff, ETriggerEvent::Triggered, this, &AVRPlayerCharacter::ToggleFlashlight);
            EnhancedInputComponent->BindAction(IA_Flashlight_ChangeColor, ETriggerEvent::Triggered, this, &AVRPlayerCharacter::ChangeColorFlashlight);
            GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, TEXT("Binding InputAction"));

            //テスト用
            EnhancedInputComponent->BindAction(IA_DebugTest, ETriggerEvent::Triggered, this, &AVRPlayerCharacter::StartHapticFeedback);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerInputComponent is not an EnhancedInputComponent"));
    }
}

//ライトのON/OFFメソッド
void AVRPlayerCharacter::ToggleFlashlight(const FInputActionValue& value)
{
    bool bIsPressed = value.Get<bool>();

    if (bIsPressed && CanToggleLight)
    {
        GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, TEXT("Light ON/OFF"));
        Flashlight->ToggleVisibility();
    }
    else if (CanToggleLight == false)
    {
        GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Silver, TEXT("Battery is Charging! Wait until the battery is full."));
    }

    ////ライトの変更後、OFFの状態ならコーンコリジョンのプリセットを NoCollision にする
    //if (Flashlight->GetVisibleFlag() == false)
    //{
    //    LightCollision->SetCollisionProfileName(TEXT("NoCollision"));
    //}
    ////ONの状態ならコーンコリジョンのプリセットを BlockAllDynamic にする
    //else
    //{
    //    LightCollision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    //}

}

//ライトの色を切り替えるメソッド
void AVRPlayerCharacter::ChangeColorFlashlight(const FInputActionValue& value)
{
    bool bIsPressed = value.Get<bool>();

    // ライトがついているときは変更可能にする
    if (bIsPressed && Flashlight->GetVisibleFlag())
    {
        // ライトの色を保持する変数を変更
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
        // ライトの色を変更
        SettingFlashlightColor();

        GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, TEXT("Light ChangeColor"));
    }
}

//ライトの色を変えるメソッド
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

//当たり判定のメソッド
void AVRPlayerCharacter::OnConeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    //GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, TEXT("Light BeginOverlap Called"));

    // 接触したアクターがオバケかどうか判定する
    if (const AEnemys* Enemy = Cast<AEnemys>(OtherActor))
    {
        //振動の開始
        if (OverlappingEnemies.Num() == 0)
        {
            StartHapticFeedback();
        }
        OverlappingEnemies.Add(OtherActor);
        //GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, TEXT("Enemy is Overlapping"));
    }

    // 接触したアクターが宝箱かどうか判定する



}
void AVRPlayerCharacter::OnConeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    //GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, TEXT("Light EndOverlap Called"));

    // オバケがコリジョンから抜けたかどうか判定する
    if (const AEnemys* enemy = Cast<AEnemys>(OtherActor))
    {
        OverlappingEnemies.Remove(OtherActor);
        //GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, TEXT("Enemy is not Overlapping"));
        
        //振動の停止
        if (OverlappingEnemies.Num() == 0)
        {
            StopHapticFeedback();
        }
    }
}

//オバケからの攻撃を受けた時のメソッド
void AVRPlayerCharacter::RecievePlayerDamage()
{
    DamageCount++;
     if (DamageNow == false)
     {
         // ダメージ回数を増やす
         DamageCount++;
         // 無敵状態にする
         DamageNow = true;
         //無敵時間の設定

     }
}

//無敵時間のメソッド


//振動を開始する関数
void AVRPlayerCharacter::StartHapticFeedback()
{
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        PlayerController->PlayHapticEffect(HapticEffect, EControllerHand::Right, 1.0f, true);

        GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, TEXT("Device Vibration"));
    }
}
// 振動を停止する関数
void AVRPlayerCharacter::StopHapticFeedback()
{
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        PlayerController->StopHapticEffect(EControllerHand::Right);
    }
}


