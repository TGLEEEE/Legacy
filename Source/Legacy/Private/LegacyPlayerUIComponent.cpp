// Fill out your copyright notice in the Description page of Project Settings.


#include "LegacyPlayerUIComponent.h"

#include "EnhancedInputComponent.h"
#include "LegacyPlayer.h"
#include "Components/ArrowComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Haptics/HapticFeedbackEffect_Curve.h"

#include "LegacyPlayerMagicComponent.h"
#include "Niagara/Public/NiagaraComponent.h"


void ULegacyPlayerUIComponent::BeginPlay()
{
	Super::BeginPlay();

	playerController = Cast<APlayerController>(me->GetController());
	if(!playerController){
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerUIComponent::BeginPlay - Player Controller not found - This can affect haptic feedbacks."));
	}


	//reset widget visibility
	me->leviosoWidgetComponent->SetVisibility(false);
	me->accioWidgetComponent->SetVisibility(false);
	me->depulsoWidgetComponent->SetVisibility(false);
	me->avadaKedavraWidgetComponent->SetVisibility(false);
}


void ULegacyPlayerUIComponent::SetupPlayerInput(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInput(PlayerInputComponent);

	auto inputSystem = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if(inputSystem){
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerUIComponent::SetupPlayerInput - Binding Keys"));
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

void ULegacyPlayerUIComponent::OnActionUIWheelSelection(const FInputActionValue& values)
{
	FVector2D axis = values.Get<FVector2D>();

	//cache the joystick X and Y component to a global variable 
	joystickXComponent = axis.X;
	joystickYComponent = axis.Y;
}
#pragma endregion


void ULegacyPlayerUIComponent::TickComponent(float DeltaTime, ELevelTick TickType,
											 FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	CheckUIState();
	CheckActiveDeadzoneBump();
}

//Checks whether to give bump haptic feedback when transitioning from joystick deadzone <-> active zone
void ULegacyPlayerUIComponent::CheckActiveDeadzoneBump()
{
	//bug: mathematically shorten this
	//if in deadzone
	if (!CheckMagnitude(joystickXComponent) && !CheckMagnitude(joystickYComponent)){

		//if !hasBumped, it means that it's in the initial phase or it has bumped (active->deadzone)
		//so don't play haptic feedback
		if(!hasBumped){return;}

		//if reaches here, it means hasBumped = true
		//play haptic feedback
		playerController->PlayHapticEffect(hFC_SpellSelectionBump, EControllerHand::Right);
		//switch hasBumped to false
		hasBumped = false;		
	}
	//if in active zone
	else{
		//if hasBumped; then it switched over zones; deadzone -> active
		if(hasBumped) {return;}

		//if it reaches here, it means that it hasn't bumped yet; (deadzone -> active)
		playerController->PlayHapticEffect(hFC_SpellSelectionBump, EControllerHand::Right);
		//switch hasBumped to true
		hasBumped = true;
	}
}


/**
 * Description: Inside TICK()
 * This method takes in the position of the joystick and assigns the quadrant.
 * First, it checks if the joystick button is pressed. (might be deprecated)
 * Second, it checks whether the joystick value is too small and in the deadzone. If so, return.
 * If not, then check the angle of the joystick.
 * Third, taking the angle, assign the corresponding quadrant 
 */
void ULegacyPlayerUIComponent::CheckUIState()
{
	
	if (!isUIActivated && !CheckMagnitude(joystickXComponent) && !CheckMagnitude(joystickYComponent)){
		me->leviosoWidgetComponent->SetVisibility(false);
		me->accioWidgetComponent->SetVisibility(false);
		me->depulsoWidgetComponent->SetVisibility(false);
		me->avadaKedavraWidgetComponent->SetVisibility(false);
		return;
	}
	else if(isUIActivated && !CheckMagnitude(joystickXComponent) && !CheckMagnitude(joystickYComponent)){
		me->leviosoWidgetComponent->SetVisibility(true);
		me->accioWidgetComponent->SetVisibility(true);
		me->depulsoWidgetComponent->SetVisibility(true);
		me->avadaKedavraWidgetComponent->SetVisibility(true);
		return;
	}

	#pragma region Debug JoyStick Angles
	////print joystick values and angles
	//UE_LOG(LogTemp, Warning, TEXT("x component-%f"), joystickXComponent);
	//UE_LOG(LogTemp, Warning, TEXT("y component - %f"), joystickYComponent);
	//UE_LOG(LogTemp, Warning, TEXT("joystickAngle - %f"), joystickAngle);
	#pragma endregion 

	//skip rest of the lines if joystick values are too small;don't check quadrants
	if (!CheckMagnitude(joystickXComponent) && !CheckMagnitude(joystickYComponent)) {
		//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerUIComponent::CheckUIState joystick values too small"));
		return;
	}

	//get the angle of the joystick
	joystickAngle = UKismetMathLibrary::DegAtan2(joystickYComponent, joystickXComponent);

	//if both of the joystick magnitude are small, then don't run
	CheckUIQuadrant(joystickAngle);

	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerUIComponent::CheckUIState - quadrantNumber %d"), quadrantNumber);
}



//setting the threshold for the joystick region
bool ULegacyPlayerUIComponent::CheckMagnitude(float& componentMagnitude)
{
	if (FMath::Abs(componentMagnitude) < deadZone) { return false;}							//if less than deadzone threshold, then false
	return true;																			//else true
}

//takes the angle and returns the quadrant number
//gives a haptic feedback
void ULegacyPlayerUIComponent::CheckUIQuadrant(float& angle)
{
	//cache previous quadrant
	int32 previousQuadrant = quadrantNumber;

	//check quadrant number with angle
	if ((angle > 0 && angle <= 45) || (angle > -45 && angle <= 0)){
		quadrantNumber = 1;							//QUADRANT 1
		if (me->magicComponent->wandLightNiagaraComponent && me->magicComponent->wandTrailsNiagaraComponent){
			me->magicComponent->wandLightNiagaraComponent->SetNiagaraVariableLinearColor(FString("Color"), FLinearColor::Yellow);
			me->magicComponent->wandTrailsNiagaraComponent->SetNiagaraVariableLinearColor(FString("Color"), FLinearColor::Yellow);
		}
		//spell selection UI
		me->leviosoWidgetComponent->SetVisibility(true);
		me->accioWidgetComponent->SetVisibility(false);
		me->depulsoWidgetComponent->SetVisibility(false);
		me->avadaKedavraWidgetComponent->SetVisibility(false);
	}
	else if (angle > 45 && angle <= 135) {
		quadrantNumber = 2;								//QUADRANT 2
		//set wand light
		if (me->magicComponent->wandLightNiagaraComponent && me->magicComponent->wandTrailsNiagaraComponent) {
			me->magicComponent->wandLightNiagaraComponent->SetNiagaraVariableLinearColor(FString("Color"), FLinearColor::Red);
			me->magicComponent->wandTrailsNiagaraComponent->SetNiagaraVariableLinearColor(FString("Color"), FLinearColor::Red);
		}
		//spell selection UI
		me->leviosoWidgetComponent->SetVisibility(false);
		me->accioWidgetComponent->SetVisibility(true);
		me->depulsoWidgetComponent->SetVisibility(false);
		me->avadaKedavraWidgetComponent->SetVisibility(false);
	}	
	else if((angle > 135 && angle <= 180) || (angle > -180 && angle <= -135)){
		quadrantNumber = 3;								//QUADRANT 3
		//set wand light
		if (me->magicComponent->wandLightNiagaraComponent && me->magicComponent->wandTrailsNiagaraComponent) {
			me->magicComponent->wandLightNiagaraComponent->SetNiagaraVariableLinearColor(FString("Color"), FLinearColor::Blue);
			me->magicComponent->wandTrailsNiagaraComponent->SetNiagaraVariableLinearColor(FString("Color"), FLinearColor::Blue);
		}
		//spell selection UI
		me->leviosoWidgetComponent->SetVisibility(false);
		me->accioWidgetComponent->SetVisibility(false);
		me->depulsoWidgetComponent->SetVisibility(true);
		me->avadaKedavraWidgetComponent->SetVisibility(false);
	}
	else if (angle > -135 && angle <= -45) {
		quadrantNumber = 4;								//QUADRANT 4
		//set wand light
		if (me->magicComponent->wandLightNiagaraComponent) {
			me->magicComponent->wandLightNiagaraComponent->SetNiagaraVariableLinearColor(FString("Color"), FLinearColor::Green);
			me->magicComponent->wandTrailsNiagaraComponent->SetNiagaraVariableLinearColor(FString("Color"), FLinearColor::Green);
		}
		//spell selection UI
		me->leviosoWidgetComponent->SetVisibility(false);
		me->accioWidgetComponent->SetVisibility(false);
		me->depulsoWidgetComponent->SetVisibility(false);
		me->avadaKedavraWidgetComponent->SetVisibility(true);
	}

	//bug: try this out - if it doesnt work just delete it
	//if in new quadrant
	if(previousQuadrant == quadrantNumber){
		//flag that it is the same spell 
		isInNewQuadrant = false;
	}
	//if in new quadrant
	else{
		//flag that it is a different spell
		isInNewQuadrant = true;
	}

	//potential bug something is buggy; no bump when switching from quadrants 
	//play haptic effect according to which spell was selected
	SpellSelectionHapticFeedback(isInNewQuadrant);
}

void ULegacyPlayerUIComponent::SpellSelectionHapticFeedback(bool isSpellDifferent)
{
	if(playerController){
		//if the same spell, do regular feedback
		if(!isSpellDifferent){
			playerController->PlayHapticEffect(hFC_SpellSelection, EControllerHand::Right);
		}
		//if different spell, give a bump feedback
		else{
			playerController->PlayHapticEffect(hFC_SpellSelectionBump, EControllerHand::Right);
		}
	}
}


