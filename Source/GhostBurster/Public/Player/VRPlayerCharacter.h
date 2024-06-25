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

	//���C�g��ON/OFF��؂�ւ��郁�\�b�h
	UFUNCTION()
	void ToggleFlashlight(const FInputActionValue& value);

	//���C�g�̐F��؂�ւ��郁�\�b�h
	UFUNCTION()
	void ChangeColorFlashlight(const FInputActionValue& value);

	//���G��ԃ^�C�}�[���I��������̏���
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

	//���C�g�̃R���W����(�R�[���^)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collision")
	TObjectPtr<UStaticMeshComponent> LightCollision;

	//���C�g�̃R���W�����Ƃ̓����蔻��
	UFUNCTION()
	void OnConeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void OnConeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//�I�o�P����̍U��(�C���^�[�t�F�[�X)
	virtual void RecievePlayerDamage() override;

private:
	//�V�[���R���|�[�l���g
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> VRRoot;

	//�X�|�b�g���C�g�R���|�[�l���g
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpotLightComponent> Flashlight;

	//���C�g�̐F��ݒ肷�郁�\�b�h
	UFUNCTION()
	void SettingFlashlightColor();

	//���C�g��(�R�[���̃R���W����)�ɓ����Ă���I�o�P���i�[���郊�X�g
	TSet<AActor*> OverlappingEnemies;

	//���C�g�̐F��ێ�����ϐ�
	EFlashlight_Color Flashlight_Color;
	
	//�I�o�P�ɗ^����U����
	int Attack;

	//�I�o�P����󂯂��U��
	int DamageCount;

	//���G���
	bool DamageNow;

	//���G��Ԃ��I���邽�߂̃^�C�}�[�n���h��
	FTimerHandle TimerHandle;

};
