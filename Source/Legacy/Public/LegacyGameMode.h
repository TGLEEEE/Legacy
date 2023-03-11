// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HeadMountedDisplayTypes.h"						//update
#include "LegacyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LEGACY_API ALegacyGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	ALegacyGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY()
	int worldTime;
	UPROPERTY()
	int worldTimeSec;
	UPROPERTY()
	int worldTimeMin;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> timerWidgetFactory;
	UPROPERTY()
	class UUserWidget* timerWidgetUI;

	FTimerHandle worldtimerHandle;

	void WorldTimer();

#pragma region VR
	bool isHMDActivated;
	bool isControllersFound;

	FVector previousPosition = FVector::Zero();
	FVector previousVelocity = FVector::Zero();
	float previousTime = 0.f;


	void GetControllers();

	FVector CalculateControllerPosition(FXRMotionControllerData& controllerData);
	FVector CalculateControllerVelocity(FVector currentPosition);
	FVector CalculateControllerAcceleration(FVector currentVelocity);

	UPROPERTY()
	class ALegacyPlayer* legacyPlayer;

	FXRMotionControllerData leftControllerData;
	FXRMotionControllerData rightControllerData;



#pragma endregion 
};
