// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LegacyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LEGACY_API ALegacyGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> timerWidgetFactory;
	UPROPERTY()
	class UUserWidget* timerWidgetUI;

	UPROPERTY()
	int worldTime;
	UPROPERTY()
	int worldTimeSec;
	UPROPERTY()
	int worldTimeMin;

	FTimerHandle worldtimerHandle;

	void WorldTimer();
	
};
