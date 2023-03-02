// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LegacyPlayerBaseComponent.h"
#include "LegacyPlayerMoveComponent.generated.h"

/**
 * 
 */
UCLASS()
class LEGACY_API ULegacyPlayerMoveComponent : public ULegacyPlayerBaseComponent
{
	GENERATED_BODY()

public:

	virtual void SetupPlayerInput(class UInputComponent* PlayerInputComponent) override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void BeginPlay() override;

#pragma region Movement
	UPROPERTY(EditAnywhere, Category = "Player Settings | Inputs | Movement")
	float moveSpeed = 500;

	UFUNCTION()
	void Move(const FInputActionValue& values);
	UFUNCTION()
	void Look(const FInputActionValue& values);
#pragma endregion Movement
};
