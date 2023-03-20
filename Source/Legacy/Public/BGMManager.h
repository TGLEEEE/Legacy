// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BGMManager.generated.h"

UCLASS()
class LEGACY_API ABGMManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABGMManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UAudioComponent* bgmComp;
	UPROPERTY(EditAnywhere)
	class UAudioComponent* endingBGMComp;
	UPROPERTY()
	class ALegacyGameMode* gm;
	bool bPlayOnce;
	bool bPlayEndingOnce;

};
