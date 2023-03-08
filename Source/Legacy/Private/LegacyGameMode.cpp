// Fill out your copyright notice in the Description page of Project Settings.


#include "LegacyGameMode.h"
#include "Blueprint/UserWidget.h"

void ALegacyGameMode::BeginPlay()
{
	Super::BeginPlay();
	WorldTimer();
	timerWidgetUI = CreateWidget(GetWorld(), timerWidgetFactory);
	timerWidgetUI->AddToViewport();
}

void ALegacyGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void ALegacyGameMode::WorldTimer()
{
	GetWorldTimerManager().SetTimer(worldtimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			worldTime++;
			worldTimeSec = worldTime % 60;
			worldTimeMin = worldTime / 60;
			UE_LOG(LogTemp, Error, TEXT("world time is : %d"), worldTime);
		}), 1.f, true);
}