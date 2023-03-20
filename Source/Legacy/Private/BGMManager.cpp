// Fill out your copyright notice in the Description page of Project Settings.


#include "BGMManager.h"

#include "LegacyGameMode.h"
#include "Components/AudioComponent.h"

// Sets default values
ABGMManager::ABGMManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bgmComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Comp"));
	bgmComp->SetupAttachment(RootComponent);
	bgmComp->bAutoActivate = false;

	endingBGMComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Ending Audio Comp"));
	endingBGMComp->SetupAttachment(RootComponent);
	endingBGMComp->bAutoActivate = false;
}

// Called when the game starts or when spawned
void ABGMManager::BeginPlay()
{
	Super::BeginPlay();
	gm = Cast<ALegacyGameMode>(GetWorld()->GetAuthGameMode());
}

// Called every frame
void ABGMManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (gm->currentWave == 1 && !bPlayOnce)
	{
		bPlayOnce = true;
		bgmComp->Activate();
	}

	if (gm->currentWave == 4 && !bPlayEndingOnce)
	{
		bPlayEndingOnce = true;
		bgmComp->Deactivate();
		endingBGMComp->Activate();
	}
}

