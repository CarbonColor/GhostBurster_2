// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/EnemyDirectionWIdget.h"

void UEnemyDirectionWIdget::SetEnemyColor(int32 EnemyColor)
{
	FLinearColor ColorValue;
	switch (EnemyColor)
	{
	case 0: //��
		ColorValue = FLinearColor::White;
		break;

	case 1: //��
		ColorValue = FLinearColor::Green;
		break;

	case 2: //��
		ColorValue = FLinearColor::Red;
		break;

	case 3: //��
		ColorValue = FLinearColor::Blue;
		break;

	case 5: //�{�X
		ColorValue = FLinearColor::Yellow;
		break;

	default:
		ColorValue = FLinearColor::Gray;
		break;
	}

	EnemyImage->SetColorAndOpacity(ColorValue);
}