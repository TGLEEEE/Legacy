// Fill out your copyright notice in the Description page of Project Settings.


#include "LegacyPlayerMoveComponent.h"
#include "EnhancedInputComponent.h"
#include "LegacyPlayer.h"


void ULegacyPlayerMoveComponent::BeginPlay()
{
	Super::BeginPlay();
}



void ULegacyPlayerMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


void ULegacyPlayerMoveComponent::SetupPlayerInput(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInput(PlayerInputComponent);


	auto inputSystem = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (inputSystem) {
		inputSystem->BindAction(me->iA_VRMove, ETriggerEvent::Triggered, this, &ULegacyPlayerMoveComponent::Move);
		inputSystem->BindAction(me->iA_Mouse, ETriggerEvent::Triggered, this, &ULegacyPlayerMoveComponent::Look);
	}
	
}

void ULegacyPlayerMoveComponent::Move(const FInputActionValue& values)
{
	FVector2D axis = values.Get<FVector2D>();

	me->AddMovementInput(me->GetActorForwardVector(), axis.X);
	me->AddMovementInput(me->GetActorRightVector(), axis.Y);
}

void ULegacyPlayerMoveComponent::Look(const FInputActionValue& values)
{
	FVector2D axis = values.Get<FVector2D>();
	me->AddControllerYawInput(axis.X);
	me->AddControllerPitchInput(axis.Y);
}
