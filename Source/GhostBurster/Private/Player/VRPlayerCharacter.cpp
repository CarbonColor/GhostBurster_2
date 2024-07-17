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

    // �R���W�����֌W�Ȃ���ɐ�������悤�ɂ���
    this->SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // VRRoot�R���|�[�l���g�����
    VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
    // Root�R���|�[�l���g�ɐݒ�
    RootComponent = VRRoot;

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
    ////�R���W�����̃v���Z�b�g��ݒ�
    //LightCollision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    // �ʒu�E�T�C�Y�E�����̒���������
    LightCollision->SetRelativeLocation(FVector(700.0f, 0.0f, 0.0f));
    LightCollision->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));  // �� FRotator �� (Y, Z, X) �̏�
    LightCollision->SetRelativeScale3D(FVector(12.5f, 12.5f, 12.5f));
    // �����蔻��̃��\�b�h���o�C���h
    LightCollision->OnComponentBeginOverlap.AddDynamic(this, &AVRPlayerCharacter::OnConeBeginOverlap);
    LightCollision->OnComponentEndOverlap.AddDynamic(this, &AVRPlayerCharacter::OnConeEndOverlap);

    // ���C�g�̐F��ݒ肷��
    Flashlight_Color = EFlashlight_Color::White;
    // �o�b�e���[�̏����l
    Battery = MaxBattery;
    //���C�g��ON/OFF�؂�ւ����\�̏�Ԃɂ���
    CanToggleLight = true;
    // �U���͂�ݒ肷��
    Attack = 1;
    // �_���[�W�J�E���g������������
    DamageCount = 0;
    // ���G���Ԃ̏�����
    DamageNow = false;

    // Tick���~�߂�
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;
    //// Tick���n�߂�
    //PrimaryActorTick.bCanEverTick = true;
    //PrimaryActorTick.bStartWithTickEnabled = true;

    ////Haptic�t�B�[�h�o�b�N�̃G�t�F�N�g��������
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

    //SplinePathActor���擾���Đݒ肷��
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

        // �}�b�s���O�R���e�L�X�g�̃Z�b�g
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

    // �_���[�W��^���鏈��
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

    //�U���̒�~
    if (OverlappingEnemies.Num() == 0)
    {
        StopHapticFeedback();
    }

    // �o�b�e���[����
    if (Flashlight->GetVisibleFlag())    //���C�gON
    {
        Battery--;
        //�o�b�e���[���؂ꂽ��
        if (Battery <= 0)
        {
            //���C�g��؂�ւ���(OFF��)
            Flashlight->SetVisibility(false);
            //�[�d�؂꒼��̓��C�g�������Ȃ�
            CanToggleLight = false;
            GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Silver, TEXT("Battery is empty! You can't use Flashlight!"));
        }
    }
    else if (Flashlight->GetVisibleFlag() == false && Battery < MaxBattery) //���C�gOFF
    {
        Battery++;
        //���C�g�������Ȃ��Ƃ��A�o�b�e���[�����^���ɂȂ�����
        if (CanToggleLight == false && Battery >= MaxBattery)
        {
            //���C�g��������悤�ɂȂ�
            CanToggleLight = true;
            GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Silver, TEXT("Battery is fill! You can't use Flashlight!"));
        }
    }
    //if (PreBattery != Battery)
    //{
    //    GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Cyan, UKismetStringLibrary::Conv_IntToString(Battery));
    //}
    PreBattery = Battery;

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
            GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, TEXT("Binding InputAction"));

            //�e�X�g�p
            EnhancedInputComponent->BindAction(IA_DebugTest, ETriggerEvent::Triggered, this, &AVRPlayerCharacter::StartHapticFeedback);
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

    if (bIsPressed && CanToggleLight)
    {
        GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, TEXT("Light ON/OFF"));
        Flashlight->ToggleVisibility();
    }
    else if (CanToggleLight == false)
    {
        GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Silver, TEXT("Battery is Charging! Wait until the battery is full."));
    }

    ////���C�g�̕ύX��AOFF�̏�ԂȂ�R�[���R���W�����̃v���Z�b�g�� NoCollision �ɂ���
    //if (Flashlight->GetVisibleFlag() == false)
    //{
    //    LightCollision->SetCollisionProfileName(TEXT("NoCollision"));
    //}
    ////ON�̏�ԂȂ�R�[���R���W�����̃v���Z�b�g�� BlockAllDynamic �ɂ���
    //else
    //{
    //    LightCollision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
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

        GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, TEXT("Light ChangeColor"));
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

//�����蔻��̃��\�b�h
void AVRPlayerCharacter::OnConeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    //GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, TEXT("Light BeginOverlap Called"));

    // �ڐG�����A�N�^�[���I�o�P���ǂ������肷��
    if (const AEnemys* Enemy = Cast<AEnemys>(OtherActor))
    {
        //�U���̊J�n
        if (OverlappingEnemies.Num() == 0)
        {
            StartHapticFeedback();
        }
        OverlappingEnemies.Add(OtherActor);
        //GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, TEXT("Enemy is Overlapping"));
    }

    // �ڐG�����A�N�^�[���󔠂��ǂ������肷��



}
void AVRPlayerCharacter::OnConeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    //GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, TEXT("Light EndOverlap Called"));

    // �I�o�P���R���W�������甲�������ǂ������肷��
    if (const AEnemys* enemy = Cast<AEnemys>(OtherActor))
    {
        OverlappingEnemies.Remove(OtherActor);
        //GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, TEXT("Enemy is not Overlapping"));
        
        //�U���̒�~
        if (OverlappingEnemies.Num() == 0)
        {
            StopHapticFeedback();
        }
    }
}

//�I�o�P����̍U�����󂯂����̃��\�b�h
void AVRPlayerCharacter::RecievePlayerDamage()
{
    DamageCount++;
     if (DamageNow == false)
     {
         // �_���[�W�񐔂𑝂₷
         DamageCount++;
         // ���G��Ԃɂ���
         DamageNow = true;
         //���G���Ԃ̐ݒ�

     }
}

//���G���Ԃ̃��\�b�h


//�U�����J�n����֐�
void AVRPlayerCharacter::StartHapticFeedback()
{
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        PlayerController->PlayHapticEffect(HapticEffect, EControllerHand::Right, 1.0f, true);

        GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, TEXT("Device Vibration"));
    }
}
// �U�����~����֐�
void AVRPlayerCharacter::StopHapticFeedback()
{
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        PlayerController->StopHapticEffect(EControllerHand::Right);
    }
}


