// Fill out your copyright notice in the Description page of Project Settings.


#include "LegacyPlayerBaseComponent.h"

#include "LegacyPlayer.h"

// Sets default values for this component's properties
ULegacyPlayerBaseComponent::ULegacyPlayerBaseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bWantsInitializeComponent = true;
}

void ULegacyPlayerBaseComponent::InitializeComponent()
{
	Super::InitializeComponent();

	me = Cast<ALegacyPlayer>(GetOwner());
	if (me) {
		me->setupPlayerInputDelegate.AddUObject(this, &ULegacyPlayerBaseComponent::SetupPlayerInput);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerBaseComponent::BeginPlay - Can't find Owner"))
	}

}


// Called when the game starts
void ULegacyPlayerBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	me->setupPlayerInputDelegate.AddUObject(this, &ULegacyPlayerBaseComponent::SetupPlayerInput);
}


// Called every frame
void ULegacyPlayerBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}



