// Fill out your copyright notice in the Description page of Project Settings.


#include "LegacyPlayerUIComponent.h"

#include "EnhancedInputComponent.h"
#include "LegacyPlayer.h"
#include "Kismet/KismetMathLibrary.h"

void ULegacyPlayerUIComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void ULegacyPlayerUIComponent::SetupPlayerInput(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInput(PlayerInputComponent);

	auto inputSystem = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if(inputSystem){
		UE_LOG(LogTemp, Warning, TEXT("SetupPlayerInput - binding"));
		inputSystem->BindAction(me->iA_UIActivation, ETriggerEvent::Triggered, this, &ULegacyPlayerUIComponent::OnActionUIActivation);
		inputSystem->BindAction(me->iA_UIActivation, ETriggerEvent::Completed, this, &ULegacyPlayerUIComponent::OnActionUIDeActivation);

		inputSystem->BindAction(me->iA_UISelection, ETriggerEvent::Triggered, this, &ULegacyPlayerUIComponent::OnActionUISelection);
	}
}

void ULegacyPlayerUIComponent::OnActionUIActivation()
{
	isUIActivated = true;
}

void ULegacyPlayerUIComponent::OnActionUIDeActivation()
{
	isUIActivated = false;
}

void ULegacyPlayerUIComponent::OnActionUISelection(const FInputActionValue& values)
{
	UE_LOG(LogTemp, Warning, TEXT("OnActionUISelection - triggered"));
	FVector2D axis = values.Get<FVector2D>();

	float angle = UKismetMathLibrary::Atan2(axis.X, axis.Y);

	//float joystickAxisMagnitude = joystickAxis.Size();
	UE_LOG(LogTemp, Warning, TEXT("x component-%f"), axis.X);
	UE_LOG(LogTemp, Warning, TEXT("y component - %f"), axis.Y);
	UE_LOG(LogTemp, Warning, TEXT("y component - %f"), angle);
}


