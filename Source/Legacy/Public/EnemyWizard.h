// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "EnemyMagicBase.h"
#include "EnemyWizard.generated.h"

/**
 * 
 */
UCLASS()
class LEGACY_API AEnemyWizard : public AEnemy
{
	GENERATED_BODY()

public:
	AEnemyWizard();
	void virtual BeginPlay() override;
	void virtual Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere)
	float wizardAttackRange = 1200;
	UPROPERTY(EditAnywhere)
	float wizardAttackDelay = 4;
	UPROPERTY(EditAnywhere)
	float rateForTornado = 0.15;
	bool bPlayDeadOnce;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* wandComp;
	UPROPERTY(EditAnywhere)
	class USoundBase* DeadSound1;
	UPROPERTY(EditAnywhere)
	class USoundBase* DeadSound2;
	UPROPERTY(EditAnywhere)
	class USoundBase* DeadSound3;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AEnemyMagicBase> enemyMagicFactorySpark;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AEnemyMagicBase> enemyMagicFactoryTornado;

	void WizardAttack();
};
