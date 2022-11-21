// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PointsWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5TESTTASK_API UPointsWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere);
	class UTextBlock* PointsTextBlock;
public:
	void SetPoints(float points);

	
	
};
