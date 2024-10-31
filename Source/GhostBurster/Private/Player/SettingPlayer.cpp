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

	// VRRootコンポーネントを作る
	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	// Rootコンポーネントに設定
	RootComponent = VRRoot;

	//カメラコンポーネントの作成
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(VRRoot);

	//ウィジェットコンポーネントの作成（キャリブレーションUI）
	CalibrationWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("CalibrationWidgetComponent"));
	CalibrationWidgetComponent->SetupAttachment(CameraComponent);
	//ウィジェットの設定
	UClass* WidgetClass = LoadObject<UClass>(nullptr, TEXT("/Game/_TeamFolder/UI/UI_DeviceSetting_Finger.UI_DeviceSetting_Finger_C"));
	CalibrationWidgetComponent->SetWidgetClass(WidgetClass);
	CalibrationWidgetComponent->SetWidgetSpace(EWidgetSpace::World);

	// モーションコントローラーコンポーネント(右手)を作る
	RightController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightController"));
	// VRRootコンポーネントにアタッチする
	RightController->SetupAttachment(VRRoot);
	RightController->SetTrackingSource(EControllerHand::Right);
	

	// 右手のライトのスタティックメッシュコンポーネントを作る
	FlashlightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlashlightMesh"));
	// 懐中電灯のメッシュを読み込んでセットする
	UStaticMesh* LightMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/_TeamFolder/Player/SM_Light"));
	FlashlightMesh->SetStaticMesh(LightMesh);
	// 右手にアタッチする
	FlashlightMesh->SetupAttachment(RightController);
	// メッシュの当たり判定をなくす
	FlashlightMesh->SetCollisionProfileName(TEXT("NoCollision"));
	// メッシュの影をなくす
	FlashlightMesh->SetCastShadow(false);

	//ライトで使うナイアガラコンポーネントを作る
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(FlashlightMesh);

}

// Called when the game starts or when spawned
void ASettingPlayer::BeginPlay()
{
	Super::BeginPlay();

	//スコアのインスタンスを取得する
	ScoreInstance = Cast<UPlayerScoreInstance>(GetGameInstance());
	ScoreInstance->AllDataResetFunction();

	//イベント進行度の初期化
	EventNumber = 0;

	//キャリブレーションの初期化
	NormalFinger = { 0, 0, 0, 0, 0 };
	FoxFinger = { 0, 0, 0, 0, 0 };
	GunFinger = { 0, 0, 0, 0, 0 };

	//ボタン制御
	bCanPushButton = true;

	//測定中ならtrueにする
	bIsMeasuring = false;

	bCanUseItem = true;

	// Enhanced Input setup
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	// 右手のInputMappingContextを追加
	Subsystem->AddMappingContext(IMC_RightController, 0);

	//Widgetの取得
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

	//初期化
	EventText->SetText(FText::FromString("Device Setting"));
	UpdateText(NormalText, NormalFinger);
	UpdateText(FoxText, FoxFinger);
	UpdateText(GunText, GunFinger);
	UpdateText(BorderText, ScoreInstance->FingerBorder);

	//隠す
	NormalText->SetVisibility(ESlateVisibility::Hidden);
	StaticNormalText->SetVisibility(ESlateVisibility::Hidden);
	FoxText->SetVisibility(ESlateVisibility::Hidden);
	StaticFoxText->SetVisibility(ESlateVisibility::Hidden);
	GunText->SetVisibility(ESlateVisibility::Hidden);
	StaticGunText->SetVisibility(ESlateVisibility::Hidden);
	BorderText->SetVisibility(ESlateVisibility::Hidden);
	StaticBorderText->SetVisibility(ESlateVisibility::Hidden);

	//ナイアガラのセット
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
		// アクションをバインド
		EnhancedInputComponent->BindAction(IA_NextEvent, ETriggerEvent::Started, this, &ASettingPlayer::PushButton);
		EnhancedInputComponent->BindAction(IA_Retry, ETriggerEvent::Started, this, &ASettingPlayer::Reset);
	}

}

//イベント関数
void ASettingPlayer::NormalFingerSetting()
{
	//テキストの表示
	StaticNormalText->SetVisibility(ESlateVisibility::Visible);
	NormalText->SetVisibility(ESlateVisibility::Visible);
	EventText->SetText(FText::FromString("1_Default Hand"));
	//2秒後にボタンを押せるようにする
	GetWorld()->GetTimerManager().SetTimer(EventTimerHandle, this, &ASettingPlayer::CanPushButton, 2.0f, false);
}
void ASettingPlayer::FoxFingerSetting()
{
	//テキストの表示
	StaticFoxText->SetVisibility(ESlateVisibility::Visible);
	FoxText->SetVisibility(ESlateVisibility::Visible);
	EventText->SetText(FText::FromString("2_Fox Hand"));
	//2秒後にボタンを押せるようにする
	GetWorld()->GetTimerManager().SetTimer(EventTimerHandle, this, &ASettingPlayer::CanPushButton, 2.0f, false);
}
void ASettingPlayer::GunFingerSetting()
{
	//テキストの表示
	StaticGunText->SetVisibility(ESlateVisibility::Visible);
	GunText->SetVisibility(ESlateVisibility::Visible);
	EventText->SetText(FText::FromString("3_Gun Hand"));
	//2秒後にボタンを押せるようにする
	GetWorld()->GetTimerManager().SetTimer(EventTimerHandle, this, &ASettingPlayer::CanPushButton, 2.0f, false);
}
void ASettingPlayer::CheckBorder()
{
	//ボーダーのセッティング
	ScoreInstance->SetFingerBorder(NormalFinger, FoxFinger, GunFinger);
	//テキストのセッティング
	UpdateText(BorderText, ScoreInstance->FingerBorder);
	//テキストの表示
	SencerText->SetVisibility(ESlateVisibility::Hidden);
	StaticBorderText->SetVisibility(ESlateVisibility::Visible);
	BorderText->SetVisibility(ESlateVisibility::Visible);
	EventText->SetText(FText::FromString("Setting Success !"));
	//2秒後にボタンを押せるようにする
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
	//5秒後に測定を止める
	GetWorld()->GetTimerManager().SetTimer(EventTimerHandle, this, &ASettingPlayer::StopMeasuring, 5.0f, false);
}
//測定を止めるメソッド
void ASettingPlayer::StopMeasuring()
{
	bIsMeasuring = false;
	EventText->SetText(FText::FromString("Success!"));
	//2秒後にNextEventを呼び出す
	GetWorld()->GetTimerManager().ClearTimer(EventTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(EventTimerHandle, this, &ASettingPlayer::NextEvent, 2.0f, false);
}

//ボタンを押せるようにするメソッド
void ASettingPlayer::CanPushButton()
{
	bCanPushButton = true;
}

//値のセッティング
void ASettingPlayer::CheckNormalFinger(const TArray<int32> Array)
{
	//親指：伸びる指の最大値を設定
	if (NormalFinger[0] < Array[0] && Array[0] != 0)
	{
		NormalFinger[0] = Array[0];
	}
	//人差し指：伸びる指の最大値を設定
	if (NormalFinger[1] < Array[1] && Array[1] != 0)
	{
		NormalFinger[1] = Array[1];
	}
	//中指：伸びる指の最大値を設定
	if (NormalFinger[2] < Array[2] && Array[2] != 0)
	{
		NormalFinger[2] = Array[2];
	}
	//薬指：伸びる指の最大値を設定
	if (NormalFinger[3] < Array[3] && Array[3] != 0)
	{
		NormalFinger[3] = Array[3];
	}
	//小指：伸びる指の最大値を設定
	if (NormalFinger[4] < Array[4] && Array[4] != 0)
	{
		NormalFinger[4] = Array[4];
	}

	//テキストの変更
	UpdateText(NormalText, NormalFinger);

}
void ASettingPlayer::CheckFoxFinger(const TArray<int32> Array)
{
	//親指：曲げる指の最小値を設定
	if (FoxFinger[0] < Array[0] && Array[0] != 0)
	{
		FoxFinger[0] = Array[0];
	}
	//人差し指：伸びる指の最大値を設定
	if (FoxFinger[1] < Array[1] && Array[1] != 0)
	{
		FoxFinger[1] = Array[1];
	}
	//中指：曲げる指の最小値を設定
	if (FoxFinger[2] < Array[2] && Array[2] != 0)
	{
		FoxFinger[2] = Array[2];
	}
	//薬指：曲げる指の最小値を設定
	if (FoxFinger[3] < Array[3] && Array[3] != 0)
	{
		FoxFinger[3] = Array[3];
	}
	//小指：伸びる指の最大値を設定
	if (FoxFinger[4] < Array[4] && Array[4] != 0)
	{
		FoxFinger[4] = Array[4];
	}

	//テキストの変更
	UpdateText(FoxText, FoxFinger);

}
void ASettingPlayer::CheckGunFinger(const TArray<int32> Array)
{
	//親指：伸びる指の最大値を設定
	if (GunFinger[0] < Array[0] && Array[0] != 0)
	{
		GunFinger[0] = Array[0];
	}
	//人差し指：伸びる指の最大値を設定
	if (GunFinger[1] < Array[1] && Array[1] != 0)
	{
		GunFinger[1] = Array[1];
	}
	//中指：曲げる指の最小値を設定
	if (GunFinger[2] < Array[2] && Array[2] != 0)
	{
		GunFinger[2] = Array[2];
	}
	//薬指：曲げる指の最小値を設定
	if (GunFinger[3] < Array[3] && Array[3] != 0)
	{
		GunFinger[3] = Array[3];
	}
	//小指：曲げる指の最小値を設定
	if (GunFinger[4] < Array[4] && Array[4] != 0)
	{
		GunFinger[4] = Array[4];
	}

	//テキストの変更
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

	//狐の形（親指[0]・中指[2]・薬指[3]）
	if (value[0] > ScoreInstance->FingerBorder[0] &&
		value[1] <= ScoreInstance->FingerBorder[1] &&
		value[2] > ScoreInstance->FingerBorder[2] &&
		value[3] > ScoreInstance->FingerBorder[3] &&
		value[4] <= ScoreInstance->FingerBorder[4])
	{
		//攻撃アイテムの処理
		UseItem_Attack();
	}
	//銃の形（中指[2]・薬指[3]・小指[4]）
	else if (value[0] <= ScoreInstance->FingerBorder[0] &&
		value[1] <= ScoreInstance->FingerBorder[1] &&
		value[2] > ScoreInstance->FingerBorder[2] &&
		value[3] > ScoreInstance->FingerBorder[3] &&
		value[4] > ScoreInstance->FingerBorder[4])
	{
		//強化アイテムの処理
		UseItem_Buff();
	}
}
void ASettingPlayer::UseItem_Buff()
{
	//アイテムの制御
	bCanUseItem = false;

	//デバッグ
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Used Item ( Light )"));

	//ナイアガラエフェクトを一度だけ再生
	if (NiagaraComponent && BuffEffectNiagara)
	{
		NiagaraComponent->Activate(true); // 再生を開始
	}
	GetWorld()->GetTimerManager().SetTimer(ItemCoolTimeHandle, this, &ASettingPlayer::ItemCoolTimeFunction, 5.0f, false);

}

void ASettingPlayer::UseItem_Attack()
{
	//アイテムの制御
	bCanUseItem = false;

	//デバッグ
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Used Item ( Enemy Damage )"));

	//狐のモデルの出現
	if (FoxGhostModel)
	{
		FActorSpawnParameters SpawnParams;
		FVector PlayerLocation = GetActorLocation();
		FRotator PlayerRotation = GetActorRotation();

		// プレイヤーの前方に100ユニット、左方向に100ユニットずらした位置を計算
		FVector ForwardVector = PlayerRotation.Vector();  // プレイヤーの前方方向
		FVector RightVector = FRotationMatrix(PlayerRotation).GetUnitAxis(EAxis::Y); // プレイヤーの右方向
		//生成座標の設定
		FVector SpawnLocation = PlayerLocation + ForwardVector * 100.0f - RightVector * 100.0f;
		SpawnLocation.Z = 0;
		FRotator SpawnRotation = PlayerRotation + FRotator(0.0f, -90.0f, 0.0f);
		// Fox_BPをワールドに生成
		AActor* SpawnedFox = GetWorld()->SpawnActor<AActor>(FoxGhostModel, SpawnLocation, SpawnRotation, SpawnParams);
	}

	GetWorld()->GetTimerManager().SetTimer(ItemCoolTimeHandle, this, &ASettingPlayer::ItemCoolTimeFunction, 5.0f, false);

}
void ASettingPlayer::ItemCoolTimeFunction()
{
	bCanUseItem = true;
	// タイマーをクリア
	GetWorld()->GetTimerManager().ClearTimer(ItemCoolTimeHandle);

	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("You Can Use Item"));
}

//テキストアップデート
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

//インプット関数
void ASettingPlayer::PushButton(const FInputActionValue& Value)
{
	if (bCanPushButton == false)
	{
		return;
	}


	bCanPushButton = false;

	NextEvent();
}

//次のイベントに進むメソッド
void ASettingPlayer::NextEvent()
{
	//タイマーをクリア
	GetWorld()->GetTimerManager().ClearTimer(EventTimerHandle);

	//イベントを進める
	EventNumber++;

	//進行度に応じてイベントを進める
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
