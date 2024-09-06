// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/VRPlayerCharacter.h"
#include "Player/PlayerSplinePath.h"
#include "Components/SpotLightComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MotionControllerComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/Enemys.h"
#include "Player/TreasureBox.h"
#include "Spawn/EnemySpawner.h"
#include "Title/TitleEnemy.h"
#include "Title/TitleEventManager.h"
#include "Haptics/HapticFeedbackEffect_Base.h"

DEFINE_LOG_CATEGORY_STATIC(PlayerScript, Log, All);


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

    //カメラコンポーネントの作成
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(VRRoot);

    //ウィジェットコンポーネントの作成
    PlayerStatusWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PlayerStatusWidgetComponent"));
    PlayerStatusWidgetComponent->SetupAttachment(CameraComponent);
    //ウィジェットの設定
    UClass* WidgetClass = LoadObject<UClass>(nullptr, TEXT("/Game/_TeamFolder/UI/UI_PlayerStatus.UI_PlayerStatus_C"));
    PlayerStatusWidgetComponent->SetWidgetClass(WidgetClass);
    PlayerStatusWidgetComponent->SetWidgetSpace(EWidgetSpace::World);

    // モーションコントローラーコンポーネント(右手)を作る
    MotionController_Right = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController_Right"));
    // VRRootコンポーネントにアタッチする
    MotionController_Right->SetupAttachment(VRRoot);

    // モーションコントローラーコンポーネント(左手)を作る
    MotionController_Left = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController_Left"));
    // VRRootコンポーネントにアタッチする
    MotionController_Left->SetupAttachment(VRRoot);

    //左手のメッシュを作る
    HandMesh_Left = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandMesh_Left"));
    //メッシュを読み込んでセットする
    USkeletalMesh* HandMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/_TeamFolder/Player/SKM_LeftHand"));
    HandMesh_Left->SetSkeletalMesh(HandMesh);
    //左手にアタッチする
    HandMesh_Left->SetupAttachment(MotionController_Left);

    // スポットライトコンポーネントを作る
    Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
    // 右手にアタッチする
    Flashlight->SetupAttachment(MotionController_Right);
    // 光を消しておく
    Flashlight->SetVisibility(false);
    // 光の位置を調整
    Flashlight->SetRelativeLocation(FVector(0.0f, 0.0f, -50.0f));
    // 光の強さ・範囲の調整をする
    Flashlight->SetIntensity(20000.0f);  // Unitless状態での数値
    Flashlight->SetAttenuationRadius(1500.0f);
    Flashlight->SetOuterConeAngle(25.0f);

    // 右手のライトのスタティックメッシュコンポーネントを作る
    FlashlightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlashlightMesh"));
    // 懐中電灯のメッシュを読み込んでセットする
    UStaticMesh* LightMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/_TeamFolder/Player/SM_Light"));
    FlashlightMesh->SetStaticMesh(LightMesh);
    // 右手にアタッチする
    FlashlightMesh->SetupAttachment(Flashlight);
    // メッシュの当たり判定をなくす
    FlashlightMesh->SetCollisionProfileName(TEXT("NoCollision"));
    // メッシュの影をなくす
    FlashlightMesh->SetCastShadow(false);

    //ボックスコリジョンを作る
    PlayerCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("PlayerCollision"));
    //プレイヤーにアタッチ
    PlayerCollision->SetupAttachment(VRRoot);
    PlayerCollision->SetBoxExtent(FVector(-50.0f, 0.0f, 50.0f));
    PlayerCollision->SetCollisionProfileName("PlayerTrigger");

    // スタティックメッシュコンポーネント(ライトコリジョン)を作る
    LightCollision = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightCollision"));
    // 当たり判定用のメッシュを読み込んで LightCollision に設定する
    UStaticMesh* ConeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/_TeamFolder/Player/SM_Cone"));
    LightCollision->SetStaticMesh(ConeMesh);
    // ライトにアタッチする
    LightCollision->SetupAttachment(Flashlight);
    // メッシュを見えないようにさせる
    LightCollision->SetVisibility(false);
    //コリジョンのプリセットを設定
    LightCollision->SetCollisionProfileName(TEXT("NoCollision"));
    // 位置・サイズ・向きの調整をする
    LightCollision->SetRelativeLocation(FVector(700.0f, 0.0f, 0.0f));
    LightCollision->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));  // ※ FRotator は (Y, Z, X) の順
    LightCollision->SetRelativeScale3D(FVector(12.5f, 12.5f, 12.5f));
    // 当たり判定のメソッドをバインド
    LightCollision->OnComponentBeginOverlap.AddDynamic(this, &AVRPlayerCharacter::OnConeBeginOverlap);
    LightCollision->OnComponentEndOverlap.AddDynamic(this, &AVRPlayerCharacter::OnConeEndOverlap);

    ////// Tickを止める
    ////PrimaryActorTick.bCanEverTick = false;
    ////PrimaryActorTick.bStartWithTickEnabled = false;
    //// Tickを始める
    //PrimaryActorTick.bCanEverTick = true;
    //PrimaryActorTick.bStartWithTickEnabled = true;

    //Hapticフィードバックのエフェクトを初期化
    UHapticFeedbackEffect_Base* Haptic_ED = LoadObject<UHapticFeedbackEffect_Base>(nullptr, TEXT("/Game/_TeamFolder/Player/Input/EnemyDamage"));
    HapticEffect_EnemyDamage = Haptic_ED;
    UHapticFeedbackEffect_Base* Haptic_PD = LoadObject<UHapticFeedbackEffect_Base>(nullptr, TEXT("/Game/_TeamFolder/Player/Input/PlayerDamage"));
    HapticEffect_PlayerDamage = Haptic_PD;

    //SEのロード
    EnemyHitSound = LoadObject<USoundCue>(nullptr, TEXT("/Game/_TeamFolder/Sound/SE/SE_EnemyHit_Cue"));
    LightSwitchSound = LoadObject<USoundCue>(nullptr, TEXT("/Game/_TeamFolder/Sound/SE/SE_LightSwitch_Cue"));
    PlayerDamageSound = LoadObject<USoundCue>(nullptr, TEXT("/Game/_TeamFolder/Sound/SE/SE_PlayerDamage_Cue"));
    UseAttackItemSound = LoadObject<USoundCue>(nullptr, TEXT("/Game/_TeamFolder/Sound/SE/SE_UseAttackItem_Cue"));
    UseBuffItemSound = LoadObject<USoundCue>(nullptr, TEXT("/Game/_TeamFolder/Sound/SE/SE_UseBuffItem_Cue"));

    //オーディオコンポーネントの作成
    EnemyDamageSoundEffect = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
    EnemyDamageSoundEffect->bAutoActivate = false;     //自動再生を無効にする
    EnemyDamageSoundEffect->SetupAttachment(VRRoot);
    if (EnemyHitSound)
    {
        EnemyDamageSoundEffect->SetSound(EnemyHitSound);
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Silver, TEXT("Not SoundEffect ! -EnemyHitSound-"));
    }

}

// Called when the game starts or when spawned
void AVRPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    //レベル名を取得する
    UWorld* World = GEngine->GetWorldFromContextObjectChecked(this);
    LevelName = UGameplayStatics::GetCurrentLevelName(World);

    // ------------------------------------------------------------------------------------
    // 変更可能な初期値設定
    // ------------------------------------------------------------------------------------

    // ライトバッテリーの秒数設定　※タイトルの時は緩くする
    BatteryTime = 15;
    if (LevelName == "Title")
    {
        BatteryTime *= 2;
    }
    // バッテリー秒数の増加率設定
    AddBatteryTime = 5;
    // 最大値をセット
    MaxBattery = 60 * BatteryTime;
    // ライトの攻撃力設定
    LightAttack = 1;
    // ライトの攻撃力増加率の設定
    AddLightAttack = 2;

    // アイテムの攻撃力の設定
    ItemAttack = 100;
    // アイテム使用のボーダー設定
    FingerBendingBorder = 350;

    // デバッグ
    DebugTimer = 0;

    // ------------------------------------------------------------------------------------
    // 変更不可能な初期値設定
    // ------------------------------------------------------------------------------------

    // ライトの色を設定する
    Flashlight_Color = EFlashlight_Color::White;
    // バッテリーの初期値
    Battery = MaxBattery;
    //ステージ番号を初期化する
    StageNumber = 1;
    //ライトのON/OFF切り替えを可能の状態にする
    bCanToggleLight = true;
    //アイテムの使用状態を可能にする
    bCanUseItem = true;
    // 無敵時間の初期化
    bIsDamageNow = false;
    //振動状態の初期化
    bIsEnemyHaptic = false;
    //アイテムのボーダー
    AttackItemBorder = { 450, 0, 450, 450, 0 };
    BuffItemBorder = { 0, 0, 450, 450, 450 };

    //スコアのインスタンスを取得する
    ScoreInstance = Cast<UPlayerScoreInstance>(GetGameInstance());
    ScoreInstance->AllDataResetFunction();

    //SplinePathActorを取得して設定する
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerSplinePath::StaticClass(), FoundActors);
    if (FoundActors.Num() > 0)
    {
        SplinePathActor = Cast<APlayerSplinePath>(FoundActors[0]);
    }

    // Enhanced Input setup
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
    // 右手のInputMappingContextを追加
    Subsystem->AddMappingContext(IMC_Flashlight, 0);
    // 左手のInputMappingContextを追加
    Subsystem->AddMappingContext(IMC_GloveDevice, 1);


    // Widgetの表示
    PlayerStatusWidgetComponent->InitWidget();
    UUserWidget* Widget = PlayerStatusWidgetComponent->GetUserWidgetObject();
    BatteryUI = Cast<UProgressBar>(Widget->GetWidgetFromName(TEXT("LightBattery")));
    ScoreUI = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("Score")));
    ItemUI = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("ItemNum")));
    // Widgetの更新
    UpdateBatteryUI();
    UpdateItemUI();
    UpdateScoreUI();
}

// Called every frame
void AVRPlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    //ダメージが入る敵だけ抽出
    TmpOverlapEnemies = OverlappingEnemies;
    DamageEnemies.Empty();
    for (AActor* Enemy : TmpOverlapEnemies)
    {
        if (!Enemy)
        {
            GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Red, TEXT("OverlapEnemy is Null"));
            continue;
        }
        //タイトルの場合
        if (LevelName == "Title")
        {
            ATitleEnemy* CheckEnemy = Cast<ATitleEnemy>(Enemy);
            if (CheckEnemy && CheckEnemy->CheckPlayerLightColor(Flashlight_Color))
            {
                DamageEnemies.Add(Enemy);
            }
        }
        //ゲームシーンの場合
        else if (LevelName == "MainLevel")
        {
            AEnemys* CheckEnemy = Cast<AEnemys>(Enemy);
            if (CheckEnemy && CheckEnemy->CheckPlayerLightColor(Flashlight_Color))
            {
                DamageEnemies.Add(Enemy);
            }
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, TEXT("Not Any Level ! -Overlap-"));
        }
    }

    //振動処理が全くないときに敵にダメージが入っていたら振動を開始
    if (bIsEnemyHaptic == false && bIsPlayerHaptic == false)
    {
        if (DamageEnemies.Num() > 0)
        {
            StartHaptic_EnemyDamage();
            EnemyDamageSoundEffect->Play();
        }
    }
    else if (bIsEnemyHaptic)
    {
        if (DamageEnemies.Num() <= 0)
        {
            StopHapticEffect();
            EnemyDamageSoundEffect->Stop();
        }
    }

    //ダメージを与える
    TmpDamageEnemies = DamageEnemies;
    for (AActor* Enemy : TmpDamageEnemies)
    {
        if (!Enemy)
        {
            GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Red, TEXT("DamageEnemy is Null"));
            continue;
        }
        else if (Enemy->GetClass()->ImplementsInterface(UDamageInterface::StaticClass()))
        {
            IDamageInterface* DamageInterface = Cast<IDamageInterface>(Enemy);
            if (DamageInterface)
            {
                DamageInterface->RecieveEnemyDamage(LightAttack);
            }
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, TEXT("Not Any Level ! -Damage-"));
        }
    }

    // バッテリー操作
    if (bCanToggleLight == false)    //ライトがつけられないとき
    {
        //バッテリーの回復
        Battery += MaxBattery / (60 * 2);
        //UIバーの色を赤くする
        BatteryUI->SetFillColorAndOpacity(FLinearColor::Red);
        if (Battery >= MaxBattery)
        {
            Battery = MaxBattery;
            //ライトがつけられるようになる
            bCanToggleLight = true;
            //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Battery is fill! You can't use Flashlight!"));
        }
        UpdateBatteryUI();
    }
    else if (Flashlight->GetVisibleFlag())    //ライトON
    {
        Battery--;
        //UIバーの色を白くする
        BatteryUI->SetFillColorAndOpacity(FLinearColor::White);
        //バッテリーが切れたら
        if (Battery <= 0)
        {
            //ライトを切り替える(OFF化)
            Flashlight->SetVisibility(false);
            //ライトの当たり判定を無効化
            LightCollision->SetCollisionProfileName(TEXT("NoCollision"));
            //充電切れ直後はライトをつけられない
            bCanToggleLight = false;
            //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Battery is empty! You can't use Flashlight!"));
        }
        UpdateBatteryUI();
    }
    else if (Flashlight->GetVisibleFlag() == false) //ライトOFF
    {
        //バッテリーの回復
        if (Battery < MaxBattery)
        {
            Battery += MaxBattery / (60 * 5);
        }
        //UIバーの色を白くする
        BatteryUI->SetFillColorAndOpacity(FLinearColor::White);
        UpdateBatteryUI();
    }

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
        // 右手のアクションをバインド
        EnhancedInputComponent->BindAction(IA_Flashlight_OnOff, ETriggerEvent::Triggered, this, &AVRPlayerCharacter::ToggleFlashlight);
        EnhancedInputComponent->BindAction(IA_Flashlight_ChangeColor, ETriggerEvent::Triggered, this, &AVRPlayerCharacter::ChangeColorFlashlight);

        // 左手のアクションをバインド
        EnhancedInputComponent->BindAction(IA_Glove_UseAttackItem, ETriggerEvent::Triggered, this, &AVRPlayerCharacter::UseItem_Attack);
        EnhancedInputComponent->BindAction(IA_Glove_UseBuffItem, ETriggerEvent::Triggered, this, &AVRPlayerCharacter::UseItem_Buff);
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

    if (bIsPressed && bCanToggleLight)
    {
        //ライトのON/OFFを切り替えて、SEを鳴らす
        Flashlight->ToggleVisibility();
        if (LightSwitchSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, LightSwitchSound, GetActorLocation());
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Silver, TEXT("Not SoundEffect ! -LightSwitchSound-"));
        }
        //ライトの電源変更後、ONになったなら
        if (Flashlight->GetVisibleFlag())
        {
            //ライトの当たり判定を有効化
            LightCollision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
        }
        else
        {
            //ライトの当たり判定を無効化
            LightCollision->SetCollisionProfileName(TEXT("NoCollision"));
        }
        //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Light ON/OFF"));
    }
    //else if (bCanToggleLight == false)
    //{
    //    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Battery is Charging! Wait until the battery is full."));
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
        //SEを鳴らす
        if (LightSwitchSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, LightSwitchSound, GetActorLocation());
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Silver, TEXT("Not SoundEffect ! -LightSwitchSound-"));
        }


        //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Light ChangeColor"));
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

void AVRPlayerCharacter::CheckUsedItem(const TArray<int32> value)
{
    //FString DebugValue = "";
    //for (int32 v : value)
    //{
    //    DebugValue += FString::FromInt(v) + ", ";
    //}
    //GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Red, DebugValue);

    //使えない状態のときは即リターン
    if (bCanUseItem == false || ScoreInstance->GetPlayerItemCount() <= 0)
    {
        return;
    }

    //狐の形（親指[0]・中指[2]・薬指[3]）
    if (value[0] > FingerBendingBorder &&
        value[1] <= FingerBendingBorder &&
        value[2] > FingerBendingBorder &&
        value[3] > FingerBendingBorder &&
        value[4] <= FingerBendingBorder)
    {
        //攻撃アイテムの処理
        UseItem_Attack();
    }
    //銃の形（中指[2]・薬指[3]・小指[4]）
    else if (value[0] <= FingerBendingBorder &&
             value[1] <= FingerBendingBorder &&
             value[2] > FingerBendingBorder &&
             value[3] > FingerBendingBorder &&
             value[4] > FingerBendingBorder)
    {
        //強化アイテムの処理
        UseItem_Buff();
    }
}
//アイテム使用メソッド
void AVRPlayerCharacter::UseItem_Attack()
{
    if (ScoreInstance->GetPlayerItemCount() <= 0)
    {
        return;
    }

    //デバッグ
    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Used Item ( Enemy Damage )"));

    //狐のモデルの出現

    //SEを鳴らす
    if (UseAttackItemSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, UseAttackItemSound, GetActorLocation());
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Silver, TEXT("Not SoundEffect ! -UseAttackItemSound-"));
    }

    //場にいるすべての敵にダメージを与える
    //ゲーム画面での処理
    if (LevelName == "MainLevel")
    {
        TArray<AActor*> Enemies;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemys::StaticClass(), Enemies);
        for (AActor* Enemy : Enemies)
        {
            if (Enemy && Enemy->GetClass()->ImplementsInterface(UDamageInterface::StaticClass()))
            {
                IDamageInterface* DamageInterface = Cast<IDamageInterface>(Enemy);
                if (DamageInterface)
                {
                    DamageInterface->RecieveEnemyDamage(ItemAttack);
                }
            }
        }
    }
    //タイトル画面での処理
    else if(LevelName == "Title")
    {
        TArray<AActor*> TitleEnemies;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATitleEnemy::StaticClass(), TitleEnemies);
        for (AActor* Enemy : TitleEnemies)
        {
            if (Enemy && Enemy->GetClass()->ImplementsInterface(UDamageInterface::StaticClass()))
            {
                IDamageInterface* DamageInterface = Cast<IDamageInterface>(Enemy);
                if (DamageInterface)
                {
                    DamageInterface->RecieveEnemyDamage(ItemAttack);
                }
            }
        }
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, TEXT("Not Any Level !"));
    }

    
    //タイトル画面での処理
    ATitleEventManager* EventManager = Cast<ATitleEventManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ATitleEventManager::StaticClass()));
    if (EventManager)
    {
        EventManager->IsUseAttackItem();
    }
    //アイテム使用処理（クールタイムや所有数減少など）
    UseItem();
}
void AVRPlayerCharacter::UseItem_Buff()
{
    if (ScoreInstance->GetPlayerItemCount() <= 0)
    {
        return;
    }

    //デバッグ
    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Used Item ( Light Enhanced )"));

    //SEを鳴らす
    if (UseBuffItemSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, UseBuffItemSound, GetActorLocation());
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Silver, TEXT("Not SoundEffect ! -UseBuffItemSound-"));
    }

    //ライトのバッテリー時間を増加
    BatteryTime += AddBatteryTime;
    //ライトの攻撃力を増加
    LightAttack += AddLightAttack;
    //最大値の再設定
    MaxBattery = 60 * BatteryTime;

    //タイトル画面での処理
    if (LevelName == "Title")
    {
        ATitleEventManager* EventManager = Cast<ATitleEventManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ATitleEventManager::StaticClass()));
        if (EventManager)
        {
            EventManager->IsUseBuffItem();
        }
    }
    //アイテム使用処理（クールタイムや所有数減少など）
    UseItem();
}

//アイテムを使用したときに行うメソッド
void AVRPlayerCharacter::UseItem()
{
    //アイテムを使えない状態にする
    bCanUseItem = false;
    //アイテムのクールタイムの設定
    GetWorld()->GetTimerManager().SetTimer(ItemCoolTimeHandle, this, &AVRPlayerCharacter::ItemCoolTimeFunction, 5.0f, false);
    //デバイスに振動要請を送る
    GloveDeviceVibration_UseItem();
    //UIの更新
    ScoreInstance->UsePlayerItem();
    UpdateItemUI();
}
//アイテムのクールタイムメソッド
void AVRPlayerCharacter::ItemCoolTimeFunction()
{
    bCanUseItem = true;
    // タイマーをクリア
    GetWorld()->GetTimerManager().ClearTimer(ItemCoolTimeHandle);

    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("You Can Use Item"));

}


//当たり判定のメソッド
void AVRPlayerCharacter::OnConeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ////とりあえず当たり判定
    //// 接触したアクターがオバケかどうか判定する
    //if (const AEnemys* Enemy = Cast<AEnemys>(OtherActor))
    //{
    //    OverlappingEnemies.Add(OtherActor);
    //    //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Enemy is Overlapping"));
    //}
    ////チュートリアル用の敵に関する当たり判定処理
    //if (const ATitleEnemy* TitleEnemy = Cast<ATitleEnemy>(OtherActor))
    //{
    //    OverlappingEnemies.Add(OtherActor);
    //    //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Enemy is Overlapping"));
    //}

    //壁貫通をなくす処理(β版)
    //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Light BeginOverlap Called (%s)"), *OtherActor->GetActorNameOrLabel()));
    FHitResult HitResult = CheckHitEnemy(OtherActor);

    // 接触したアクターがオバケかどうか判定する
    if (const AEnemys* Enemy = Cast<AEnemys>(OtherActor))
    {
        //間に壁がないかどうかを調べる
        if (HitResult.GetActor() == Enemy)
        {
            OverlappingEnemies.Add(OtherActor);
            FString Debug = "Overlap (" + OtherActor->GetName() + ")";
            GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, Debug);
        }
    }
    //チュートリアル用の敵に関する当たり判定処理
    if (const ATitleEnemy* TitleEnemy = Cast<ATitleEnemy>(OtherActor))
    {
        //間に壁がないかどうか調べる
        if (HitResult.GetActor() == TitleEnemy)
        {
            OverlappingEnemies.Add(OtherActor);
            //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Enemy is Overlapping"));
        }
    }
    if (ATreasureBox* Treasure = Cast<ATreasureBox>(OtherActor))
    {
        if (HitResult.GetActor() == Treasure)
        {
            if (Treasure->IsOpenedTreasure() == false)
            {
                Treasure->OpenTreasureBox();
            }
        }
    }

    
}
// 当たり判定の壁貫通をなくす処理
FHitResult AVRPlayerCharacter::CheckHitEnemy(AActor* OtherActor)
{
    // 光源座標(Coneの先端)からコリジョンが当たった場所に向けてRayを飛ばす
    FVector LightPoint = Flashlight->GetComponentLocation();
    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    //無視する当たり判定
    CollisionParams.AddIgnoredActor(this);
    if (OtherActor->ActorHasTag("IgnoreActor"))
    {
        FString Debug = "Ignoring Actor with Tag (" + OtherActor->GetName() + ")";
        // Debug message to confirm tag presence
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, Debug);
        CollisionParams.AddIgnoredActor(OtherActor);
    }

    // ObjectTypeの設定
    FCollisionObjectQueryParams ObjectParams;
    ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);

    // Rayを飛ばす
    bool bHit = GetWorld()->LineTraceSingleByObjectType(
        HitResult,
        LightPoint,
        OtherActor->GetActorLocation(),
        ObjectParams,
        CollisionParams
    );

    if (bHit)
    {
        //// Rayを表示する
        //DrawDebugLine(
        //    GetWorld(),
        //    LightPoint,
        //    OtherActor->GetActorLocation(),
        //    FColor::Emerald,    // 線の色
        //    false,              // 永続的に描画しない（一定時間後に消える）
        //    2.0f,               // 描画時間（秒）
        //    0,
        //    2.0f                // 線の太さ
        //);

        //GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, HitResult.GetActor()->GetName());
        return HitResult;
    }

    return FHitResult();
}
//bool AVRPlayerCharacter::IgnoreActorClass(const AActor* Actor)
//{
//    if (Actor->ActorHasTag("IgnoreActor"))
//    {
//        return true;
//    }
//    return false;
//}

void AVRPlayerCharacter::OnConeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Light EndOverlap Called"));

    // オバケがコリジョンから抜けたかどうか判定する
    if (const AEnemys* Enemy = Cast<AEnemys>(OtherActor))
    {
        OverlappingEnemies.Remove(OtherActor);
        //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Enemy is not Overlapping"));
    }
    if (const ATitleEnemy* TitleEnemy = Cast<ATitleEnemy>(OtherActor))
    {
        OverlappingEnemies.Remove(OtherActor);
    }
}

//オバケからの攻撃を受けた時のメソッド
void AVRPlayerCharacter::RecievePlayerDamage()
{
    if (bIsDamageNow == false)
    {
        // ダメージ回数を増やす
        ScoreInstance->AddPlayerDamageCount();
        // 無敵状態にする
        bIsDamageNow = true;
        //無敵時間の設定 (3秒後に無敵状態を解除)
        GetWorld()->GetTimerManager().SetTimer(NoDamageTimerHandle, this, &AVRPlayerCharacter::NoDamageFunction, 3.0f, false);
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Player damage !"));
        //デバイスを振動させる
        StartHaptic_PlayerDamage();
        GloveDeviceVibration_Damage();
        //SEを鳴らす
        if (PlayerDamageSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, PlayerDamageSound, GetActorLocation());
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Silver, TEXT("Not SoundEffect ! -PlayerDamageSound-"));
        }
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Player takes no damage !"));
    }
}
//無敵時間のメソッド
void AVRPlayerCharacter::NoDamageFunction()
{
    // 無敵状態を解除
    bIsDamageNow = false;
    // タイマーをクリア
    GetWorld()->GetTimerManager().ClearTimer(NoDamageTimerHandle);

}

//振動を開始するメソッド
void AVRPlayerCharacter::StartHaptic_EnemyDamage()
{
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        PlayerController->PlayHapticEffect(HapticEffect_EnemyDamage, EControllerHand::Right, 1.0f, true);
        PlayerController->PlayHapticEffect(HapticEffect_EnemyDamage, EControllerHand::Left, 1.0f, true);
        bIsEnemyHaptic = true;
        bIsPlayerHaptic = false;
        //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Device Vibration (Enemy)"));
    }
}
void AVRPlayerCharacter::StartHaptic_PlayerDamage()
{
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        PlayerController->PlayHapticEffect(HapticEffect_PlayerDamage, EControllerHand::Right, 1.0f, false);
        PlayerController->PlayHapticEffect(HapticEffect_PlayerDamage, EControllerHand::Left, 1.0f, true);
        bIsEnemyHaptic = false;
        bIsPlayerHaptic = true;
        GetWorld()->GetTimerManager().SetTimer(HapticTimer, this, &AVRPlayerCharacter::StopHapticEffect, 1.5f, false);
        //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Device Vibration (Player)"));
    }
}
// 振動を停止するメソッド
void AVRPlayerCharacter::StopHapticEffect()
{
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        PlayerController->StopHapticEffect(EControllerHand::Right);
        PlayerController->StopHapticEffect(EControllerHand::Left);
        bIsEnemyHaptic = false;
        bIsPlayerHaptic = false;
    }
}



//ステージ番号を進めるメソッド
void AVRPlayerCharacter::NextStage()
{
    StageNumber++;
}
//ステージ番号を取得するメソッド
int AVRPlayerCharacter::GetStageNumber()
{
    return StageNumber;
}
//ウィジェットのバッテリーを更新するメソッド
void AVRPlayerCharacter::UpdateBatteryUI()
{
    if (BatteryUI)
    {
        BatteryUI->SetPercent(Battery / (float)MaxBattery);
    }
    else
    {
        UE_LOG(PlayerScript, Warning, TEXT("Battery UI is null !"));
    }
}
//ウィジェットのアイテム所有数を更新するメソッド
void AVRPlayerCharacter::UpdateItemUI()
{
    if (ItemUI)
    {
        int32 Item = ScoreInstance->GetPlayerItemCount();
        ItemUI->SetText(FText::AsNumber(Item));
    }
    else
    {
        UE_LOG(PlayerScript, Warning, TEXT("Item UI is null !"));
    }
}
//ウィジェットのスコアを更新するメソッド
void AVRPlayerCharacter::UpdateScoreUI()
{
    if (ScoreUI)
    {
        int32 Score = ScoreInstance->GetPlayerDefaultScore();
        ScoreUI->SetText(FText::AsNumber(Score));
    }
    else
    {
        UE_LOG(PlayerScript, Warning, TEXT("Score UI is null !"));
    }
}

//アイテムを増やすメソッド
void AVRPlayerCharacter::AddItem()
{
    ScoreInstance->AddPlayerItem();
    UpdateItemUI();
}
//スコアを増やすメソッド
void AVRPlayerCharacter::AddScore(int32 Value)
{
    ScoreInstance->AddPlayerScore(Value);
    UpdateScoreUI();
}

//余ったアイテムをスコアに変換するメソッド
void AVRPlayerCharacter::ChangeScore()
{
    if (ScoreInstance->GetPlayerItemCount() > 0)
    {
        GetWorld()->GetTimerManager().SetTimer(ScoreChangeHandle, this, &AVRPlayerCharacter::ChangeScore_Step, 0.5f, true);
    }
}
void AVRPlayerCharacter::ChangeScore_Step()
{
    if (ScoreInstance->GetPlayerItemCount() > 0)
    {
        AddScore(ScoreInstance->GetItemPerScore());
        ScoreInstance->UsePlayerItem();
        UpdateItemUI();
    }
    else
    {
        GetWorld()->GetTimerManager().ClearTimer(ScoreChangeHandle);
    }
}
