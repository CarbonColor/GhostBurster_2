// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Flashlight_Enumeration.h"
#include "Interface/DamageInterface.h"
#include "VRPlayerCharacter.generated.h"

class USpotLightComponent;
class UInputAction;
class UInputMappingContext;
class UMotionControllerComponent;

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

	//無敵状態タイマーが終わった時の処理
	UFUNCTION()
	void EndTimeHandle();

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
	TObjectPtr<UInputMappingContext> IMC_Flashlight;

	// Motion Controller
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UMotionControllerComponent> MotionController_Right;

	//ライトのコリジョン(コーン型)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collision")
	TObjectPtr<UStaticMeshComponent> LightCollision;

	//ライトのコリジョンとの当たり判定
	UFUNCTION()
	void OnConeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void OnConeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//オバケからの攻撃(インターフェース)
	virtual void RecievePlayerDamage() override;

private:
	//シーンコンポーネント
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> VRRoot;

	//スポットライトコンポーネント
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpotLightComponent> Flashlight;

	//ライトの色を設定するメソッド
	UFUNCTION()
	void SettingFlashlightColor();

	//ライト内(コーンのコリジョン)に入っているオバケを格納するリスト
	TSet<AActor*> OverlappingEnemies;

	//ライトの色を保持する変数
	EFlashlight_Color Flashlight_Color;
	
	//オバケに与える攻撃力
	int Attack;

	//オバケから受けた攻撃
	int DamageCount;

	//無敵状態
	bool DamageNow;

	//無敵状態を終えるためのタイマーハンドル
	FTimerHandle TimerHandle;

};
