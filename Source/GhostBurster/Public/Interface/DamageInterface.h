// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Player/Flashlight_Enumeration.h"
#include "DamageInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDamageInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GHOSTBURSTER_API IDamageInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	//オバケが受けるダメージ
	virtual void RecieveEnemyDamage(int DamageAmount, EFlashlight_Color Color) PURE_VIRTUAL(IDamageInterface::ReceiveEnemyDamage, );

	//オバケに与えるアイテムのダメージ
	virtual void RecieveItemDamage(int DamageAmount) PURE_VIRTUAL(IDamageInterface::RecieveItemDamage, );

	//プレイヤーが受けるダメージ
	virtual void RecievePlayerDamage() PURE_VIRTUAL(IDamageInterface::RecievePlayerDamage, );

};
