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

	//��_���[�W���������郁�\�b�h
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	//�X�|�b�g���C�g�R���|�[�l���g
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class USpotLightComponent* SpotLight;

	//���[�V�����R���g���[���[�R���|�[�l���g
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UMotionControllerComponent* RightHandController;

	//�E��̃��b�V��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UStaticMeshComponent* RightHandMesh;

	//�X�|�b�g���C�g�𑀍삷�郁�\�b�h
	void ToggleFlashLight();

	//�_���[�W�񐔂��L�^����ϐ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GamePlay")
		int32 DamageCount;
};
