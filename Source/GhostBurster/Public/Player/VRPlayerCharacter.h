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

	//���C�g��ON/OFF��؂�ւ��郁�\�b�h
	UFUNCTION()
		void ToggleFlashlight(const FInputActionValue& value);

	//���C�g�̐F��؂�ւ��郁�\�b�h
	UFUNCTION()
		void ChangeColorFlashlight(const FInputActionValue& value);

	//�A�C�e�����g�p���ꂽ���ǂ������m�F���郁�\�b�h
	UFUNCTION(BlueprintCallable)
		void CheckUsedItem(const TArray<int> value);


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

	//����̃��b�V��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
		TObjectPtr<USkeletalMeshComponent> HandMesh_Left;

	//�E��̃��b�V��(�����d����StaticMesh)
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Mesh")
		TObjectPtr<UStaticMeshComponent> FlashlightMesh;

	//�ς̗�̃��b�V��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blueprint")
		TSubclassOf<AActor> FoxGhostModel;

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


	//�A�C�e���{�[�_�[���X�g
	UPROPERTY()
		TArray<int32> AttackItemBorder;
	UPROPERTY()
		TArray<int32> BuffItemBorder;

	//���C�g�̃R���W�����Ƃ̓����蔻��
	UFUNCTION()
		void OnConeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnConeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//�A�C�e���̌��𑝂₷����
	UFUNCTION(BlueprintCallable)
		void AddItem();
	//�X�R�A�𑝂₷����
	UFUNCTION(BlueprintCallable)
		void AddScore(int32 Value);

	//�o�b�e���[��UI���X�V���郁�\�b�h
	UFUNCTION()
		void UpdateBatteryUI();
	//�A�C�e���̏��L����UI���X�V���郁�\�b�h
	UFUNCTION()
		void UpdateItemUI();
	//�X�R�A��UI���X�V���郁�\�b�h
	UFUNCTION()
		void UpdateScoreUI();
	//�]�����A�C�e�����X�R�A�ɕϊ����郁�\�b�h�i�^�C�g���p�j
	UFUNCTION()
		void ChangeScore();


	//�I�o�P����̍U��(�C���^�[�t�F�[�X)
	virtual void RecievePlayerDamage() override;

	//Haptic Feedback Effect(�R���g���[���[�̐U��)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Haptics")
		TObjectPtr<UHapticFeedbackEffect_Base> HapticEffect_EnemyDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Haptics")
		TObjectPtr<UHapticFeedbackEffect_Base> HapticEffect_PlayerDamage;

	//�f�o�C�X�ɐU���v���𑗂郁�\�b�h�iBlueprint�Ŏ����j
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Device")
		void GloveDeviceVibration_UseItem();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Device")
		void GloveDeviceVibration_Damage();


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
	
	//�V�[���R���|�[�l���g
	UPROPERTY(VisibleAnywhere)
		TObjectPtr<USceneComponent> VRRoot;

	//�X�|�b�g���C�g�R���|�[�l���g
	UPROPERTY(VisibleAnywhere)
		TObjectPtr<USpotLightComponent> Flashlight;

	//�v���C���[��UI
	UPROPERTY()
		TObjectPtr<UUserWidget> PlayerStatusWidget;

	//�X�R�A�̃C���X�^���X
	UPROPERTY()
		TObjectPtr<UPlayerScoreInstance> ScoreInstance;

	//�G�ւ̃_���[�W���̃I�[�f�B�I�R���|�[�l���g
	UPROPERTY()
		TObjectPtr<UAudioComponent> EnemyDamageSoundEffect;

	//���C�g�̐F��ݒ肷�郁�\�b�h
	UFUNCTION()
		void SettingFlashlightColor();

	// �����蔻��̕Ǌђʂ��Ȃ�������
	UFUNCTION()
		FHitResult CheckHitEnemy(AActor* OtherActor);
	//UFUNCTION()
	//	bool IgnoreActorClass(const AActor* Actor);

	//���G���
	bool bIsDamageNow;
	//���G���ԗp�^�C�}�[�n���h��
	FTimerHandle NoDamageTimerHandle;
	//���G���Ԃ̏���
	UFUNCTION()
		void NoDamageFunction();

	//�X�v���C���o�H���Ǘ�����A�N�^�[
	TObjectPtr<APlayerSplinePath> SplinePathActor;

	//���C�g��(�R�[���̃R���W����)�ɓ����Ă���I�o�P���i�[���郊�X�g
	TSet<AActor*> OverlappingEnemies;
	TSet<AActor*> TmpOverlapEnemies;
	TSet<AActor*> DamageEnemies;
	TSet<AActor*> TmpDamageEnemies;

	//���C�g�̐F��ێ�����ϐ�
	EFlashlight_Color Flashlight_Color;

	//���C�g�o�b�e���[�b��
	int32 BatteryTime;
	//�o�b�e���[�b���̑�����
	int32 AddBatteryTime;
	//���C�g�o�b�e���[�̍ő�l
	int32 MaxBattery;
	//���C�g�̍U����
	int32 LightAttack;
	//�U���͂̑�����
	int32 AddLightAttack;
	//���C�g�o�b�e���[
	int32 Battery;
	//���C�g�o�b�e���[��UI
	UPROPERTY()
		TObjectPtr<UProgressBar> BatteryUI;

	//�X�R�A�̃e�L�X�gUI
	UPROPERTY()
		TObjectPtr<UTextBlock> ScoreUI;

	//�A�C�e���̍U����
	int32 ItemAttack;
	//�A�C�e�����L���̃e�L�X�gUI
	UPROPERTY()
		TObjectPtr<UTextBlock> ItemUI;
	//�A�C�e���g�p���
	bool bCanUseItem;
	//�A�C�e���N�[���^�C���p�^�C�}�[�n���h��
	FTimerHandle ItemCoolTimeHandle;
	//�A�C�e���̃N�[���^�C������
	UFUNCTION()
		void ItemCoolTimeFunction();

	//�U���A�C�e���̃^�C�}�[�n���h��
	FTimerHandle AttackItemTimeHandle;
	UFUNCTION()
		void AttackItemFunction();

	//���C�g�̑�����󂯕t���Ă��邩�ǂ���
	bool bCanToggleLight;
	//�G�ɓ������Ă���Ƃ��̐U�����쓮�����ǂ���
	bool bIsEnemyHaptic;
	//�v���C���[�_���[�W�̐U�����쓮�����ǂ���
	bool bIsPlayerHaptic;
	FTimerHandle HapticTimer;
	
	//�O���[�u�̋Ȃ���ݒ�
	UPROPERTY(EditAnywhere)
		int32 FingerBendingBorder;

	//�o������G�𔻕ʂ���X�e�[�W�ԍ�
	UPROPERTY(BlueprintReadOnly, Category = "Stage", meta = (AllowPrivateAccess = "true"))
		int32 StageNumber;

	//�A�C�e���������\�b�h
	UFUNCTION()
		void UseItem();
	UFUNCTION()
		void UseItem_Attack();
	UFUNCTION()
		void UseItem_Buff();

	//�X�R�A�ϊ��̒x���p�^�C�}�[
	FTimerHandle ScoreChangeHandle;
	UFUNCTION()
		void ChangeScore_Step();

	//�f�o�b�O�p�^�C�}�[
	int32 DebugTimer;

	//���݂̃��[���h��
	FString LevelName;
};
