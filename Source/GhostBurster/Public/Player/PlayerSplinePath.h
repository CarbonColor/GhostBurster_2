// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "PlayerSplinePath.generated.h"

UCLASS()
class GHOSTBURSTER_API APlayerSplinePath : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerSplinePath();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spline", meta = (AllowPrivateAccess = "true"))
		TObjectPtr<USplineComponent> SplineComponent;

	//経路上の現在地を0～1で表す
	float CurrentSplineDistance;
	//移動速度
	UPROPERTY(EditAnywhere, Category = "Speed")
	float MovementSpeed;

public:
	UFUNCTION(BlueprintCallable, Category = "Spline")
		void SetMovementSpeed(float Speed);

	UFUNCTION(BlueprintCallable, Category = "Spline")
		void StopMovement();

	UFUNCTION(BlueprintCallable, Category = "Spline")
		void ResumeMovement();

	UFUNCTION(BlueprintCallable, Category = "Spline")
		FVector GetLocationAtCurrentDistance() const;

	UFUNCTION(BlueprintCallable, Category = "Spline")
		void MoveAlongSpline(float DeltaTime);

};
