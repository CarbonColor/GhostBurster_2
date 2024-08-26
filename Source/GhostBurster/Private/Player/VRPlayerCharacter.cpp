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
#include "Title/TitleEnemy.h"
#include "Title/TitleEventManager.h"
#include "Haptics/HapticFeedbackEffect_Base.h"

DEFINE_LOG_CATEGORY_STATIC(PlayerScript, Log, All);


// Sets default values
AVRPlayerCharacter::AVRPlayerCharacter()
{
    // ------------------------------------------------------------------------------------
    // �ύX�\�ȏ����l�ݒ�
    // ------------------------------------------------------------------------------------
    
    // ���C�g�o�b�e���[�̕b���ݒ�
    BatteryTime = 10;
    // �o�b�e���[�b���̑������ݒ�
    AddBatteryTime = 5;
    // �ő�l���Z�b�g
    MaxBattery = 60 * BatteryTime;
    // ���C�g�̍U���͐ݒ�
    LightAttack = 1;
    // ���C�g�̍U���͑������̐ݒ�
    AddLightAttack = 2;

    // �A�C�e�����̏����l
    ItemCount = 2;
    // �A�C�e���̍U���͂̐ݒ�
    ItemAttack = 50;
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

    // ------------------------------------------------------------------------------------
    // �R���|�[�l���g�֌W
    // ------------------------------------------------------------------------------------

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

    // �X�|�b�g���C�g�R���|�[�l���g�����
    Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
    // �E��ɃA�^�b�`����
    Flashlight->SetupAttachment(MotionController_Right);
    // ��������
    Flashlight->SetVisibility(true);
    // ���̋����E�͈͂̒���������
    Flashlight->SetIntensity(20000.0f);  // Unitless��Ԃł̐��l
    Flashlight->SetAttenuationRadius(1500.0f);
    Flashlight->SetOuterConeAngle(25.0f);

    // �E��̃��C�g�̃X�^�e�B�b�N���b�V���R���|�[�l���g�����
    FlashlightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlashlightMesh"));
    // �����d���̃��b�V����ǂݍ���ŃZ�b�g����
    UStaticMesh* LightMesh = LoadObject<UStaticMesh>(NULL, TEXT("/Game/_TeamFolder/Player/SM_Light"), NULL, LOAD_None, NULL);
    FlashlightMesh->SetStaticMesh(LightMesh);
    // �E��ɃA�^�b�`����
    FlashlightMesh->SetupAttachment(Flashlight);
    // ���b�V���̓����蔻����Ȃ���
    FlashlightMesh->SetCollisionProfileName(TEXT("NoCollision"));
    // �ʒu�E�T�C�Y�E�����̒���������
    FlashlightMesh->SetRelativeLocation(FlashlightMeshLocation);
    FlashlightMesh->SetRelativeRotation(FlashlightMeshRotation);  // �� FRotator �� (Y, Z, X) �̏�
    FlashlightMesh->SetRelativeScale3D(FlashlightMeshScale);

    //�{�b�N�X�R���W���������
    PlayerCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("PlayerCollision"));
    //���C�g�ɃA�^�b�`���Ă݂�
    PlayerCollision->SetupAttachment(VRRoot);
    PlayerCollision->SetBoxExtent(FVector(-50.0f, 0.0f, 50.0f));
    PlayerCollision->SetCollisionProfileName("PlayerTrigger");

    // �X�^�e�B�b�N���b�V���R���|�[�l���g(���C�g�R���W����)�����
    LightCollision = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightCollision"));
    // �����蔻��p�̃��b�V����ǂݍ���� LightCollision �ɐݒ肷��
    UStaticMesh* ConeMesh = LoadObject<UStaticMesh>(NULL, TEXT("/Game/_TeamFolder/Player/SM_Cone"), NULL, LOAD_None, NULL);
    LightCollision->SetStaticMesh(ConeMesh);
    // ���C�g�ɃA�^�b�`����
    LightCollision->SetupAttachment(Flashlight);
    // ���b�V���������Ȃ��悤�ɂ�����
    LightCollision->SetVisibility(false);
    //�R���W�����̃v���Z�b�g��ݒ�
    LightCollision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
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

}

// Called when the game starts or when spawned
void AVRPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    //�X�R�A�̃C���X�^���X���擾����
    ScoreInstance = Cast<UPlayerScoreInstance>(GetGameInstance());
    //�X�R�A�̏�����
    ScoreInstance->ResetPlayerScore();
    //�_���[�W�J�E���g�̏�����
    ScoreInstance->ResetPlayerDamageCount();


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
    Subsystem->AddMappingContext(IMC_Flashlight, 0);

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

    TmpEnemies = OverlappingEnemies;
    for (AActor* Enemy : TmpEnemies)
    {
        if (Enemy && Enemy->GetClass()->ImplementsInterface(UDamageInterface::StaticClass()))
        {
            IDamageInterface* DamageInterface = Cast<IDamageInterface>(Enemy);
            if (DamageInterface)
            {
                DamageInterface->RecieveEnemyDamage(LightAttack, Flashlight_Color);
            }
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
            //GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Yellow, TEXT("Battery is fill! You can't use Flashlight!"));
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
            LightCollision->SetCollisionProfileName("NoCollision");
            //�[�d�؂꒼��̓��C�g�������Ȃ�
            bCanToggleLight = false;
            //GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Yellow, TEXT("Battery is empty! You can't use Flashlight!"));
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
        if (!EnhancedInputComponent)
        {
            UE_LOG(LogTemp, Warning, TEXT("EnhancedInputComponent is null"));
        }
        else
        {
            EnhancedInputComponent->BindAction(IA_Flashlight_OnOff, ETriggerEvent::Triggered, this, &AVRPlayerCharacter::ToggleFlashlight);
            EnhancedInputComponent->BindAction(IA_Flashlight_ChangeColor, ETriggerEvent::Triggered, this, &AVRPlayerCharacter::ChangeColorFlashlight);
            //GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, TEXT("Binding InputAction"));

            //�e�X�g�p
            //EnhancedInputComponent->BindAction(IA_DebugTest, ETriggerEvent::Triggered, this, &AVRPlayerCharacter::StartHaptic_EnemyDamage);
            //EnhancedInputComponent->BindAction(IA_DebugTest1, ETriggerEvent::Triggered, this, &AVRPlayerCharacter::StartHaptic_PlayerDamage);
            EnhancedInputComponent->BindAction(IA_DebugTest, ETriggerEvent::Triggered, this, &AVRPlayerCharacter::UseItem_Attack);
            EnhancedInputComponent->BindAction(IA_DebugTest1, ETriggerEvent::Triggered, this, &AVRPlayerCharacter::UseItem_Buff);
            EnhancedInputComponent->BindAction(IA_DebugTest2, ETriggerEvent::Triggered, this, &AVRPlayerCharacter::UseItem_Score);
        }
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
        //GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, TEXT("Light ON/OFF"));
        Flashlight->ToggleVisibility();
        //���C�g�̓d���ύX��AON�ɂȂ����Ȃ�
        if (Flashlight->GetVisibleFlag())
        {
            //���C�g�̓����蔻���L����
            LightCollision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
        }
        else
        {
            //���C�g�̓����蔻��𖳌���
            LightCollision->SetCollisionProfileName("NoCollision");
        }
    }
    //else if (bCanToggleLight == false)
    //{
    //    GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Yellow, TEXT("Battery is Charging! Wait until the battery is full."));
    //}

    //���C�g�̕ύX��AOFF�̏�ԂȂ�R�[���R���W�����̃v���Z�b�g�� NoCollision �ɂ���
    if (Flashlight->GetVisibleFlag() == false)
    {
        LightCollision->SetCollisionProfileName(TEXT("NoCollision"));
    }
    //ON�̏�ԂȂ�R�[���R���W�����̃v���Z�b�g�� BlockAllDynamic �ɂ���
    else
    {
        LightCollision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    }

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

        //GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, TEXT("Light ChangeColor"));
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
    FString DebugValue = "";
    for (int32 v : value)
    {
        DebugValue += FString::FromInt(v) + ", ";
    }
    GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Silver, DebugValue);

    //�g���Ȃ���Ԃ̂Ƃ��͑����^�[��
    if (bCanUseItem == false || ItemCount <= 0)
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
        
        //�f�o�b�O
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, TEXT("Used Item ( Enemy Damage )"));
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

        //�f�o�b�O
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, TEXT("Used Item ( Light Enhanced )"));

    }
    //���̌`�i�e�w[0]�E�l�����w[1]�j
    else if (value[0] > FingerBendingBorder &&
             value[1] > FingerBendingBorder &&
             value[2] <= FingerBendingBorder &&
             value[3] <= FingerBendingBorder &&
             value[4] <= FingerBendingBorder)
    {
        //�X�R�A�A�C�e���̏���
        UseItem_Score();

        //�f�o�b�O
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, TEXT("Used Item ( Add Score )"));

    }
    //����ȊO�͉����������^�[��
    else
    {
        return;
    }

    //�A�C�e�����g���Ȃ���Ԃɂ���
    bCanUseItem = false;
    //�A�C�e���̃N�[���^�C���̐ݒ�
    GetWorld()->GetTimerManager().SetTimer(ItemCoolTimeHandle, this, &AVRPlayerCharacter::ItemCoolTimeFunction, 5.0f, false);
    //�f�o�C�X�ɐU���v���𑗂�
    GloveDeviceVibration_UseItem();
    //UI�̍X�V
    ItemCount--;
    UpdateItemUI();

}
//�A�C�e���̃N�[���^�C�����\�b�h
void AVRPlayerCharacter::ItemCoolTimeFunction()
{
    bCanUseItem = true;
    // �^�C�}�[���N���A
    GetWorld()->GetTimerManager().ClearTimer(ItemCoolTimeHandle);

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, TEXT("You Can Use Item"));

}


//�����蔻��̃��\�b�h
void AVRPlayerCharacter::OnConeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, FString::Printf(TEXT("Light BeginOverlap Called (%s)"), *OtherActor->GetActorNameOrLabel()));

    // �ڐG�����A�N�^�[���I�o�P���ǂ������肷��
    if (const AEnemys* Enemy = Cast<AEnemys>(OtherActor))
    {
        OverlappingEnemies.Add(OtherActor);
        //GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, TEXT("Enemy is Overlapping"));
    }
    if (const ATitleEnemy* TitleEnemy = Cast<ATitleEnemy>(OtherActor))
    {
        OverlappingEnemies.Add(OtherActor);
    }
    //�U���̊J�n
    if (OverlappingEnemies.Num() > 0)
    {
        StartHaptic_EnemyDamage();
    }
}
void AVRPlayerCharacter::OnConeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    //GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, TEXT("Light EndOverlap Called"));

    // �I�o�P���R���W�������甲�������ǂ������肷��
    if (const AEnemys* Enemy = Cast<AEnemys>(OtherActor))
    {
        OverlappingEnemies.Remove(OtherActor);
        //GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, TEXT("Enemy is not Overlapping"));
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
        GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Cyan, TEXT("Player damage !"));
        //�f�o�C�X��U��������
        StartHaptic_PlayerDamage();
        GloveDeviceVibration_Damage();
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Cyan, TEXT("Player takes no damage !"));
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
        PlayerController->PlayHapticEffect(HapticEffect_EnemyDamage, EControllerHand::Right, 1.0f, false);

        GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, TEXT("Device Vibration (Enemy)"));
    }
}
void AVRPlayerCharacter::StartHaptic_PlayerDamage()
{
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        PlayerController->PlayHapticEffect(HapticEffect_PlayerDamage, EControllerHand::Right, 1.0f, false);

        GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, TEXT("Device Vibration (Player)"));
    }
}
// �U�����~���郁�\�b�h
void AVRPlayerCharacter::StopHapticEffect()
{
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        PlayerController->StopHapticEffect(EControllerHand::Right);
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
        ItemUI->SetText(FText::AsNumber(ItemCount));
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
        int32 Score = ScoreInstance->GetPlayerScore();
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
    ItemCount++;
    UpdateItemUI();
}
//�X�R�A�𑝂₷���\�b�h
void AVRPlayerCharacter::AddScore(int32 Value)
{
    ScoreInstance->AddPlayerScore(Value);
    UpdateScoreUI();
}

//�A�C�e���g�p���\�b�h
void AVRPlayerCharacter::UseItem_Attack()
{
    //�ς̃��f���̏o��

    //��ɂ��邷�ׂĂ̓G�Ƀ_���[�W��^����
    TArray<AActor*> Enemies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemys::StaticClass(), Enemies);
    for (AActor* Enemy : Enemies)
    {
        if (Enemy && Enemy->GetClass()->ImplementsInterface(UDamageInterface::StaticClass()))
        {
            IDamageInterface* DamageInterface = Cast<IDamageInterface>(Enemy);
            if (DamageInterface)
            {
                FString String = Enemy->GetName() + TEXT(" is Damage");
                GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, String);
                DamageInterface->RecieveItemDamage(50);
            }
        }
    }

    //�^�C�g����ʂł̏���
    TArray<AActor*> TitleEnemies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATitleEnemy::StaticClass(), TitleEnemies);
    for (AActor* Enemy : TitleEnemies)
    {
        if (Enemy && Enemy->GetClass()->ImplementsInterface(UDamageInterface::StaticClass()))
        {
            IDamageInterface* DamageInterface = Cast<IDamageInterface>(Enemy);
            if (DamageInterface)
            {
                FString String = Enemy->GetName() + TEXT(" is Damage");
                GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, String);
                DamageInterface->RecieveItemDamage(50);
            }
        }
    }
    ATitleEventManager* EventManager = Cast<ATitleEventManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ATitleEventManager::StaticClass()));
    if (EventManager)
    {
        EventManager->IsUseAttackItem();
    }
}
void AVRPlayerCharacter::UseItem_Buff()
{
    //���C�g�̃o�b�e���[���Ԃ𑝉�
    BatteryTime += AddBatteryTime;
    //���C�g�̍U���͂𑝉�
    LightAttack += AddLightAttack;
    //�ő�l�̍Đݒ�
    MaxBattery = 60 * BatteryTime;

    //�^�C�g����ʂł̏���
    ATitleEventManager* EventManager = Cast<ATitleEventManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ATitleEventManager::StaticClass()));
    if (EventManager)
    {
        EventManager->IsUseBuffItem();
    }
}
void AVRPlayerCharacter::UseItem_Score()
{
    //�X�R�A�̑���
    ScoreInstance->AddPlayerScore(1000);
    //UI�̍X�V
    UpdateScoreUI();

    //�^�C�g����ʂł̏���
    ATitleEventManager* EventManager = Cast<ATitleEventManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ATitleEventManager::StaticClass()));
    if (EventManager)
    {
        EventManager->IsUseScoreItem();
    }

}
