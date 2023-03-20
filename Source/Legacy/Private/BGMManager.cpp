// Fill out your copyright notice in the Description page of Project Settings.


#include "BGMManager.h"

#include "LegacyGameMode.h"
#include "Components/AudioComponent.h"

// Sets default values
ABGMManager::ABGMManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bgmComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Main Audio Comp"));
	bgmComp->SetupAttachment(RootComponent);
	bgmComp->bAutoActivate = false;
	bgmComp->VolumeMultiplier = 0.4f;

	endingBGMComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Ending Audio Comp"));
	endingBGMComp->SetupAttachment(RootComponent);
	endingBGMComp->bAutoActivate = false;
	endingBGMComp->VolumeMultiplier = 0.6f;

	ambientSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Ambient Audio Comp"));
	ambientSoundComp->SetupAttachment(RootComponent);
	ambientSoundComp->bAutoActivate = false;
	ambientSoundComp->VolumeMultiplier = 4.f;
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
		ambientSoundComp->Activate();
	}

	if (gm->currentWave == 4 && !bPlayEndingOnce)
	{
		bPlayEndingOnce = true;
		bgmComp->Deactivate();
		endingBGMComp->Activate();
	}
}

