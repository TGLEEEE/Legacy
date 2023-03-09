// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LegacyPlayerBaseComponent.h"
#include "LegacyPlayerUIComponent.generated.h"

/**
 * 
 */
UCLASS()
class LEGACY_API ULegacyPlayerUIComponent : public ULegacyPlayerBaseComponent
{
	GENERATED_BODY()

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void SetupPlayerInput(UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnActionUIActivation();
	UFUNCTION()
	void OnActionUIDeActivation();

	UFUNCTION()
	void OnActionUISelection(const FInputActionValue& values);

	bool isUIActivated;


};
