// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "EnemyPaladin.generated.h"

/**
 * 
 */
UCLASS()
class LEGACY_API AEnemyPaladin : public AEnemy
{
	GENERATED_BODY()

public:
	AEnemyPaladin();
	void virtual BeginPlay() override;
	void virtual Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class USoundBase* DeadSound;
	bool bPlayDeadOnce;
};
