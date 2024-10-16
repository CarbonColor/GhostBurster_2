// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/EnemyDirectionWIdget.h"

void UEnemyDirectionWIdget::SetEnemyColor(int32 EnemyColor)
{
	FLinearColor ColorValue;
	switch (EnemyColor)
	{
	case 0: //”’
		ColorValue = FLinearColor::White;
		break;

	case 1: //—Î
		ColorValue = FLinearColor::Green;
		break;

	case 2: //Ô
		ColorValue = FLinearColor::Red;
		break;

	case 3: //ûò
		ColorValue = FLinearColor::Blue;
		break;

	case 5: //ƒ{ƒX
		ColorValue = FLinearColor::Yellow;
		break;

	default:
		ColorValue = FLinearColor::Gray;
		break;
	}

	EnemyImage->SetColorAndOpacity(ColorValue);
}