// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyState.generated.h"

UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	PALADIN,
	WIZARD,
	BOSS
};

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

	UPROPERTY(EditAnywhere)
	int hp = 2;
	UPROPERTY(EditAnywhere)
	float mass = 100;
	UPROPERTY(EditAnywhere)
	bool bIsGrabbed;
	bool bReadyToAir = true;
	EEnemyType ownerType;

	UFUNCTION(BlueprintCallable)
	void OnDamageProcess(int amount);
	UFUNCTION()
	void Throw(FVector force, int Amount);
	UFUNCTION()
	void SetOriginalPhysicsState();
	
};
