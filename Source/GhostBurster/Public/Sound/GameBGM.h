// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"

#include "GameBGM.generated.h"

UCLASS()
class GHOSTBURSTER_API AGameBGM : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameBGM();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	//// Called every frame
	//virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "BGM")
		TArray<USoundCue*> BGM_Cue;

	UFUNCTION(BlueprintCallable)
		void ChangeBGM();


private:
	//シーンコンポーネント
	UPROPERTY(VisibleAnywhere)
		TObjectPtr<USceneComponent> DefaultSceneComponent;

	//BGM用のオーディオコンポーネント
	UPROPERTY()
		TObjectPtr<UAudioComponent> AudioComponent;

	int32 BGMIndex;

	float FadeTime;

	FTimerHandle FadeEndHandle;

	void NextBGM();
};
