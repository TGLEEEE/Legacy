// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LegacyPlayerBaseComponent.h"
#include "LegacyPlayerUIComponent.generated.h"


UCLASS()
class LEGACY_API ULegacyPlayerUIComponent : public ULegacyPlayerBaseComponent
{
	GENERATED_BODY()

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void SetupPlayerInput(UInputComponent* PlayerInputComponent) override;

public:
	UFUNCTION()
	void OnActionUIActivation();
	UFUNCTION()
	void OnActionUIDeActivation();

	UFUNCTION()
	void OnActionUIWheelSelection(const FInputActionValue& values);

	void CheckUIState();
	int32 CheckUIQuadrant(float& angle);					//change this to call by reference?

	bool CheckMagnitude(float& componentMagnitude);

	bool isUIActivated;
	int32 quadrantNumber;
	float joystickAngle;
	float joystickXComponent;
	float joystickYComponent;

};
