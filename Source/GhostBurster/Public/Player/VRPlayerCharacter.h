// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameData/PlayerScoreInstance.h"
#include "TimerManager.h"
#include "Misc/OutputDeviceNull.h"
#include "Flashlight_Enumeration.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
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

	//アイテムが使用されたかどうかを確認するメソッド
	UFUNCTION(BlueprintCallable)
		void CheckUsedItem(const TArray<int> value);


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
		TObjectPtr<UInputAction> IA_Glove_UseAttackItem;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		TObjectPtr<UInputAction> IA_Glove_UseBuffItem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		TObjectPtr<UInputMappingContext> IMC_Flashlight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		TObjectPtr<UInputMappingContext> IMC_GloveDevice;

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

	//狐の霊のメッシュ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blueprint")
		TSubclassOf<AActor> FoxGhostModel;

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


	//アイテムボーダーリスト
	UPROPERTY()
		TArray<int32> AttackItemBorder;
	UPROPERTY()
		TArray<int32> BuffItemBorder;

	//ライトのコリジョンとの当たり判定
	UFUNCTION()
		void OnConeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnConeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//アイテムの個数を増やす処理
	UFUNCTION(BlueprintCallable)
		void AddItem();
	//スコアを増やす処理
	UFUNCTION(BlueprintCallable)
		void AddScore(int32 Value);

	//バッテリーのUIを更新するメソッド
	UFUNCTION()
		void UpdateBatteryUI();
	//アイテムの所有数のUIを更新するメソッド
	UFUNCTION()
		void UpdateItemUI();
	//スコアのUIを更新するメソッド
	UFUNCTION()
		void UpdateScoreUI();
	//余ったアイテムをスコアに変換するメソッド（タイトル用）
	UFUNCTION()
		void ChangeScore();


	//オバケからの攻撃(インターフェース)
	virtual void RecievePlayerDamage() override;

	//Haptic Feedback Effect(コントローラーの振動)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Haptics")
		TObjectPtr<UHapticFeedbackEffect_Base> HapticEffect_EnemyDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Haptics")
		TObjectPtr<UHapticFeedbackEffect_Base> HapticEffect_PlayerDamage;

	//デバイスに振動要請を送るメソッド（Blueprintで実装）
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Device")
		void GloveDeviceVibration_UseItem();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Device")
		void GloveDeviceVibration_Damage();


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
		int32 GetStageNumber();

	//SE
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SE")
		TObjectPtr<USoundCue> EnemyHitSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SE")
		TObjectPtr<USoundCue> LightSwitchSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SE")
		TObjectPtr<USoundCue> PlayerDamageSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SE")
		TObjectPtr<USoundCue> UseAttackItemSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SE")
		TObjectPtr<USoundCue> UseBuffItemSound;


private:
	
	//シーンコンポーネント
	UPROPERTY(VisibleAnywhere)
		TObjectPtr<USceneComponent> VRRoot;

	//スポットライトコンポーネント
	UPROPERTY(VisibleAnywhere)
		TObjectPtr<USpotLightComponent> Flashlight;

	//プレイヤーのUI
	UPROPERTY()
		TObjectPtr<UUserWidget> PlayerStatusWidget;

	//スコアのインスタンス
	UPROPERTY()
		TObjectPtr<UPlayerScoreInstance> ScoreInstance;

	//敵へのダメージ音のオーディオコンポーネント
	UPROPERTY()
		TObjectPtr<UAudioComponent> EnemyDamageSoundEffect;

	//ライトの色を設定するメソッド
	UFUNCTION()
		void SettingFlashlightColor();

	// 当たり判定の壁貫通をなくす処理
	UFUNCTION()
		FHitResult CheckHitEnemy(AActor* OtherActor);
	//UFUNCTION()
	//	bool IgnoreActorClass(const AActor* Actor);

	//無敵状態
	bool bIsDamageNow;
	//無敵時間用タイマーハンドル
	FTimerHandle NoDamageTimerHandle;
	//無敵時間の処理
	UFUNCTION()
		void NoDamageFunction();

	//スプライン経路を管理するアクター
	TObjectPtr<APlayerSplinePath> SplinePathActor;

	//ライト内(コーンのコリジョン)に入っているオバケを格納するリスト
	TSet<AActor*> OverlappingEnemies;
	TSet<AActor*> TmpOverlapEnemies;
	TSet<AActor*> DamageEnemies;
	TSet<AActor*> TmpDamageEnemies;

	//ライトの色を保持する変数
	EFlashlight_Color Flashlight_Color;

	//ライトバッテリー秒数
	int32 BatteryTime;
	//バッテリー秒数の増加率
	int32 AddBatteryTime;
	//ライトバッテリーの最大値
	int32 MaxBattery;
	//ライトの攻撃力
	int32 LightAttack;
	//攻撃力の増加率
	int32 AddLightAttack;
	//ライトバッテリー
	int32 Battery;
	//ライトバッテリーのUI
	UPROPERTY()
		TObjectPtr<UProgressBar> BatteryUI;

	//スコアのテキストUI
	UPROPERTY()
		TObjectPtr<UTextBlock> ScoreUI;

	//アイテムの攻撃力
	int32 ItemAttack;
	//アイテム所有数のテキストUI
	UPROPERTY()
		TObjectPtr<UTextBlock> ItemUI;
	//アイテム使用状態
	bool bCanUseItem;
	//アイテムクールタイム用タイマーハンドル
	FTimerHandle ItemCoolTimeHandle;
	//アイテムのクールタイム処理
	UFUNCTION()
		void ItemCoolTimeFunction();

	//攻撃アイテムのタイマーハンドル
	FTimerHandle AttackItemTimeHandle;
	UFUNCTION()
		void AttackItemFunction();

	//ライトの操作を受け付けているかどうか
	bool bCanToggleLight;
	//敵に当たっているときの振動が作動中かどうか
	bool bIsEnemyHaptic;
	//プレイヤーダメージの振動が作動中かどうか
	bool bIsPlayerHaptic;
	FTimerHandle HapticTimer;
	
	//グローブの曲げ具合設定
	UPROPERTY(EditAnywhere)
		int32 FingerBendingBorder;

	//出現する敵を判別するステージ番号
	UPROPERTY(BlueprintReadOnly, Category = "Stage", meta = (AllowPrivateAccess = "true"))
		int32 StageNumber;

	//アイテム処理メソッド
	UFUNCTION()
		void UseItem();
	UFUNCTION()
		void UseItem_Attack();
	UFUNCTION()
		void UseItem_Buff();

	//スコア変換の遅延用タイマー
	FTimerHandle ScoreChangeHandle;
	UFUNCTION()
		void ChangeScore_Step();

	//デバッグ用タイマー
	int32 DebugTimer;

	//現在のワールド名
	FString LevelName;
};
