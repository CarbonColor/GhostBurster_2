// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MotionControllerComponent.h"
#include "VRPlayer.generated.h"

UCLASS()
class GHOSTBURSTER_API AVRPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVRPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//被ダメージを処理するメソッド
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	//スポットライトコンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class USpotLightComponent* SpotLight;

	//モーションコントローラーコンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UMotionControllerComponent* RightHandController;

	//右手のメッシュ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UStaticMeshComponent* RightHandMesh;

	//スポットライトを操作するメソッド
	void ToggleFlashLight();

	//ダメージ回数を記録する変数
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GamePlay")
		int32 DamageCount;
};
