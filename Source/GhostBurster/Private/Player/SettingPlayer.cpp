// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SettingPlayer.h"
#include "MotionControllerComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ASettingPlayer::ASettingPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// VRRoot�R���|�[�l���g�����
	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	// Root�R���|�[�l���g�ɐݒ�
	RootComponent = VRRoot;

	//�J�����R���|�[�l���g�̍쐬
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(VRRoot);

	//�E�B�W�F�b�g�R���|�[�l���g�̍쐬�i�L�����u���[�V����UI�j
	CalibrationWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("CalibrationWidgetComponent"));
	CalibrationWidgetComponent->SetupAttachment(CameraComponent);
	//�E�B�W�F�b�g�̐ݒ�
	UClass* WidgetClass = LoadObject<UClass>(nullptr, TEXT("/Game/_TeamFolder/UI/UI_DeviceSetting_Finger.UI_DeviceSetting_Finger_C"));
	CalibrationWidgetComponent->SetWidgetClass(WidgetClass);
	CalibrationWidgetComponent->SetWidgetSpace(EWidgetSpace::World);

	// ���[�V�����R���g���[���[�R���|�[�l���g(�E��)�����
	RightController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightController"));
	// VRRoot�R���|�[�l���g�ɃA�^�b�`����
	RightController->SetupAttachment(VRRoot);
	RightController->SetTrackingSource(EControllerHand::Right);
	

	// �E��̃��C�g�̃X�^�e�B�b�N���b�V���R���|�[�l���g�����
	FlashlightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlashlightMesh"));
	// �����d���̃��b�V����ǂݍ���ŃZ�b�g����
	UStaticMesh* LightMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/_TeamFolder/Player/SM_Light"));
	FlashlightMesh->SetStaticMesh(LightMesh);
	// �E��ɃA�^�b�`����
	FlashlightMesh->SetupAttachment(RightController);
	// ���b�V���̓����蔻����Ȃ���
	FlashlightMesh->SetCollisionProfileName(TEXT("NoCollision"));
	// ���b�V���̉e���Ȃ���
	FlashlightMesh->SetCastShadow(false);

	//���C�g�Ŏg���i�C�A�K���R���|�[�l���g�����
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(FlashlightMesh);

}

// Called when the game starts or when spawned
void ASettingPlayer::BeginPlay()
{
	Super::BeginPlay();

	//�X�R�A�̃C���X�^���X���擾����
	ScoreInstance = Cast<UPlayerScoreInstance>(GetGameInstance());
	ScoreInstance->AllDataResetFunction();

	//�C�x���g�i�s�x�̏�����
	EventNumber = 0;

	//�L�����u���[�V�����̏�����
	NormalFinger = { 0, 0, 0, 0, 0 };
	FoxFinger = { 0, 0, 0, 0, 0 };
	GunFinger = { 0, 0, 0, 0, 0 };

	//�{�^������
	bCanPushButton = true;

	//���蒆�Ȃ�true�ɂ���
	bIsMeasuring = false;

	bCanUseItem = true;

	// Enhanced Input setup
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	// �E���InputMappingContext��ǉ�
	Subsystem->AddMappingContext(IMC_RightController, 0);

	//Widget�̎擾
	CalibrationWidgetComponent->InitWidget();
	TextWidget = CalibrationWidgetComponent->GetUserWidgetObject();

	EventText = Cast<UTextBlock>(TextWidget->GetWidgetFromName(TEXT("EventText")));
	SencerText = Cast<UTextBlock>(TextWidget->GetWidgetFromName(TEXT("Sencer")));
	NormalText = Cast<UTextBlock>(TextWidget->GetWidgetFromName(TEXT("NormalFinger")));
	StaticNormalText = Cast<UTextBlock>(TextWidget->GetWidgetFromName(TEXT("Static_NormalFinger")));
	FoxText = Cast<UTextBlock>(TextWidget->GetWidgetFromName(TEXT("FoxFinger")));
	StaticFoxText = Cast<UTextBlock>(TextWidget->GetWidgetFromName(TEXT("Static_FoxFinger")));
	GunText = Cast<UTextBlock>(TextWidget->GetWidgetFromName(TEXT("GunFinger")));
	StaticGunText = Cast<UTextBlock>(TextWidget->GetWidgetFromName(TEXT("Static_GunFinger")));
	BorderText = Cast<UTextBlock>(TextWidget->GetWidgetFromName(TEXT("Border")));
	StaticBorderText = Cast<UTextBlock>(TextWidget->GetWidgetFromName(TEXT("Static_Border")));

	//������
	EventText->SetText(FText::FromString("Device Setting"));
	UpdateText(NormalText, NormalFinger);
	UpdateText(FoxText, FoxFinger);
	UpdateText(GunText, GunFinger);
	UpdateText(BorderText, ScoreInstance->FingerBorder);

	//�B��
	NormalText->SetVisibility(ESlateVisibility::Hidden);
	StaticNormalText->SetVisibility(ESlateVisibility::Hidden);
	FoxText->SetVisibility(ESlateVisibility::Hidden);
	StaticFoxText->SetVisibility(ESlateVisibility::Hidden);
	GunText->SetVisibility(ESlateVisibility::Hidden);
	StaticGunText->SetVisibility(ESlateVisibility::Hidden);
	BorderText->SetVisibility(ESlateVisibility::Hidden);
	StaticBorderText->SetVisibility(ESlateVisibility::Hidden);

	//�i�C�A�K���̃Z�b�g
	if (BuffEffectNiagara)
	{
		NiagaraComponent->SetAsset(BuffEffectNiagara);
	}

}

// Called every frame
void ASettingPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASettingPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// �A�N�V�������o�C���h
		EnhancedInputComponent->BindAction(IA_NextEvent, ETriggerEvent::Started, this, &ASettingPlayer::PushButton);
		EnhancedInputComponent->BindAction(IA_Retry, ETriggerEvent::Started, this, &ASettingPlayer::Reset);
	}

}

//�C�x���g�֐�
void ASettingPlayer::NormalFingerSetting()
{
	//�e�L�X�g�̕\��
	StaticNormalText->SetVisibility(ESlateVisibility::Visible);
	NormalText->SetVisibility(ESlateVisibility::Visible);
	EventText->SetText(FText::FromString("1_Default Hand"));
	//2�b��Ƀ{�^����������悤�ɂ���
	GetWorld()->GetTimerManager().SetTimer(EventTimerHandle, this, &ASettingPlayer::CanPushButton, 2.0f, false);
}
void ASettingPlayer::FoxFingerSetting()
{
	//�e�L�X�g�̕\��
	StaticFoxText->SetVisibility(ESlateVisibility::Visible);
	FoxText->SetVisibility(ESlateVisibility::Visible);
	EventText->SetText(FText::FromString("2_Fox Hand"));
	//2�b��Ƀ{�^����������悤�ɂ���
	GetWorld()->GetTimerManager().SetTimer(EventTimerHandle, this, &ASettingPlayer::CanPushButton, 2.0f, false);
}
void ASettingPlayer::GunFingerSetting()
{
	//�e�L�X�g�̕\��
	StaticGunText->SetVisibility(ESlateVisibility::Visible);
	GunText->SetVisibility(ESlateVisibility::Visible);
	EventText->SetText(FText::FromString("3_Gun Hand"));
	//2�b��Ƀ{�^����������悤�ɂ���
	GetWorld()->GetTimerManager().SetTimer(EventTimerHandle, this, &ASettingPlayer::CanPushButton, 2.0f, false);
}
void ASettingPlayer::CheckBorder()
{
	//�{�[�_�[�̃Z�b�e�B���O
	ScoreInstance->SetFingerBorder(NormalFinger, FoxFinger, GunFinger);
	//�e�L�X�g�̃Z�b�e�B���O
	UpdateText(BorderText, ScoreInstance->FingerBorder);
	//�e�L�X�g�̕\��
	SencerText->SetVisibility(ESlateVisibility::Hidden);
	StaticBorderText->SetVisibility(ESlateVisibility::Visible);
	BorderText->SetVisibility(ESlateVisibility::Visible);
	EventText->SetText(FText::FromString("Setting Success !"));
	//2�b��Ƀ{�^����������悤�ɂ���
	GetWorld()->GetTimerManager().SetTimer(EventTimerHandle, this, &ASettingPlayer::CanPushButton, 2.0f, false);
}
void ASettingPlayer::GoToTitle()
{
	UGameplayStatics::OpenLevel(GetWorld(), "Title");
}

void ASettingPlayer::Measuring()
{
	EventText->SetText(FText::FromString("Measuring......"));
	bIsMeasuring = true;
	//5�b��ɑ�����~�߂�
	GetWorld()->GetTimerManager().SetTimer(EventTimerHandle, this, &ASettingPlayer::StopMeasuring, 5.0f, false);
}
//������~�߂郁�\�b�h
void ASettingPlayer::StopMeasuring()
{
	bIsMeasuring = false;
	EventText->SetText(FText::FromString("Success!"));
	//2�b���NextEvent���Ăяo��
	GetWorld()->GetTimerManager().ClearTimer(EventTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(EventTimerHandle, this, &ASettingPlayer::NextEvent, 2.0f, false);
}

//�{�^����������悤�ɂ��郁�\�b�h
void ASettingPlayer::CanPushButton()
{
	bCanPushButton = true;
}

//�l�̃Z�b�e�B���O
void ASettingPlayer::CheckNormalFinger(const TArray<int32> Array)
{
	//�e�w�F�L�т�w�̍ő�l��ݒ�
	if (NormalFinger[0] < Array[0] && Array[0] != 0)
	{
		NormalFinger[0] = Array[0];
	}
	//�l�����w�F�L�т�w�̍ő�l��ݒ�
	if (NormalFinger[1] < Array[1] && Array[1] != 0)
	{
		NormalFinger[1] = Array[1];
	}
	//���w�F�L�т�w�̍ő�l��ݒ�
	if (NormalFinger[2] < Array[2] && Array[2] != 0)
	{
		NormalFinger[2] = Array[2];
	}
	//��w�F�L�т�w�̍ő�l��ݒ�
	if (NormalFinger[3] < Array[3] && Array[3] != 0)
	{
		NormalFinger[3] = Array[3];
	}
	//���w�F�L�т�w�̍ő�l��ݒ�
	if (NormalFinger[4] < Array[4] && Array[4] != 0)
	{
		NormalFinger[4] = Array[4];
	}

	//�e�L�X�g�̕ύX
	UpdateText(NormalText, NormalFinger);

}
void ASettingPlayer::CheckFoxFinger(const TArray<int32> Array)
{
	//�e�w�F�Ȃ���w�̍ŏ��l��ݒ�
	if (FoxFinger[0] < Array[0] && Array[0] != 0)
	{
		FoxFinger[0] = Array[0];
	}
	//�l�����w�F�L�т�w�̍ő�l��ݒ�
	if (FoxFinger[1] < Array[1] && Array[1] != 0)
	{
		FoxFinger[1] = Array[1];
	}
	//���w�F�Ȃ���w�̍ŏ��l��ݒ�
	if (FoxFinger[2] < Array[2] && Array[2] != 0)
	{
		FoxFinger[2] = Array[2];
	}
	//��w�F�Ȃ���w�̍ŏ��l��ݒ�
	if (FoxFinger[3] < Array[3] && Array[3] != 0)
	{
		FoxFinger[3] = Array[3];
	}
	//���w�F�L�т�w�̍ő�l��ݒ�
	if (FoxFinger[4] < Array[4] && Array[4] != 0)
	{
		FoxFinger[4] = Array[4];
	}

	//�e�L�X�g�̕ύX
	UpdateText(FoxText, FoxFinger);

}
void ASettingPlayer::CheckGunFinger(const TArray<int32> Array)
{
	//�e�w�F�L�т�w�̍ő�l��ݒ�
	if (GunFinger[0] < Array[0] && Array[0] != 0)
	{
		GunFinger[0] = Array[0];
	}
	//�l�����w�F�L�т�w�̍ő�l��ݒ�
	if (GunFinger[1] < Array[1] && Array[1] != 0)
	{
		GunFinger[1] = Array[1];
	}
	//���w�F�Ȃ���w�̍ŏ��l��ݒ�
	if (GunFinger[2] < Array[2] && Array[2] != 0)
	{
		GunFinger[2] = Array[2];
	}
	//��w�F�Ȃ���w�̍ŏ��l��ݒ�
	if (GunFinger[3] < Array[3] && Array[3] != 0)
	{
		GunFinger[3] = Array[3];
	}
	//���w�F�Ȃ���w�̍ŏ��l��ݒ�
	if (GunFinger[4] < Array[4] && Array[4] != 0)
	{
		GunFinger[4] = Array[4];
	}

	//�e�L�X�g�̕ύX
	UpdateText(GunText, GunFinger);

}

void ASettingPlayer::CheckUsedItem(const TArray<int32> value)
{
	//FString DebugValue = "";
	//for (int32 v : value)
	//{
	//    DebugValue += FString::FromInt(v) + ", ";
	//}
	//GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Red, DebugValue);

	if (bCanUseItem == false)
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
void ASettingPlayer::UseItem_Buff()
{
	//�A�C�e���̐���
	bCanUseItem = false;

	//�f�o�b�O
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Used Item ( Light )"));

	//�i�C�A�K���G�t�F�N�g����x�����Đ�
	if (NiagaraComponent && BuffEffectNiagara)
	{
		NiagaraComponent->Activate(true); // �Đ����J�n
	}
	GetWorld()->GetTimerManager().SetTimer(ItemCoolTimeHandle, this, &ASettingPlayer::ItemCoolTimeFunction, 5.0f, false);

}

void ASettingPlayer::UseItem_Attack()
{
	//�A�C�e���̐���
	bCanUseItem = false;

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

	GetWorld()->GetTimerManager().SetTimer(ItemCoolTimeHandle, this, &ASettingPlayer::ItemCoolTimeFunction, 5.0f, false);

}
void ASettingPlayer::ItemCoolTimeFunction()
{
	bCanUseItem = true;
	// �^�C�}�[���N���A
	GetWorld()->GetTimerManager().ClearTimer(ItemCoolTimeHandle);

	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("You Can Use Item"));
}

//�e�L�X�g�A�b�v�f�[�g
void ASettingPlayer::UpdateText(UTextBlock* TextBlock, const TArray<int32> Array)
{
	if (TextBlock == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("None TextBlock"));
		return;
	}

	FString String = "";

	for (int32 i = 0; i < Array.Num(); ++i)
	{
		String += FString::FromInt(Array[i]);

		if (i < Array.Num() - 1)
		{
			String += TEXT(", ");
		}
	}

	TextBlock->SetText(FText::FromString(String));
}

//�C���v�b�g�֐�
void ASettingPlayer::PushButton(const FInputActionValue& Value)
{
	if (bCanPushButton == false)
	{
		return;
	}


	bCanPushButton = false;

	NextEvent();
}

//���̃C�x���g�ɐi�ރ��\�b�h
void ASettingPlayer::NextEvent()
{
	//�^�C�}�[���N���A
	GetWorld()->GetTimerManager().ClearTimer(EventTimerHandle);

	//�C�x���g��i�߂�
	EventNumber++;

	//�i�s�x�ɉ����ăC�x���g��i�߂�
	switch (EventNumber)
	{
	case 1:
		NormalFingerSetting();
		break;

	case 3:
		FoxFingerSetting();
		break;

	case 5:
		GunFingerSetting();
		break;

	case 7:
		CheckBorder();
		break;

	case 8:
		GoToTitle();
		break;

	case 2:
	case 4:
	case 6:
		Measuring();
		break;

	default:
		break;
	}
}

void ASettingPlayer::Reset()
{
	if (EventNumber == 7)
	{
		UGameplayStatics::OpenLevel(GetWorld(), "Setting_Finger");
	}
}
