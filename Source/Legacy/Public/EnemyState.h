// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyState.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LEGACY_API UEnemyState : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyState();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UPROPERTY()
	class AEnemy* me;
	UPROPERTY()
	class ALegacyPlayer* player;

	UPROPERTY(EditAnywhere)
	int hp = 10;
	UPROPERTY(EditAnywhere)
	float mass = 100;

	UFUNCTION(BlueprintCallable)
	void OnDamageProcess(int amount);

	UFUNCTION()
	void Throw(FVector force, int Amount);

	UFUNCTION()
	void SetOriginalPhysicsState();

	float originZ;
};
