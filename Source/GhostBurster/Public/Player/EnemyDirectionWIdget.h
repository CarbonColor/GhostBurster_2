// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Flashlight_Enumeration.h"
#include "EnemyDirectionWIdget.generated.h"

/**
 * 
 */
UCLASS()
class GHOSTBURSTER_API UEnemyDirectionWIdget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> EnemyImage;

	UFUNCTION(BlueprintCallable)
	void SetEnemyColor(int32 EnemyColor);

};
