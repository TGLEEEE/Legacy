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
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class USoundBase* DeadSound;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* swordComp;

	bool bPlayDeadOnce;
	bool bDoDamageOnce;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void SwordCollisionManager();
};
