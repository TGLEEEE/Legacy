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

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AEnemy> enemyPaladinFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AEnemy> enemyWizardFactory;

	int worldTime;
	int worldTimeSec;
	int worldTimeMin;
	int currentCountForSpawnPaladin;
	int tempCountForSpawnPaladin;
	int currentCountForSpawnWizard;
	int tempCountForSpawnWizard;
	int enemyKillCount;
	int enemyCountTotal;
	int currentWave;
	bool bIsInWave;
	TArray<AActor*> arrForCount;

	FTimerHandle worldtimerHandle;
	FTimerHandle spawnPaladinHandle;
	FTimerHandle spawnWizardHandle;

	UFUNCTION(BlueprintCallable)
	void WaveStart();
	void WorldTimer();
	void SpawnEnemyPaladin(int spawnCount);
	void SpawnEnemyWizard(int spawnCount);
	void WaveStageManager(int wave);
	void UpdateEnemyCountTotal();

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
