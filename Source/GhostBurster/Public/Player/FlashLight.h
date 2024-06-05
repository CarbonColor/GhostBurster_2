// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FlashLight.generated.h"

UCLASS()
class GHOSTBURSTER_API AFlashLight : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AFlashLight();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	//�X�|�b�g���C�g�R���|�[�l���g
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpotLightComponent* SpotLight;

	//���[�V�����R���g���[���[�R���|�[�l���g
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UMotionControllerComponent* MotionController;

	//�����d����ON/OFF��؂�ւ��鏈��
	void ToggleFlashLight();

};
