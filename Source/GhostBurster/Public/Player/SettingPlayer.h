// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "GameFramework/Pawn.h"
#include "GameData/PlayerScoreInstance.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "TimerManager.h"

#include "SettingPlayer.generated.h"

class UInputAction;
class UInputMappingContext;
class UMotionControllerComponent;


UCLASS()
class GHOSTBURSTER_API ASettingPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASettingPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//イベントを進めるメソッド
	UFUNCTION()
	void PushButton(const FInputActionValue& Value);

	//アイテムが使用されたかどうかを確認するメソッド
	UFUNCTION(BlueprintCallable)
	void CheckUsedItem(const TArray<int> value);
	UFUNCTION()
	void UseItem_Attack();
	UFUNCTION()
	void UseItem_Buff();
	//アイテム使用状態
	bool bCanUseItem;
	//アイテムクールタイム用タイマーハンドル
	FTimerHandle ItemCoolTimeHandle;
	//アイテムのクールタイム処理
	UFUNCTION()
	void ItemCoolTimeFunction();


	UFUNCTION()
	void NextEvent();
	UFUNCTION()
	void Reset();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//狐の霊のメッシュ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blueprint")
	TSubclassOf<AActor> FoxGhostModel;

	//懐中電灯強化のナイアガラ
	UPROPERTY(EditAnywhere, Category = "Niagara")
	TObjectPtr<UNiagaraSystem> BuffEffectNiagara;

	//右手のメッシュ(懐中電灯のStaticMesh)
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> FlashlightMesh;

	//モーションコントローラー
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UMotionControllerComponent> RightController;

	// Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_NextEvent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Retry;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC_RightController;

	//キャリブレーションウィジェット
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TObjectPtr<UWidgetComponent> CalibrationWidgetComponent;

	//通常の手の値
	UPROPERTY(BlueprintReadOnly, Category = "DeviceData")
	TArray<int32> NormalFinger;
	//狐の手の値
	UPROPERTY(BlueprintReadOnly, Category = "DeviceData")
	TArray<int32> FoxFinger;
	//銃の手の値
	UPROPERTY(BlueprintReadOnly, Category = "DeviceData")
	TArray<int32> GunFinger;

	//イベントの進行度
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Event")
	int32 EventNumber;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Branch")
	bool bIsMeasuring;


	//値のセッティング
	UFUNCTION(BlueprintCallable, Category = "DeviceData")
	void CheckNormalFinger(const TArray<int32> Array);
	UFUNCTION(BlueprintCallable, Category = "DeviceData")
	void CheckFoxFinger(const TArray<int32> Array);
	UFUNCTION(BlueprintCallable, Category = "DeviceData")
	void CheckGunFinger(const TArray<int32> Array);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	//シーンコンポーネント
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> VRRoot;

	//カメラのコンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> CameraComponent;

	//ナイアガラコンポーネント
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	//スコアのインスタンス
	UPROPERTY()
	TObjectPtr<UPlayerScoreInstance> ScoreInstance;

	//ウィジェット
	TObjectPtr<UUserWidget> TextWidget;

	//テキストブロック
	TObjectPtr<UTextBlock> EventText_JPN;
	TObjectPtr<UTextBlock> EventText_Eng;
	TObjectPtr<UTextBlock> SencerText;
	TObjectPtr<UTextBlock> NormalText;
	TObjectPtr<UTextBlock> StaticNormalText;
	TObjectPtr<UTextBlock> FoxText;
	TObjectPtr<UTextBlock> StaticFoxText;
	TObjectPtr<UTextBlock> GunText;
	TObjectPtr<UTextBlock> StaticGunText;
	TObjectPtr<UTextBlock> BorderText;
	TObjectPtr<UTextBlock> StaticBorderText;

	//イベント関数
	void NormalFingerSetting();
	void FoxFingerSetting();
	void GunFingerSetting();
	void CheckBorder();
	void GoToTitle();
	void Measuring();

	//イベントタイマーハンドル
	FTimerHandle EventTimerHandle;
	//タイマーで呼ばれるメソッド
	void CanPushButton();
	void StopMeasuring();

	//テキストアップデート
	void UpdateText(UTextBlock* TextBlock, const TArray<int32> Array);

	//ボタンの制御
	bool bCanPushButton;
};
