// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "TimerManager.h"
#include "Flashlight_Enumeration.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Camera/CameraComponent.h"
#include "Blueprint/UserWidget.h"
#include "Interface/DamageInterface.h"
#include "VRPlayerCharacter.generated.h"

class USpotLightComponent;
class UInputAction;
class UInputMappingContext;
class UMotionControllerComponent;
class UHapticFeedbackEffect_Base;
class APlayerSplinePath;

UCLASS()
class GHOSTBURSTER_API AVRPlayerCharacter : public APawn, public IDamageInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AVRPlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//ライトのON/OFFを切り替えるメソッド
	UFUNCTION()
		void ToggleFlashlight(const FInputActionValue& value);

	//ライトの色を切り替えるメソッド
	UFUNCTION()
		void ChangeColorFlashlight(const FInputActionValue& value);

	//バッテリーのUIを更新するメソッド
	UFUNCTION()
		void UpdateBatteryUI();

	//アイテムの所有数のUIを更新するメソッド
	UFUNCTION()
		void UpdateItemUI();

	//スコアのUIを更新するメソッド
	UFUNCTION()
		void UpdateScoreUI();


	////ハンドトラッキングのセットアップ
	//UFUNCTION()
	//	void SetupHandTracking();

	////ハンドトラッキングのアップデート
	//UFUNCTION()
	//	void UpdateHandPose();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		TObjectPtr<UInputAction> IA_Flashlight_OnOff;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		TObjectPtr<UInputAction> IA_Flashlight_ChangeColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		TObjectPtr<UInputAction> IA_DebugTest;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		TObjectPtr<UInputAction> IA_DebugTest1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		TObjectPtr<UInputMappingContext> IMC_Flashlight;

	// Motion Controller
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		TObjectPtr<UMotionControllerComponent> MotionController_Right;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		TObjectPtr<UMotionControllerComponent> MotionController_Left;

	//左手のメッシュ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
		TObjectPtr<USkeletalMeshComponent> HandMesh_Left;

	//右手のメッシュ(懐中電灯のStaticMesh)
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Mesh")
		TObjectPtr<UStaticMeshComponent> FlashlightMesh;

	//プレイヤーのコリジョン(キューブ型)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collision")
		TObjectPtr<UBoxComponent> PlayerCollision;

	//ライトのコリジョン(コーン型)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collision")
		TObjectPtr<UStaticMeshComponent> LightCollision;

	//カメラのコンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		TObjectPtr<UCameraComponent> CameraComponent;

	//プレイヤーUI
	UPROPERTY(EditAnywhere, Category = "UI")
		TObjectPtr<UWidgetComponent> PlayerStatusWidgetComponent;

	// パラメーター系統
	UPROPERTY(EditAnywhere, Category = "Mesh Parameter")
		FVector FlashlightMeshLocation;

	UPROPERTY(EditAnywhere, Category = "Mesh Parameter")
		FRotator FlashlightMeshRotation;

	UPROPERTY(EditAnywhere, Category = "Mesh Parameter")
		FVector FlashlightMeshScale;

	//ライトのコリジョンとの当たり判定
	UFUNCTION()
		void OnConeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnConeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//オバケからの攻撃(インターフェース)
	virtual void RecievePlayerDamage() override;


	//Haptic Feedback Effect(コントローラーの振動)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Haptics")
		TObjectPtr<UHapticFeedbackEffect_Base> HapticEffect_EnemyDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Haptics")
		TObjectPtr<UHapticFeedbackEffect_Base> HapticEffect_PlayerDamage;

	//振動を開始するメソッド
	UFUNCTION()
		void StartHaptic_EnemyDamage();
	UFUNCTION()
		void StartHaptic_PlayerDamage();
	//振動を停止するメソッド
	UFUNCTION()
		void StopHapticEffect();

	//ステージ番号を増やす
	UFUNCTION(BlueprintCallable, Category = "Stage")
		void NextStage();

	//ステージ番号を取得する
	UFUNCTION(BlueprintCallable, Category = "Stage")
		int GetStageNumber();


private:
	//シーンコンポーネント
	UPROPERTY(VisibleAnywhere)
		TObjectPtr<USceneComponent> VRRoot;

	////スプリングアームコンポーネント
	//UPROPERTY(VisibleAnywhere)
	//	TObjectPtr<USpringArmComponent> SpringArmComponent;

	//スポットライトコンポーネント
	UPROPERTY(VisibleAnywhere)
		TObjectPtr<USpotLightComponent> Flashlight;

	UPROPERTY()
		TObjectPtr<UUserWidget> PlayerStatusWidget;

	//ライトの色を設定するメソッド
	UFUNCTION()
	void SettingFlashlightColor();

	//無敵時間の処理
	UFUNCTION()
	void NoDamageFunction();

	//スプライン経路を管理するアクター
	TObjectPtr<APlayerSplinePath> SplinePathActor;

	//ライト内(コーンのコリジョン)に入っているオバケを格納するリスト
	TSet<AActor*> OverlappingEnemies;

	//ライトの色を保持する変数
	EFlashlight_Color Flashlight_Color;

	// UI関連
	//ライトバッテリーの最大値
	const int MaxBattery = 60 * 30;
	//ライトバッテリー
	int Battery;
	UPROPERTY()
	TObjectPtr<UProgressBar> BatteryUI;
	//プレイヤーのスコア
	int Score;
	UPROPERTY()
	TObjectPtr<UTextBlock> ScoreUI;
	//アイテムの所有数
	int Item;
	UPROPERTY()
	TObjectPtr<UTextBlock> ItemUI;

	//ライトの操作を受け付けているかどうか
	bool CanToggleLight;
	
	//オバケに与える攻撃力
	int Attack;
	//オバケから受けた攻撃
	int DamageCount;

	//無敵状態
	bool DamageNow;
	//無敵時間用タイマーハンドル
	FTimerHandle NoDamageTimerHandle;

	//出現する敵を判別するステージ番号
	UPROPERTY(BlueprintReadOnly, Category = "Stage", meta = (AllowPrivateAccess = "true"))
		int StageNumber;


	//デバッグ用タイマー
	int DebugTimer;
};
