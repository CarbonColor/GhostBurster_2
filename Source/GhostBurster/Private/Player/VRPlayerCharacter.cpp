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

    // �R���W�����֌W�Ȃ���ɐ�������悤�ɂ���
    this->SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // VRRoot�R���|�[�l���g�����
    VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
    // Root�R���|�[�l���g�ɐݒ�
    RootComponent = VRRoot;

    //�J�����R���|�[�l���g�̍쐬
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(VRRoot);

    //�E�B�W�F�b�g�R���|�[�l���g�̍쐬�i�v���C���[UI�j
    PlayerStatusWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PlayerStatusWidgetComponent"));
    PlayerStatusWidgetComponent->SetupAttachment(CameraComponent);
    //�E�B�W�F�b�g�̐ݒ�
    UClass* PlayerWidgetClass = LoadObject<UClass>(nullptr, TEXT("/Game/_TeamFolder/UI/UI_PlayerStatus.UI_PlayerStatus_C"));
    PlayerStatusWidgetComponent->SetWidgetClass(PlayerWidgetClass);
    PlayerStatusWidgetComponent->SetWidgetSpace(EWidgetSpace::World);

    //�E�B�W�F�b�g�R���|�[�l���g�̍쐬�i�t�F�[�hUI�j
    FadeOutWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("FadeOutWidgetComponent"));
    FadeOutWidgetComponent->SetupAttachment(CameraComponent);
    //�E�B�W�F�b�g�̐ݒ�
    UClass* FadeWidgetClass = LoadObject<UClass>(nullptr, TEXT("/Game/_TeamFolder/UI/UI_FadeOut.UI_FadeOut_C"));
    FadeOutWidgetComponent->SetWidgetClass(FadeWidgetClass);
    FadeOutWidgetComponent->SetWidgetSpace(EWidgetSpace::World);

    //�����̃I�[�f�B�I�R���|�[�l���g
    LeftEarComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("LeftEarAudioComponent"));
    LeftEarComponent->SetupAttachment(CameraComponent);
    LeftEarComponent->SetRelativeLocation(FVector(0, -15, 0));
    //�E���̃I�[�f�B�I�R���|�[�l���g
    RightEarComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("RightEarAudioComponent"));
    RightEarComponent->SetupAttachment(CameraComponent);
    LeftEarComponent->SetRelativeLocation(FVector(0, 15, 0));

    // ���[�V�����R���g���[���[�R���|�[�l���g(�E��)�����
    MotionController_Right = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController_Right"));
    // VRRoot�R���|�[�l���g�ɃA�^�b�`����
    MotionController_Right->SetupAttachment(VRRoot);

    // ���[�V�����R���g���[���[�R���|�[�l���g(����)�����
    MotionController_Left = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController_Left"));
    // VRRoot�R���|�[�l���g�ɃA�^�b�`����
    MotionController_Left->SetupAttachment(VRRoot);

    //����̃��b�V�������
    HandMesh_Left = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandMesh_Left"));
    //���b�V����ǂݍ���ŃZ�b�g����
    USkeletalMesh* HandMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/_TeamFolder/Player/SKM_LeftHand"));
    HandMesh_Left->SetSkeletalMesh(HandMesh);
    //����ɃA�^�b�`����
    HandMesh_Left->SetupAttachment(MotionController_Left);

    // �X�|�b�g���C�g�R���|�[�l���g�����
    Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
    // �E��ɃA�^�b�`����
    Flashlight->SetupAttachment(MotionController_Right);
    // ���������Ă���
    Flashlight->SetVisibility(false);
    // ���̈ʒu�𒲐�
    Flashlight->SetRelativeLocation(FVector(0.0f, 0.0f, -50.0f));

    // �E��̃��C�g�̃X�^�e�B�b�N���b�V���R���|�[�l���g�����
    FlashlightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlashlightMesh"));
    // �����d���̃��b�V����ǂݍ���ŃZ�b�g����
    UStaticMesh* LightMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/_TeamFolder/Player/SM_Light"));
    FlashlightMesh->SetStaticMesh(LightMesh);
    // �E��ɃA�^�b�`����
    FlashlightMesh->SetupAttachment(Flashlight);
    // ���b�V���̓����蔻����Ȃ���
    FlashlightMesh->SetCollisionProfileName(TEXT("NoCollision"));
    // ���b�V���̉e���Ȃ���
    FlashlightMesh->SetCastShadow(false);

    //���C�g�Ŏg���i�C�A�K���R���|�[�l���g�����
    NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
    NiagaraComponent->SetupAttachment(Flashlight);

    //�{�b�N�X�R���W���������
    PlayerCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("PlayerCollision"));
    //�v���C���[�ɃA�^�b�`
    PlayerCollision->SetupAttachment(VRRoot);
    PlayerCollision->SetBoxExtent(FVector(-50.0f, 0.0f, 50.0f));
    PlayerCollision->SetCollisionProfileName("PlayerTrigger");

    // �X�^�e�B�b�N���b�V���R���|�[�l���g(���C�g�R���W����)�����
    LightCollision = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightCollision"));
    // �����蔻��p�̃��b�V����ǂݍ���� LightCollision �ɐݒ肷��
    UStaticMesh* ConeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/_TeamFolder/Player/SM_Cone"));
    LightCollision->SetStaticMesh(ConeMesh);
    // ���C�g�ɃA�^�b�`����
    LightCollision->SetupAttachment(Flashlight);
    // ���b�V���������Ȃ��悤�ɂ�����
    LightCollision->SetVisibility(false);
    //�R���W�����̃v���Z�b�g��ݒ�
    LightCollision->SetCollisionProfileName(TEXT("NoCollision"));
    // �����蔻��̃��\�b�h���o�C���h
    LightCollision->OnComponentBeginOverlap.AddDynamic(this, &AVRPlayerCharacter::OnConeBeginOverlap);
    LightCollision->OnComponentEndOverlap.AddDynamic(this, &AVRPlayerCharacter::OnConeEndOverlap);

    ////// Tick���~�߂�
    ////PrimaryActorTick.bCanEverTick = false;
    ////PrimaryActorTick.bStartWithTickEnabled = false;
    //// Tick���n�߂�
    //PrimaryActorTick.bCanEverTick = true;
    //PrimaryActorTick.bStartWithTickEnabled = true;

    //Haptic�t�B�[�h�o�b�N�̃G�t�F�N�g��������
    UHapticFeedbackEffect_Base* Haptic_ED = LoadObject<UHapticFeedbackEffect_Base>(nullptr, TEXT("/Game/_TeamFolder/Player/Input/EnemyDamage"));
    HapticEffect_EnemyDamage = Haptic_ED;
    UHapticFeedbackEffect_Base* Haptic_PD = LoadObject<UHapticFeedbackEffect_Base>(nullptr, TEXT("/Game/_TeamFolder/Player/Input/PlayerDamage"));
    HapticEffect_PlayerDamage = Haptic_PD;

    //SE�̃��[�h
    EnemyHitSound = LoadObject<USoundCue>(nullptr, TEXT("/Game/_TeamFolder/Sound/SE/SE_EnemyHit_Cue"));
    LightSwitchSound = LoadObject<USoundCue>(nullptr, TEXT("/Game/_TeamFolder/Sound/SE/SE_LightSwitch_Cue"));
    PlayerDamageSound = LoadObject<USoundCue>(nullptr, TEXT("/Game/_TeamFolder/Sound/SE/SE_PlayerDamage_Cue"));
    UseAttackItemSound = LoadObject<USoundCue>(nullptr, TEXT("/Game/_TeamFolder/Sound/SE/SE_UseAttackItem_Cue"));
    UseBuffItemSound = LoadObject<USoundCue>(nullptr, TEXT("/Game/_TeamFolder/Sound/SE/SE_UseBuffItem_Cue"));

    //�I�[�f�B�I�R���|�[�l���g�̍쐬
    EnemyDamageSoundEffect = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
    EnemyDamageSoundEffect->bAutoActivate = false;     //�����Đ��𖳌��ɂ���
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

    //�X�R�A�̃C���X�^���X���擾����
    ScoreInstance = Cast<UPlayerScoreInstance>(GetGameInstance());
    ScoreInstance->AllDataResetFunction();

    //���x�������擾����
    UWorld* World = GEngine->GetWorldFromContextObjectChecked(this);
    LevelName = UGameplayStatics::GetCurrentLevelName(World);

    // ------------------------------------------------------------------------------------
    // �ύX�\�ȏ����l�ݒ�
    // ------------------------------------------------------------------------------------

    // ���C�g�o�b�e���[�̕b���ݒ�@���^�C�g���̎��͊ɂ�����
    BatteryTime = 20;
    if (LevelName == "Title")
    {
        BatteryTime *= 2;
    }
    // �o�b�e���[�b���̑������ݒ�
    AddBatteryTime = 10;
    // �ő�l���Z�b�g
    MaxBattery = 60 * BatteryTime;
    // ���C�g�̍U���͐ݒ�
    LightAttack = 10;
    // ���C�g�̍U���͑������̐ݒ�
    AddLightAttack = 6;

    // �A�C�e���̍U���͂̐ݒ�
    ItemAttack = 60 * LightAttack * 2.5f;

    // ------------------------------------------------------------------------------------
    // �ύX�s�\�ȏ����l�ݒ�
    // ------------------------------------------------------------------------------------

    // ���C�g�̐F��ݒ肷��
    Flashlight->SetLightFColor(FColor::Green);
    Flashlight_Color = EFlashlight_Color::Green;
    LightColor_UI = FLinearColor::Green;
    // �o�b�e���[�̏����l
    Battery = MaxBattery;
    //�X�e�[�W�ԍ�������������
    StageNumber = 1;
    //���C�g���x��������������
    LightLevel = 1;
    //���C�g��ON/OFF�؂�ւ����\�̏�Ԃɂ���
    bCanToggleLight = true;
    //�A�C�e���̎g�p��Ԃ��\�ɂ���
    bCanUseItem = true;
    // ���G���Ԃ̏�����
    bIsDamageNow = false;
    //�U����Ԃ̏�����
    bIsEnemyHaptic = false;

    // Enhanced Input setup
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
    // �E���InputMappingContext��ǉ�
    Subsystem->AddMappingContext(IMC_Flashlight, 0);
    // �����InputMappingContext��ǉ�
    Subsystem->AddMappingContext(IMC_GloveDevice, 1);

    // Widget�̕\��
    PlayerStatusWidgetComponent->InitWidget();
    PlayerWidget = PlayerStatusWidgetComponent->GetUserWidgetObject();
    BatteryUI = Cast<UProgressBar>(PlayerWidget->GetWidgetFromName(TEXT("LightBattery")));
    BatteryUI->SetFillColorAndOpacity(LightColor_UI);
    ScoreUI = Cast<UTextBlock>(PlayerWidget->GetWidgetFromName(TEXT("Score")));
    ItemUI = Cast<UTextBlock>(PlayerWidget->GetWidgetFromName(TEXT("ItemNum")));
    LevelUI = Cast<UTextBlock>(PlayerWidget->GetWidgetFromName(TEXT("LightLevel")));
    // Widget�̍X�V
    UpdateBatteryUI();
    UpdateItemUI();
    UpdateScoreUI();
    UpdateLevelUI();

    //�i�C�A�K���̃Z�b�g
    if(BuffEffectNiagara)
    {
        NiagaraComponent->SetAsset(BuffEffectNiagara);
    }

    if (LevelName == "Title")
    {
        TitleEvent = Cast<ATitleEventManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ATitleEventManager::StaticClass()));
    }

    bIsGameEnd = false;
}

// Called every frame
void AVRPlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    //�_���[�W������G�������o
    TmpOverlapEnemies = OverlappingEnemies;
    DamageEnemies.Empty();
    for (AActor* Enemy : TmpOverlapEnemies)
    {
        if (!Enemy)
        {
            GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Red, TEXT("OverlapEnemy is Null"));
            continue;
        }
        //�^�C�g���̏ꍇ
        if (LevelName == "Title")
        {
            ATitleEnemy* CheckEnemy = Cast<ATitleEnemy>(Enemy);
            if (CheckEnemy && CheckEnemy->CheckPlayerLightColor(Flashlight_Color))
            {
                DamageEnemies.Add(Enemy);
            }
        }
        //�Q�[���V�[���̏ꍇ
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

    //�U���������S���Ȃ��Ƃ��ɓG�Ƀ_���[�W�������Ă�����U�����J�n
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
    //�_���[�W��^����
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

    //�󔠂ɑ΂��鏈��
    for (int i = TreasureBoxes.Num() - 1; i >= 0; --i)
    {
        ATreasureBox* Box = Cast<ATreasureBox>(TreasureBoxes[i]);
        if (!Box)
        {
            GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Red, TEXT("TreasureBox is Null"));
            continue;
        }
        else
        {
            Box->OpenChargeBox();
        }
    }

    // �o�b�e���[����
    if (bCanToggleLight == false)    //���C�g�������Ȃ��Ƃ�
    {
        //���C�g�̓I�t�ɂ���
        Flashlight->SetVisibility(false);
        //�o�b�e���[�̉�
        Battery += MaxBattery / (60 * 1.5f);
        //UI�o�[�̐F���D�F�ɂ���
        BatteryUI->WidgetStyle.BackgroundImage.TintColor = FLinearColor::Black;
        //BatteryUI->SetFillColorAndOpacity(FLinearColor::Black);
        if (Battery >= MaxBattery)
        {
            Battery = MaxBattery;
            //���C�g��������悤�ɂȂ�
            bCanToggleLight = true;
            BatteryUI->WidgetStyle.BackgroundImage.TintColor = FLinearColor::White;
            //BatteryUI->SetFillColorAndOpacity(LightColor_UI);
            
            //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Battery is fill! You can't use Flashlight!"));
        }
        UpdateBatteryUI();
    }
    else if (Flashlight->GetVisibleFlag())    //���C�gON
    {
        Battery--;
        //�o�b�e���[���؂ꂽ��
        if (Battery <= 0)
        {
            //���C�g��؂�ւ���(OFF��)
            Flashlight->SetVisibility(false);
            //���C�g�̓����蔻��𖳌���
            LightCollision->SetCollisionProfileName(TEXT("NoCollision"));
            //�[�d�؂꒼��̓��C�g�������Ȃ�
            bCanToggleLight = false;
            //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Battery is empty! You can't use Flashlight!"));
        }
        UpdateBatteryUI();
    }
    else if (Flashlight->GetVisibleFlag() == false) //���C�gOFF
    {
        //�o�b�e���[�̉�
        if (Battery < MaxBattery)
        {
            Battery += MaxBattery / (60 * 3.5f);
        }
        UpdateBatteryUI();
    }

    if (LevelName == "Title")
    {
        if (TitleEvent->GetCanChangeLight() == false && Flashlight->GetVisibleFlag())
        {
            Flashlight->SetVisibility(false);
            UGameplayStatics::PlaySoundAtLocation(this, LightSwitchSound, GetActorLocation());
            LightCollision->SetCollisionProfileName(TEXT("NoCollision"));
        }
    }

    //��ʊO�̓G��SE�̍X�V���\�b�h
    UpdateViewOutEnemySound();


    ////��ʊO�̓G��\������UI�̍X�V
    //UpdateEnemyIndicators();
}

// Called to bind functionality to input
void AVRPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // �e�A�N�V�����̃o�C���h
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        // �E��̃A�N�V�������o�C���h
        EnhancedInputComponent->BindAction(IA_Flashlight_OnOff, ETriggerEvent::Started, this, &AVRPlayerCharacter::ToggleFlashlight_On);
        EnhancedInputComponent->BindAction(IA_Flashlight_OnOff, ETriggerEvent::Completed, this, &AVRPlayerCharacter::ToggleFlashlight_Off);
        EnhancedInputComponent->BindAction(IA_Flashlight_ChangeColor, ETriggerEvent::Triggered, this, &AVRPlayerCharacter::ChangeColorFlashlight);

        // ����̃A�N�V�������o�C���h
        EnhancedInputComponent->BindAction(IA_Glove_UseAttackItem, ETriggerEvent::Triggered, this, &AVRPlayerCharacter::UseItem_Attack);
        EnhancedInputComponent->BindAction(IA_Glove_UseBuffItem, ETriggerEvent::Triggered, this, &AVRPlayerCharacter::UseItem_Buff);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerInputComponent is not an EnhancedInputComponent"));
    }
}

//���C�g��ON/OFF���\�b�h
void AVRPlayerCharacter::ToggleFlashlight_On(const FInputActionValue& value)
{
    if (LevelName == "Title")
    {
        if (TitleEvent->GetCanChangeLight() == false)
        {
            return;
        }
    }
    if (bIsGameEnd)
    {
        return;
    }
    Flashlight->SetVisibility(true);
    UGameplayStatics::PlaySoundAtLocation(this, LightSwitchSound, GetActorLocation());
    LightCollision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
}
void AVRPlayerCharacter::ToggleFlashlight_Off(const FInputActionValue& value)
{
    if (LevelName == "Title")
    {
        if (TitleEvent->GetCanChangeLight() == false)
        {
            return;
        }
    }
    if (bIsGameEnd)
    {
        return;
    }
    Flashlight->SetVisibility(false);
    UGameplayStatics::PlaySoundAtLocation(this, LightSwitchSound, GetActorLocation());
    LightCollision->SetCollisionProfileName(TEXT("NoCollision"));
}

//���C�g�̐F��؂�ւ��郁�\�b�h
void AVRPlayerCharacter::ChangeColorFlashlight(const FInputActionValue& value)
{
    if (LevelName == "Title")
    {
        if (TitleEvent->GetCanChangeLight() == false)
        {
            return;
        }
    }
    if (bIsGameEnd)
    {
        return;
    }

    bool bIsPressed = value.Get<bool>();

    // ���C�g�����Ă���Ƃ��͕ύX�\�ɂ���
    if (bIsPressed)
    {
        // ���C�g�̐F��ێ�����ϐ���ύX
        switch (Flashlight_Color)
        {
        //case EFlashlight_Color::Yellow:
        //    Flashlight_Color = EFlashlight_Color::Green;
        //    LightColor_UI = FLinearColor::Green;
        //    break;

        case EFlashlight_Color::Green:
            Flashlight_Color = EFlashlight_Color::Red;
            LightColor_UI = FLinearColor::Red;
            break;

        case EFlashlight_Color::Red:
            Flashlight_Color = EFlashlight_Color::Blue;
            LightColor_UI = FLinearColor::Blue;
            break;

        case EFlashlight_Color::Blue:
            Flashlight_Color = EFlashlight_Color::Green;
            LightColor_UI = FLinearColor::Green;
            break;
        }
        // ���C�g�̐F��ύX
        SettingFlashlightColor();
        //SE��炷
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

//���C�g�̐F��ς��郁�\�b�h
void AVRPlayerCharacter::SettingFlashlightColor()
{
    switch (Flashlight_Color)
    {
    case EFlashlight_Color::Yellow:
        Flashlight->SetLightColor(FColor::Yellow);
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
    BatteryUI->SetFillColorAndOpacity(LightColor_UI);
}

void AVRPlayerCharacter::CheckUsedItem(const TArray<int32> value)
{
    //FString DebugValue = "";
    //for (int32 v : value)
    //{
    //    DebugValue += FString::FromInt(v) + ", ";
    //}
    //GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Red, DebugValue);

    //�g���Ȃ���Ԃ̂Ƃ��͑����^�[��
    if (bCanUseItem == false || ScoreInstance->GetPlayerItemCount() <= 0)
    {
        return;
    }

    //�ς̌`�i�e�w[0]�E���w[2]�E��w[3]�j
    if (value[0] > ScoreInstance->FingerBorder[0] &&
        value[1] <= ScoreInstance->FingerBorder[1] &&
        value[2] > ScoreInstance->FingerBorder[2] &&
        value[3] > ScoreInstance->FingerBorder[3] &&
        value[4] <= ScoreInstance->FingerBorder[4])
    {
        //�U���A�C�e���̏���
        UseItem_Attack();
    }
    //�e�̌`�i���w[2]�E��w[3]�E���w[4]�j
    else if (value[0] <= ScoreInstance->FingerBorder[0] &&
             value[1] <= ScoreInstance->FingerBorder[1] &&
             value[2] > ScoreInstance->FingerBorder[2] &&
             value[3] > ScoreInstance->FingerBorder[3] &&
             value[4] > ScoreInstance->FingerBorder[4])
    {
        //�����A�C�e���̏���
        UseItem_Buff();
    }
}
//�A�C�e���g�p���\�b�h
void AVRPlayerCharacter::UseItem_Attack()
{
    if (bCanUseItem == false || ScoreInstance->GetPlayerItemCount() <= 0 || bIsGameEnd)
    {
        return;
    }

    if (LevelName == "Title")
    {
        if (TitleEvent->GetCanUseAttackItem() == false)
        {
            return;
        }
    }

    //�f�o�b�O
    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Used Item ( Enemy Damage )"));


    //�ς̃��f���̏o��
    if (FoxGhostModel)
    {
        FActorSpawnParameters SpawnParams;
        FVector PlayerLocation = GetActorLocation();
        FRotator PlayerRotation = GetActorRotation();

        // �v���C���[�̑O����100���j�b�g�A��������100���j�b�g���炵���ʒu���v�Z
        FVector ForwardVector = PlayerRotation.Vector();  // �v���C���[�̑O������
        FVector RightVector = FRotationMatrix(PlayerRotation).GetUnitAxis(EAxis::Y); // �v���C���[�̉E����
        //�������W�̐ݒ�
        FVector SpawnLocation = PlayerLocation + ForwardVector * 100.0f - RightVector * 100.0f;
        SpawnLocation.Z = 0;
        FRotator SpawnRotation = PlayerRotation + FRotator(0.0f, -90.0f, 0.0f);
        // Fox_BP�����[���h�ɐ���
        AActor* SpawnedFox = GetWorld()->SpawnActor<AActor>(FoxGhostModel, SpawnLocation, SpawnRotation, SpawnParams);
    }

    GetWorld()->GetTimerManager().SetTimer(AttackItemTimeHandle, this, &AVRPlayerCharacter::AttackItemFunction, 1.0f, false);
    //�x�����ɍēx�g��ꂽ�獢��̂�
    bCanUseItem = false;

}
void AVRPlayerCharacter::AttackItemFunction()
{
    //SE��炷
    if (UseAttackItemSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, UseAttackItemSound, GetActorLocation());
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Silver, TEXT("Not SoundEffect ! -UseAttackItemSound-"));
    }

    //��ɂ��邷�ׂĂ̓G�Ƀ_���[�W��^����
    //�Q�[����ʂł̏���
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
    //�^�C�g����ʂł̏���
    else if (LevelName == "Title")
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


    //�^�C�g����ʂł̏���
    if (TitleEvent)
    {
        TitleEvent->IsUseAttackItem();
    }
    //�A�C�e���g�p�����i�N�[���^�C���⏊�L�������Ȃǁj
    UseItem();
}

void AVRPlayerCharacter::UseItem_Buff()
{
    if (bCanUseItem == false || ScoreInstance->GetPlayerItemCount() <= 0 || bIsGameEnd)
    {
        return;
    }

    if (LevelName == "Title")
    {
        if (TitleEvent->GetCanUseBuffItem() == false)
        {
            return;
        }
    }

    //�f�o�b�O
    //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Used Item ( Light Enhanced )"));

    //SE��炷
    if (UseBuffItemSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, UseBuffItemSound, GetActorLocation());
    }
    else
    {
        //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Silver, TEXT("Not SoundEffect ! -UseBuffItemSound-"));
    }

    //�i�C�A�K���G�t�F�N�g����x�����Đ�
    if (NiagaraComponent && BuffEffectNiagara)
    {
        NiagaraComponent->Activate(true); // �Đ����J�n
    }

    //�o�b�e���[�̃��x�����グ��
    LightLevel++;
    //���C�g�̃o�b�e���[���Ԃ𑝉�
    BatteryTime += AddBatteryTime;
    //���C�g�̍U���͂𑝉�
    LightAttack += AddLightAttack;
    //�ő�l�̍Đݒ�
    MaxBattery = 60 * BatteryTime;
    //�o�b�e���[��S��
    Battery = MaxBattery;
    //UI�̍X�V
    UpdateLevelUI();
    UpdateBatteryUI();

    //�^�C�g����ʂł̏���
    if (LevelName == "Title")
    {
        if (TitleEvent)
        {
            TitleEvent->IsUseBuffItem();
        }
    }
    //�A�C�e���g�p�����i�N�[���^�C���⏊�L�������Ȃǁj
    UseItem();
}

//�A�C�e�����g�p�����Ƃ��ɍs�����\�b�h
void AVRPlayerCharacter::UseItem()
{
    //�A�C�e�����g���Ȃ���Ԃɂ���
    bCanUseItem = false;
    //�A�C�e���̃N�[���^�C���̐ݒ�
    GetWorld()->GetTimerManager().SetTimer(ItemCoolTimeHandle, this, &AVRPlayerCharacter::ItemCoolTimeFunction, 5.0f, false);
    //�f�o�C�X�ɐU���v���𑗂�
    GloveDeviceVibration_UseItem();
    //UI�̍X�V
    ScoreInstance->UsePlayerItem();
    UpdateItemUI();
}
//�A�C�e���̃N�[���^�C�����\�b�h
void AVRPlayerCharacter::ItemCoolTimeFunction()
{
    bCanUseItem = true;
    // �^�C�}�[���N���A
    GetWorld()->GetTimerManager().ClearTimer(ItemCoolTimeHandle);

    //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("You Can Use Item"));

}

//�����蔻��̃��\�b�h
void AVRPlayerCharacter::OnConeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ////�Ƃ肠���������蔻��
    //// �ڐG�����A�N�^�[���I�o�P���ǂ������肷��
    //if (const AEnemys* Enemy = Cast<AEnemys>(OtherActor))
    //{
    //    OverlappingEnemies.Add(OtherActor);
    //    //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Enemy is Overlapping"));
    //}
    ////�`���[�g���A���p�̓G�Ɋւ��铖���蔻�菈��
    //if (const ATitleEnemy* TitleEnemy = Cast<ATitleEnemy>(OtherActor))
    //{
    //    OverlappingEnemies.Add(OtherActor);
    //    //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Enemy is Overlapping"));
    //}

    //�Ǌђʂ��Ȃ�������
    //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Light BeginOverlap Called (%s)"), *OtherActor->GetActorNameOrLabel()));
    FHitResult HitResult = CheckHitEnemy(OtherActor);

    //�G�̓����蔻�菈��
    if (const AEnemys* Enemy = Cast<AEnemys>(OtherActor))
    {
        //�Ԃɕǂ��Ȃ����ǂ����𒲂ׂ�
        if (HitResult.GetActor() == Enemy)
        {
            OverlappingEnemies.Add(OtherActor);
            //FString Debug = "Overlap (" + OtherActor->GetName() + ")";
            //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, Debug);
        }
    }
    //�`���[�g���A���p�̓G�Ɋւ��铖���蔻�菈��
    if (const ATitleEnemy* TitleEnemy = Cast<ATitleEnemy>(OtherActor))
    {
        //�Ԃɕǂ��Ȃ����ǂ������ׂ�
        if (HitResult.GetActor() == TitleEnemy)
        {
            OverlappingEnemies.Add(OtherActor);
            //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Enemy is Overlapping"));
        }
    }
    //�󔠂̓����蔻�菈��
    if (ATreasureBox* Treasure = Cast<ATreasureBox>(OtherActor))
    {
        if (HitResult.GetActor() == Treasure)
        {
            TreasureBoxes.Add(OtherActor);
        }
    }

    
}
// �����蔻��̕Ǌђʂ��Ȃ�������
FHitResult AVRPlayerCharacter::CheckHitEnemy(AActor* OtherActor)
{
    // �������W(Cone�̐�[)����R���W���������������ꏊ�Ɍ�����Ray���΂�
    FVector LightPoint = Flashlight->GetComponentLocation();
    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    //�������铖���蔻��
    CollisionParams.AddIgnoredActor(this);
    if (OtherActor->ActorHasTag("IgnoreActor"))
    {
        //FString Debug = "Ignoring Actor with Tag (" + OtherActor->GetName() + ")";
        //GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, Debug);
        CollisionParams.AddIgnoredActor(OtherActor);
    }

    // ObjectType�̐ݒ�
    FCollisionObjectQueryParams ObjectParams;
    ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);

    // Ray���΂�
    bool bHit = GetWorld()->LineTraceSingleByObjectType(
        HitResult,
        LightPoint,
        OtherActor->GetActorLocation(),
        ObjectParams,
        CollisionParams
    );

    if (bHit)
    {
        //// Ray��\������
        //DrawDebugLine(
        //    GetWorld(),
        //    LightPoint,
        //    OtherActor->GetActorLocation(),
        //    FColor::Emerald,    // ���̐F
        //    false,              // �i���I�ɕ`�悵�Ȃ��i��莞�Ԍ�ɏ�����j
        //    2.0f,               // �`�掞�ԁi�b�j
        //    0,
        //    2.0f                // ���̑���
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

    // �I�o�P���R���W�������甲�������ǂ������肷��
    if (const AEnemys* Enemy = Cast<AEnemys>(OtherActor))
    {
        OverlappingEnemies.Remove(OtherActor);
        //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Enemy is not Overlapping"));
    }
    if (const ATitleEnemy* TitleEnemy = Cast<ATitleEnemy>(OtherActor))
    {
        OverlappingEnemies.Remove(OtherActor);
    }
    if (ATreasureBox* Treasure = Cast<ATreasureBox>(OtherActor))
    {
        TreasureBoxes.Remove(OtherActor);
    }
}

//��ʊO�̓G�����m���郁�\�b�h
void AVRPlayerCharacter::UpdateViewOutEnemySound()
{
    FVector PlayerLocation = GetActorLocation();
    FRotator PlayerRotation = GetActorRotation();
}

//�I�o�P����̍U�����󂯂����̃��\�b�h
void AVRPlayerCharacter::RecievePlayerDamage()
{
    if (bIsDamageNow == false)
    {
        // �_���[�W�񐔂𑝂₷
        ScoreInstance->AddPlayerDamageCount();
        // ���G��Ԃɂ���
        bIsDamageNow = true;
        //���G���Ԃ̐ݒ� (3�b��ɖ��G��Ԃ�����)
        GetWorld()->GetTimerManager().SetTimer(NoDamageTimerHandle, this, &AVRPlayerCharacter::NoDamageFunction, 3.0f, false);
        //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Player damage !"));
        //�f�o�C�X��U��������
        StartHaptic_PlayerDamage();
        GloveDeviceVibration_Damage();
        //SE��炷
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
        //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Player takes no damage !"));
    }
}
//���G���Ԃ̃��\�b�h
void AVRPlayerCharacter::NoDamageFunction()
{
    // ���G��Ԃ�����
    bIsDamageNow = false;
    // �^�C�}�[���N���A
    GetWorld()->GetTimerManager().ClearTimer(NoDamageTimerHandle);

}

//�U�����J�n���郁�\�b�h
void AVRPlayerCharacter::StartHaptic_EnemyDamage()
{
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        PlayerController->PlayHapticEffect(HapticEffect_EnemyDamage, EControllerHand::Right, 1.0f, true);
        PlayerController->PlayHapticEffect(HapticEffect_EnemyDamage, EControllerHand::Left, 1.0f, true);
        bIsEnemyHaptic = true;
        //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Device Vibration (Enemy)"));
    }
}
void AVRPlayerCharacter::StartHaptic_PlayerDamage()
{
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        PlayerController->PlayHapticEffect(HapticEffect_PlayerDamage, EControllerHand::Right, 1.0f, false);
        PlayerController->PlayHapticEffect(HapticEffect_PlayerDamage, EControllerHand::Left, 1.0f, true);
        bIsPlayerHaptic = true;
        GetWorld()->GetTimerManager().SetTimer(HapticTimer, this, &AVRPlayerCharacter::StopHapticEffect, 1.5f, false);
        //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Device Vibration (Player)"));
    }
}
// �U�����~���郁�\�b�h
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

//�X�e�[�W�ԍ���i�߂郁�\�b�h
void AVRPlayerCharacter::NextStage()
{
    StageNumber++;
}
//�X�e�[�W�ԍ����擾���郁�\�b�h
int AVRPlayerCharacter::GetStageNumber()
{
    return StageNumber;
}
//�E�B�W�F�b�g�̃o�b�e���[���X�V���郁�\�b�h
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
//�E�B�W�F�b�g�̃A�C�e�����L�����X�V���郁�\�b�h
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
//�E�B�W�F�b�g�̃X�R�A���X�V���郁�\�b�h
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
//�E�B�W�F�b�g�̃��C�g���x�����X�V���郁�\�b�h
void AVRPlayerCharacter::UpdateLevelUI()
{
    if (LevelUI)
    {
        LevelUI->SetText(FText::AsNumber(LightLevel));
    }
    else
    {
        UE_LOG(PlayerScript, Warning, TEXT("Level UI is null !"));
    }
}

//��ʊO�̓G��\������UI���X�V���郁�\�b�h
void AVRPlayerCharacter::UpdateEnemyIndicators()
{
    ////�����̃C���W�P�[�^�[���N���A
    //ClearAllIndicators();

    ////�S�Ă̓G���擾
    //TArray<AActor*> Enemies;
    //UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemys::StaticClass(), Enemies);

    //for (AActor* EnemyActor : Enemies)
    //{
    //    AEnemys* Enemy = Cast<AEnemys>(EnemyActor);
    //    if (Enemy && !Enemy->WasRecentlyRendered(0.1f))
    //    {
    //        FVector DirectionToEnemy = Enemy->GetActorLocation() - GetActorLocation();
    //        float DotProduct = FVector::DotProduct(GetActorForwardVector(), DirectionToEnemy.GetSafeNormal());

    //        bool bIsLeftSide = FVector::DotProduct(GetActorRightVector(), DirectionToEnemy.GetSafeNormal()) < 0;

    //        //�G�����E�O���ǂ������m�F���K�؂�UI��\��
    //        if (DotProduct < 0)  //�G���w��ɂ���ꍇ�̂�
    //        {
    //            AddIndicator(bIsLeftSide, Enemy->GetEnemyColor());
    //        }
    //    }
    //}

}
//�����̃C���W�P�[�^�[���N���A���郁�\�b�h
void AVRPlayerCharacter::ClearAllIndicators()
{
    //for (UEnemyDirectionWIdget* Widget : LeftIndicators)
    //{
    //    Widget->RemoveFromParent();
    //}
    //LeftIndicators.Empty();
    //for (UEnemyDirectionWIdget* Widget : RightIndicators)
    //{
    //    Widget->RemoveFromParent();
    //}
    //RightIndicators.Empty();
}
//�C���W�P�[�^�[��ǉ����郁�\�b�h
void AVRPlayerCharacter::AddIndicator(bool bIsLeftSide, int32 EnemyColor)
{
    //if (EnemyDirectionWidgetClass)
    //{
    //    UEnemyDirectionWIdget* NewWidget = CreateWidget<UEnemyDirectionWIdget>(GetWorld(), EnemyDirectionWidgetClass);
    //    if (NewWidget)
    //    {
    //        NewWidget->SetEnemyColor(EnemyColor);
    //        NewWidget->AddToViewport();

    //        if (bIsLeftSide)
    //        {
    //            LeftIndicators.Add(NewWidget);
    //            PositionIndicator(NewWidget, true, LeftIndicators.Num() - 1);
    //        }
    //        else
    //        {
    //            RightIndicators.Add(NewWidget);
    //            PositionIndicator(NewWidget, false, RightIndicators.Num() - 1);
    //        }
    //    }
    //}
}
//�C���W�P�[�^�[�̏ꏊ��ݒ肷�郁�\�b�h
void AVRPlayerCharacter::PositionIndicator(UEnemyDirectionWIdget* IndicatorWidget, bool bIsLeftSide, int32 PositionIndex)
{
    //FVector2D ViewportSize;
    //GEngine->GameViewport->GetViewportSize(ViewportSize);

    //float YPos = (ViewportSize.Y / (bIsLeftSide ? LeftIndicators.Num() : RightIndicators.Num())) * (PositionIndex + 1);

    ////��ʂ̍����E�Ɉʒu�𒲐�
    //FVector2D IndicatorPosition = bIsLeftSide ? FVector2D(50.0f, YPos) : FVector2D(ViewportSize.X - 50.0f, YPos);
    //IndicatorWidget->SetPositionInViewport(IndicatorPosition);
}

//�A�C�e���𑝂₷���\�b�h
void AVRPlayerCharacter::AddItem()
{
    if (ScoreInstance)
    {
        ScoreInstance->AddPlayerItem();
        UpdateItemUI();
    }
    else
    {
        UE_LOG(PlayerScript, Warning, TEXT("ScoreInstance is Null -AddItem-"));
    }
}
//�X�R�A�𑝂₷���\�b�h
void AVRPlayerCharacter::AddScore(int32 Value)
{
    if (ScoreInstance)
    {
        ScoreInstance->AddPlayerScore(Value);
        UpdateScoreUI();
    }
    else
    {
        UE_LOG(PlayerScript, Warning, TEXT("ScoreInstance is Null -AddScore-"));
    }

}

//�]�����A�C�e�����X�R�A�ɕϊ����郁�\�b�h
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
        if (ChangeScoreSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, ChangeScoreSound, GetActorLocation());
        }
    }
    else
    {
        GetWorld()->GetTimerManager().ClearTimer(ScoreChangeHandle);
    }
}

void AVRPlayerCharacter::GameEndSet()
{
    bIsGameEnd = true;
    Flashlight->SetVisibility(false);
    PlayerStatusWidgetComponent->SetHiddenInGame(true);
}