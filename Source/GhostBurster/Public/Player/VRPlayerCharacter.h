// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Flashlight_Enumeration.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
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
		TObjectPtr<UInputMappingContext> IMC_Flashlight;

	// Motion Controller
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		TObjectPtr<UMotionControllerComponent> MotionController_Right;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		TObjectPtr<UMotionControllerComponent> MotionController_Left;

	//左手のメッシュ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
		TObjectPtr<USkeletalMeshComponent> HandMesh_Left;

	//プレイヤーのコリジョン(キューブ型)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collision")
		TObjectPtr<UBoxComponent> PlayerCollision;

	//ライトのコリジョン(コーン型)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collision")
		TObjectPtr<UStaticMeshComponent> LightCollision;

	//ライトのコリジョンとの当たり判定
	UFUNCTION()
		void OnConeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnConeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//オバケからの攻撃(インターフェース)
	virtual void RecievePlayerDamage() override;


	//Haptic Feedback Effect(コントローラーの振動)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Haptics")
		TObjectPtr<UHapticFeedbackEffect_Base> HapticEffect;

	//振動を開始するメソッド
	UFUNCTION()
		void StartHapticFeedback();
	UFUNCTION()
		void StopHapticFeedback();

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

	//スポットライトコンポーネント
	UPROPERTY(VisibleAnywhere)
		TObjectPtr<USpotLightComponent> Flashlight;

	//プレイヤーUI
	UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<UUserWidget> PlayerStatusWidgetClass;
	UPROPERTY()
		TObjectPtr<UUserWidget> PlayerStatusWidget;

	//ライトの色を設定するメソッド
	UFUNCTION()
	void SettingFlashlightColor();

	//スプライン経路を管理するアクター
	TObjectPtr<APlayerSplinePath> SplinePathActor;

	//ライト内(コーンのコリジョン)に入っているオバケを格納するリスト
	TSet<AActor*> OverlappingEnemies;

	//ライトの色を保持する変数
	EFlashlight_Color Flashlight_Color;

	// UI関連
	//ライトバッテリーの最大値
	const int MaxBattery = 60 * 10;
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

	//出現する敵を判別するステージ番号
	UPROPERTY(BlueprintReadOnly, Category = "Stage", meta = (AllowPrivateAccess = "true"))
		int StageNumber;
};
