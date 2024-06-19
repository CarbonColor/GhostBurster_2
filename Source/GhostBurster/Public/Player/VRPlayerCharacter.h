// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "VRPlayerCharacter.generated.h"

class USpotLightComponent;
class UInputAction;
class UInputMappingContext;

UCLASS()
class GHOSTBURSTER_API AVRPlayerCharacter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AVRPlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//メソッド
	UFUNCTION()
		void ToggleFlashlight(const FInputActionValue& value);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		UInputAction* IA_Flashlight_OnOff;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		UInputMappingContext* IMC_Flashlight;

private:
	//コンポーネント
	UPROPERTY(VisibleAnywhere)
		USceneComponent* VRRoot;

	UPROPERTY(VisibleAnywhere)
		USpotLightComponent* Flashlight;




};
