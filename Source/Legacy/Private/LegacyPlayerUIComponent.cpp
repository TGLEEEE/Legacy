// Fill out your copyright notice in the Description page of Project Settings.


#include "LegacyPlayerUIComponent.h"

//update
#include "EnhancedInputComponent.h"
#include "LegacyPlayer.h"
#include "Kismet/KismetMathLibrary.h"

void ULegacyPlayerUIComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	CheckUIState();

}

void ULegacyPlayerUIComponent::SetupPlayerInput(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInput(PlayerInputComponent);

	auto inputSystem = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if(inputSystem){
		UE_LOG(LogTemp, Warning, TEXT("SetupPlayerInput - binding"));
		inputSystem->BindAction(me->iA_UIActivation, ETriggerEvent::Triggered, this, &ULegacyPlayerUIComponent::OnActionUIActivation);
		inputSystem->BindAction(me->iA_UIActivation, ETriggerEvent::Completed, this, &ULegacyPlayerUIComponent::OnActionUIDeActivation);

		inputSystem->BindAction(me->iA_UISelection, ETriggerEvent::Triggered, this, &ULegacyPlayerUIComponent::OnActionUIWheelSelection);
	}
}

#pragma region Inputs
void ULegacyPlayerUIComponent::OnActionUIActivation()
{
	isUIActivated = true;
	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerUIComponent::OnActionUIActivation - isUIActivated"));
}

void ULegacyPlayerUIComponent::OnActionUIDeActivation()
{
	isUIActivated = false;
	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerUIComponent::OnActionUIDeActivation - !isUIActivated"));
}
#pragma endregion

void ULegacyPlayerUIComponent::OnActionUIWheelSelection(const FInputActionValue& values)
{
	FVector2D axis = values.Get<FVector2D>();

	joystickXComponent = axis.X;
	joystickYComponent = axis.Y;
}


void ULegacyPlayerUIComponent::CheckUIState()
{
	//not bug: just wrist hurting, turn off for now
	/*if (!isUIActivated) { return; }*/

	#pragma region Debug
	////print joystick values and angles
	//UE_LOG(LogTemp, Warning, TEXT("x component-%f"), joystickXComponent);
	//UE_LOG(LogTemp, Warning, TEXT("y component - %f"), joystickYComponent);
	//UE_LOG(LogTemp, Warning, TEXT("joystickAngle - %f"), joystickAngle);
	#pragma endregion 

	//print if joystick values are too small
	if (!CheckMagnitude(joystickXComponent) && !CheckMagnitude(joystickYComponent)) {
		//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerUIComponent::CheckUIState joystick values too small"));
		return;
	}

	joystickAngle = UKismetMathLibrary::DegAtan2(joystickYComponent, joystickXComponent);

	//if both of the joystick magnitude are small, then don't run
	quadrantNumber = CheckUIQuadrant(joystickAngle);


	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerUIComponent::CheckUIState - quadrantNumber %d"), quadrantNumber);
}

//setting the threshold for the joystick region
bool ULegacyPlayerUIComponent::CheckMagnitude(float& componentMagnitude)
{
	if (FMath::Abs(componentMagnitude) < 0.4) { return false;}							//too small
	return true;
}

int32 ULegacyPlayerUIComponent::CheckUIQuadrant(float& angle)
{
	if ((angle > 0 && angle <= 45) || (angle > -45 && angle <= 0)){
		return 1;								//QUADRANT 1
	}
	if (angle > 45 && angle <= 135) {
		return 2;								//QUADRANT 2
	}	
	if((angle > 135 && angle <= 180) || (angle > -180 && angle <= -135)){
		return 3;								//QUADRANT 3
	}
	if (angle > -135 && angle <= -45) {
		return 4;								//QUADRANT 4
	}

	return -1;									//just return if some thing weird
}
