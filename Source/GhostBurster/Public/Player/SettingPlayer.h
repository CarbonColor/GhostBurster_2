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

	//�C�x���g��i�߂郁�\�b�h
	UFUNCTION()
	void PushButton(const FInputActionValue& Value);

	//�A�C�e�����g�p���ꂽ���ǂ������m�F���郁�\�b�h
	UFUNCTION(BlueprintCallable)
	void CheckUsedItem(const TArray<int> value);
	UFUNCTION()
	void UseItem_Attack();
	UFUNCTION()
	void UseItem_Buff();
	//�A�C�e���g�p���
	bool bCanUseItem;
	//�A�C�e���N�[���^�C���p�^�C�}�[�n���h��
	FTimerHandle ItemCoolTimeHandle;
	//�A�C�e���̃N�[���^�C������
	UFUNCTION()
	void ItemCoolTimeFunction();


	UFUNCTION()
	void NextEvent();
	UFUNCTION()
	void Reset();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//�ς̗�̃��b�V��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blueprint")
	TSubclassOf<AActor> FoxGhostModel;

	//�����d�������̃i�C�A�K��
	UPROPERTY(EditAnywhere, Category = "Niagara")
	TObjectPtr<UNiagaraSystem> BuffEffectNiagara;

	//�E��̃��b�V��(�����d����StaticMesh)
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> FlashlightMesh;

	//���[�V�����R���g���[���[
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UMotionControllerComponent> RightController;

	// Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_NextEvent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Retry;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC_RightController;

	//�L�����u���[�V�����E�B�W�F�b�g
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TObjectPtr<UWidgetComponent> CalibrationWidgetComponent;

	//�ʏ�̎�̒l
	UPROPERTY(BlueprintReadOnly, Category = "DeviceData")
	TArray<int32> NormalFinger;
	//�ς̎�̒l
	UPROPERTY(BlueprintReadOnly, Category = "DeviceData")
	TArray<int32> FoxFinger;
	//�e�̎�̒l
	UPROPERTY(BlueprintReadOnly, Category = "DeviceData")
	TArray<int32> GunFinger;

	//�C�x���g�̐i�s�x
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Event")
	int32 EventNumber;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Branch")
	bool bIsMeasuring;


	//�l�̃Z�b�e�B���O
	UFUNCTION(BlueprintCallable, Category = "DeviceData")
	void CheckNormalFinger(const TArray<int32> Array);
	UFUNCTION(BlueprintCallable, Category = "DeviceData")
	void CheckFoxFinger(const TArray<int32> Array);
	UFUNCTION(BlueprintCallable, Category = "DeviceData")
	void CheckGunFinger(const TArray<int32> Array);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	//�V�[���R���|�[�l���g
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> VRRoot;

	//�J�����̃R���|�[�l���g
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> CameraComponent;

	//�i�C�A�K���R���|�[�l���g
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	//�X�R�A�̃C���X�^���X
	UPROPERTY()
	TObjectPtr<UPlayerScoreInstance> ScoreInstance;

	//�E�B�W�F�b�g
	TObjectPtr<UUserWidget> TextWidget;

	//�e�L�X�g�u���b�N
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

	//�C�x���g�֐�
	void NormalFingerSetting();
	void FoxFingerSetting();
	void GunFingerSetting();
	void CheckBorder();
	void GoToTitle();
	void Measuring();

	//�C�x���g�^�C�}�[�n���h��
	FTimerHandle EventTimerHandle;
	//�^�C�}�[�ŌĂ΂�郁�\�b�h
	void CanPushButton();
	void StopMeasuring();

	//�e�L�X�g�A�b�v�f�[�g
	void UpdateText(UTextBlock* TextBlock, const TArray<int32> Array);

	//�{�^���̐���
	bool bCanPushButton;
};
