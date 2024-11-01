// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SpotLightComponent.h"
#include "Enemy/BossEnemy.h"
#include "Sound/SoundCue.h"
#include "BossEvent.generated.h"

UCLASS()
class GHOSTBURSTER_API ABossEvent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossEvent();

	UFUNCTION(BlueprintCallable, Category = "Boss Events")
	void BossAppearEvent(ABossEnemy* Boss);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UPROPERTY(EditAnywhere, Category = "Light")
	TObjectPtr<USpotLightComponent> SpotLight;

	UPROPERTY(EditAnywhere, Category = "SE")
	TObjectPtr<USoundCue> LightSound;


private:

	FTimerHandle TimerHandle;
	FTimerHandle LightHandle;

	TObjectPtr<ABossEnemy> BossEnemy;
	int32 BlinkCount;
	int32 UpdateCount;

	float TargetIntensity;
	float AddIntensity;

	void BlinkLight();
	void IncreaseLightIntensity();
	void UpdateIntensity();

};
