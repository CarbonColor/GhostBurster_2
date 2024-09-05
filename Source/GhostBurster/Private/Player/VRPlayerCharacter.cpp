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

    //�E�B�W�F�b�g�R���|�[�l���g�̍쐬
    PlayerStatusWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PlayerStatusWidgetComponent"));
    PlayerStatusWidgetComponent->SetupAttachment(CameraComponent);
    //�E�B�W�F�b�g�̐ݒ�
    UClass* WidgetClass = LoadObject<UClass>(nullptr, TEXT("/Game/_TeamFolder/UI/UI_PlayerStatus.UI_PlayerStatus_C"));
    PlayerStatusWidgetComponent->SetWidgetClass(WidgetClass);
    PlayerStatusWidgetComponent->SetWidgetSpace(EWidgetSpace::World);

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
    // ���̋����E�͈͂̒���������
    Flashlight->SetIntensity(20000.0f);  // Unitless��Ԃł̐��l
    Flashlight->SetAttenuationRadius(1500.0f);
    Flashlight->SetOuterConeAngle(25.0f);

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
    // �ʒu�E�T�C�Y�E�����̒���������
    LightCollision->SetRelativeLocation(FVector(700.0f, 0.0f, 0.0f));
    LightCollision->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));  // �� FRotator �� (Y, Z, X) �̏�
    LightCollision->SetRelativeScale3D(FVector(12.5f, 12.5f, 12.5f));
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

    //���x�������擾����
    UWorld* World = GEngine->GetWorldFromContextObjectChecked(this);
    LevelName = UGameplayStatics::GetCurrentLevelName(World);

    // ------------------------------------------------------------------------------------
    // �ύX�\�ȏ����l�ݒ�
    // ------------------------------------------------------------------------------------

    // ���C�g�o�b�e���[�̕b���ݒ�@���^�C�g���̎��͊ɂ�����
    BatteryTime = 15;
    if (LevelName == "Title")
    {
        BatteryTime *= 2;
    }
    // �o�b�e���[�b���̑������ݒ�
    AddBatteryTime = 5;
    // �ő�l���Z�b�g
    MaxBattery = 60 * BatteryTime;
    // ���C�g�̍U���͐ݒ�
    LightAttack = 1;
    // ���C�g�̍U���͑������̐ݒ�
    AddLightAttack = 2;

    // �A�C�e���̍U���͂̐ݒ�
    ItemAttack = 100;
    // �A�C�e���g�p�̃{�[�_�[�ݒ�
    FingerBendingBorder = 350;

    // �f�o�b�O
    DebugTimer = 0;

    // ------------------------------------------------------------------------------------
    // �ύX�s�\�ȏ����l�ݒ�
    // ------------------------------------------------------------------------------------

    // ���C�g�̐F��ݒ肷��
    Flashlight_Color = EFlashlight_Color::White;
    // �o�b�e���[�̏����l
    Battery = MaxBattery;
    //�X�e�[�W�ԍ�������������
    StageNumber = 1;
    //���C�g��ON/OFF�؂�ւ����\�̏�Ԃɂ���
    bCanToggleLight = true;
    //�A�C�e���̎g�p��Ԃ��\�ɂ���
    bCanUseItem = true;
    // ���G���Ԃ̏�����
    bIsDamageNow = false;
    //�U����Ԃ̏�����
    bIsEnemyHaptic = false;
    //�A�C�e���̃{�[�_�[
    AttackItemBorder = { 450, 0, 450, 450, 0 };
    BuffItemBorder = { 0, 0, 450, 450, 450 };

    //�X�R�A�̃C���X�^���X���擾����
    ScoreInstance = Cast<UPlayerScoreInstance>(GetGameInstance());
    ScoreInstance->AllDataResetFunction();

    //SplinePathActor���擾���Đݒ肷��
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerSplinePath::StaticClass(), FoundActors);
    if (FoundActors.Num() > 0)
    {
        SplinePathActor = Cast<APlayerSplinePath>(FoundActors[0]);
    }

    // Enhanced Input setup
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
    // �E���InputMappingContext��ǉ�
    Subsystem->AddMappingContext(IMC_Flashlight, 0);
    // �����InputMappingContext��ǉ�
    Subsystem->AddMappingContext(IMC_GloveDevice, 1);


    // Widget�̕\��
    PlayerStatusWidgetComponent->InitWidget();
    UUserWidget* Widget = PlayerStatusWidgetComponent->GetUserWidgetObject();
    BatteryUI = Cast<UProgressBar>(Widget->GetWidgetFromName(TEXT("LightBattery")));
    ScoreUI = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("Score")));
    ItemUI = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("ItemNum")));
    // Widget�̍X�V
    UpdateBatteryUI();
    UpdateItemUI();
    UpdateScoreUI();
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

    // �o�b�e���[����
    if (bCanToggleLight == false)    //���C�g�������Ȃ��Ƃ�
    {
        //�o�b�e���[�̉�
        Battery += MaxBattery / (60 * 2);
        //UI�o�[�̐F��Ԃ�����
        BatteryUI->SetFillColorAndOpacity(FLinearColor::Red);
        if (Battery >= MaxBattery)
        {
            Battery = MaxBattery;
            //���C�g��������悤�ɂȂ�
            bCanToggleLight = true;
            //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Battery is fill! You can't use Flashlight!"));
        }
        UpdateBatteryUI();
    }
    else if (Flashlight->GetVisibleFlag())    //���C�gON
    {
        Battery--;
        //UI�o�[�̐F�𔒂�����
        BatteryUI->SetFillColorAndOpacity(FLinearColor::White);
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
            Battery += MaxBattery / (60 * 5);
        }
        //UI�o�[�̐F�𔒂�����
        BatteryUI->SetFillColorAndOpacity(FLinearColor::White);
        UpdateBatteryUI();
    }

    //PlayerSplinePath�ɉ����Ĉړ�
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

    // �e�A�N�V�����̃o�C���h
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        // �E��̃A�N�V�������o�C���h
        EnhancedInputComponent->BindAction(IA_Flashlight_OnOff, ETriggerEvent::Triggered, this, &AVRPlayerCharacter::ToggleFlashlight);
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
void AVRPlayerCharacter::ToggleFlashlight(const FInputActionValue& value)
{
    bool bIsPressed = value.Get<bool>();

    if (bIsPressed && bCanToggleLight)
    {
        //���C�g��ON/OFF��؂�ւ��āASE��炷
        Flashlight->ToggleVisibility();
        if (LightSwitchSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, LightSwitchSound, GetActorLocation());
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Silver, TEXT("Not SoundEffect ! -LightSwitchSound-"));
        }
        //���C�g�̓d���ύX��AON�ɂȂ����Ȃ�
        if (Flashlight->GetVisibleFlag())
        {
            //���C�g�̓����蔻���L����
            LightCollision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
        }
        else
        {
            //���C�g�̓����蔻��𖳌���
            LightCollision->SetCollisionProfileName(TEXT("NoCollision"));
        }
        //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Light ON/OFF"));
    }
    //else if (bCanToggleLight == false)
    //{
    //    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Battery is Charging! Wait until the battery is full."));
    //}
}

//���C�g�̐F��؂�ւ��郁�\�b�h
void AVRPlayerCharacter::ChangeColorFlashlight(const FInputActionValue& value)
{
    bool bIsPressed = value.Get<bool>();

    // ���C�g�����Ă���Ƃ��͕ύX�\�ɂ���
    if (bIsPressed && Flashlight->GetVisibleFlag())
    {
        // ���C�g�̐F��ێ�����ϐ���ύX
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

    //�g���Ȃ���Ԃ̂Ƃ��͑����^�[��
    if (bCanUseItem == false || ScoreInstance->GetPlayerItemCount() <= 0)
    {
        return;
    }

    //�ς̌`�i�e�w[0]�E���w[2]�E��w[3]�j
    if (value[0] > FingerBendingBorder &&
        value[1] <= FingerBendingBorder &&
        value[2] > FingerBendingBorder &&
        value[3] > FingerBendingBorder &&
        value[4] <= FingerBendingBorder)
    {
        //�U���A�C�e���̏���
        UseItem_Attack();
    }
    //�e�̌`�i���w[2]�E��w[3]�E���w[4]�j
    else if (value[0] <= FingerBendingBorder &&
             value[1] <= FingerBendingBorder &&
             value[2] > FingerBendingBorder &&
             value[3] > FingerBendingBorder &&
             value[4] > FingerBendingBorder)
    {
        //�����A�C�e���̏���
        UseItem_Buff();
    }
}
//�A�C�e���g�p���\�b�h
void AVRPlayerCharacter::UseItem_Attack()
{
    if (ScoreInstance->GetPlayerItemCount() <= 0)
    {
        return;
    }

    //�f�o�b�O
    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Used Item ( Enemy Damage )"));

    //�ς̃��f���̏o��

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

    
    //�^�C�g����ʂł̏���
    ATitleEventManager* EventManager = Cast<ATitleEventManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ATitleEventManager::StaticClass()));
    if (EventManager)
    {
        EventManager->IsUseAttackItem();
    }
    //�A�C�e���g�p�����i�N�[���^�C���⏊�L�������Ȃǁj
    UseItem();
}
void AVRPlayerCharacter::UseItem_Buff()
{
    if (ScoreInstance->GetPlayerItemCount() <= 0)
    {
        return;
    }

    //�f�o�b�O
    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Used Item ( Light Enhanced )"));

    //SE��炷
    if (UseBuffItemSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, UseBuffItemSound, GetActorLocation());
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Silver, TEXT("Not SoundEffect ! -UseBuffItemSound-"));
    }

    //���C�g�̃o�b�e���[���Ԃ𑝉�
    BatteryTime += AddBatteryTime;
    //���C�g�̍U���͂𑝉�
    LightAttack += AddLightAttack;
    //�ő�l�̍Đݒ�
    MaxBattery = 60 * BatteryTime;

    //�^�C�g����ʂł̏���
    if (LevelName == "Title")
    {
        ATitleEventManager* EventManager = Cast<ATitleEventManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ATitleEventManager::StaticClass()));
        if (EventManager)
        {
            EventManager->IsUseBuffItem();
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

    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("You Can Use Item"));

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

    //�Ǌђʂ��Ȃ�������(����)
    //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Light BeginOverlap Called (%s)"), *OtherActor->GetActorNameOrLabel()));
    FHitResult HitResult = CheckHitEnemy(OtherActor);

    // �ڐG�����A�N�^�[���I�o�P���ǂ������肷��
    if (const AEnemys* Enemy = Cast<AEnemys>(OtherActor))
    {
        //�Ԃɕǂ��Ȃ����ǂ����𒲂ׂ�
        if (HitResult.GetActor() == Enemy)
        {
            OverlappingEnemies.Add(OtherActor);
            FString Debug = "Overlap (" + OtherActor->GetName() + ")";
            GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, Debug);
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
        FString Debug = "Ignoring Actor with Tag (" + OtherActor->GetName() + ")";
        // Debug message to confirm tag presence
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, Debug);
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
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Player damage !"));
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
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Player takes no damage !"));
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

//�A�C�e���𑝂₷���\�b�h
void AVRPlayerCharacter::AddItem()
{
    ScoreInstance->AddPlayerItem();
    UpdateItemUI();
}
//�X�R�A�𑝂₷���\�b�h
void AVRPlayerCharacter::AddScore(int32 Value)
{
    ScoreInstance->AddPlayerScore(Value);
    UpdateScoreUI();
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
    }
    else
    {
        GetWorld()->GetTimerManager().ClearTimer(ScoreChangeHandle);
    }
}
