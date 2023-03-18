// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyMagicBase.generated.h"

UCLASS()
class LEGACY_API AEnemyMagicBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyMagicBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* mainFXComp;
	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* subFXComp;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* boxComp;
	UPROPERTY(EditAnywhere)
	class UAudioComponent* magicSoundComp;
	UPROPERTY()
	class ALegacyPlayer* player;

	UPROPERTY(EditAnywhere)
	float magicSpeed = 0.3;
	float alpha;
	FVector target;
	FVector startLoc;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
