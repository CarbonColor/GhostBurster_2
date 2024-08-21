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

	//���C�g��ON/OFF��؂�ւ��郁�\�b�h
	UFUNCTION()
		void ToggleFlashlight(const FInputActionValue& value);

	//���C�g�̐F��؂�ւ��郁�\�b�h
	UFUNCTION()
		void ChangeColorFlashlight(const FInputActionValue& value);

	//�o�b�e���[��UI���X�V���郁�\�b�h
	UFUNCTION()
		void UpdateBatteryUI();

	//�A�C�e���̏��L����UI���X�V���郁�\�b�h
	UFUNCTION()
		void UpdateItemUI();

	//�X�R�A��UI���X�V���郁�\�b�h
	UFUNCTION()
		void UpdateScoreUI();


	////�n���h�g���b�L���O�̃Z�b�g�A�b�v
	//UFUNCTION()
	//	void SetupHandTracking();

	////�n���h�g���b�L���O�̃A�b�v�f�[�g
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

	//����̃��b�V��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
		TObjectPtr<USkeletalMeshComponent> HandMesh_Left;

	//�E��̃��b�V��(�����d����StaticMesh)
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Mesh")
		TObjectPtr<UStaticMeshComponent> FlashlightMesh;

	//�v���C���[�̃R���W����(�L���[�u�^)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collision")
		TObjectPtr<UBoxComponent> PlayerCollision;

	//���C�g�̃R���W����(�R�[���^)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collision")
		TObjectPtr<UStaticMeshComponent> LightCollision;

	//�J�����̃R���|�[�l���g
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		TObjectPtr<UCameraComponent> CameraComponent;

	//�v���C���[UI
	UPROPERTY(EditAnywhere, Category = "UI")
		TObjectPtr<UWidgetComponent> PlayerStatusWidgetComponent;

	// �p�����[�^�[�n��
	UPROPERTY(EditAnywhere, Category = "Mesh Parameter")
		FVector FlashlightMeshLocation;

	UPROPERTY(EditAnywhere, Category = "Mesh Parameter")
		FRotator FlashlightMeshRotation;

	UPROPERTY(EditAnywhere, Category = "Mesh Parameter")
		FVector FlashlightMeshScale;

	//���C�g�̃R���W�����Ƃ̓����蔻��
	UFUNCTION()
		void OnConeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnConeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//�I�o�P����̍U��(�C���^�[�t�F�[�X)
	virtual void RecievePlayerDamage() override;


	//Haptic Feedback Effect(�R���g���[���[�̐U��)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Haptics")
		TObjectPtr<UHapticFeedbackEffect_Base> HapticEffect_EnemyDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Haptics")
		TObjectPtr<UHapticFeedbackEffect_Base> HapticEffect_PlayerDamage;

	//�U�����J�n���郁�\�b�h
	UFUNCTION()
		void StartHaptic_EnemyDamage();
	UFUNCTION()
		void StartHaptic_PlayerDamage();
	//�U�����~���郁�\�b�h
	UFUNCTION()
		void StopHapticEffect();

	//�X�e�[�W�ԍ��𑝂₷
	UFUNCTION(BlueprintCallable, Category = "Stage")
		void NextStage();

	//�X�e�[�W�ԍ����擾����
	UFUNCTION(BlueprintCallable, Category = "Stage")
		int GetStageNumber();


private:
	//�V�[���R���|�[�l���g
	UPROPERTY(VisibleAnywhere)
		TObjectPtr<USceneComponent> VRRoot;

	////�X�v�����O�A�[���R���|�[�l���g
	//UPROPERTY(VisibleAnywhere)
	//	TObjectPtr<USpringArmComponent> SpringArmComponent;

	//�X�|�b�g���C�g�R���|�[�l���g
	UPROPERTY(VisibleAnywhere)
		TObjectPtr<USpotLightComponent> Flashlight;

	UPROPERTY()
		TObjectPtr<UUserWidget> PlayerStatusWidget;

	//���C�g�̐F��ݒ肷�郁�\�b�h
	UFUNCTION()
	void SettingFlashlightColor();

	//���G���Ԃ̏���
	UFUNCTION()
	void NoDamageFunction();

	//�X�v���C���o�H���Ǘ�����A�N�^�[
	TObjectPtr<APlayerSplinePath> SplinePathActor;

	//���C�g��(�R�[���̃R���W����)�ɓ����Ă���I�o�P���i�[���郊�X�g
	TSet<AActor*> OverlappingEnemies;

	//���C�g�̐F��ێ�����ϐ�
	EFlashlight_Color Flashlight_Color;

	// UI�֘A
	//���C�g�o�b�e���[�̍ő�l
	const int MaxBattery = 60 * 30;
	//���C�g�o�b�e���[
	int Battery;
	UPROPERTY()
	TObjectPtr<UProgressBar> BatteryUI;
	//�v���C���[�̃X�R�A
	int Score;
	UPROPERTY()
	TObjectPtr<UTextBlock> ScoreUI;
	//�A�C�e���̏��L��
	int Item;
	UPROPERTY()
	TObjectPtr<UTextBlock> ItemUI;

	//���C�g�̑�����󂯕t���Ă��邩�ǂ���
	bool CanToggleLight;
	
	//�I�o�P�ɗ^����U����
	int Attack;
	//�I�o�P����󂯂��U��
	int DamageCount;

	//���G���
	bool DamageNow;
	//���G���ԗp�^�C�}�[�n���h��
	FTimerHandle NoDamageTimerHandle;

	//�o������G�𔻕ʂ���X�e�[�W�ԍ�
	UPROPERTY(BlueprintReadOnly, Category = "Stage", meta = (AllowPrivateAccess = "true"))
		int StageNumber;


	//�f�o�b�O�p�^�C�}�[
	int DebugTimer;
};
