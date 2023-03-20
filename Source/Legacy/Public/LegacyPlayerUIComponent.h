// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LegacyPlayerBaseComponent.h"
#include "LegacyPlayerUIComponent.generated.h"


UCLASS()
class LEGACY_API ULegacyPlayerUIComponent : public ULegacyPlayerBaseComponent
{
	GENERATED_BODY()

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void SetupPlayerInput(UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;
public:
	int32 quadrantNumber;			//number: 1-4
	
private:

#pragma region Input Actions
	//joystick button pressed and released
	UFUNCTION()
	void OnActionUIActivation();
	UFUNCTION()
	void OnActionUIDeActivation();
	
	//joystick swivel
	UFUNCTION()
	void OnActionUIWheelSelection(const FInputActionValue& values);
#pragma endregion

	//constantly checks the position of the joystick position
	void CheckUIState();

#pragma region Get Quandrant
	//takes in the angle of the joystick and returns the quadrant number
	void CheckUIQuadrant(float& angle);					

	//checks if the joystick position is at the edge or in the deadzone
	bool CheckMagnitude(float& componentMagnitude);
	
	bool isUIActivated;				//state changed according to the IA is pressed or released

	float joystickAngle;			//0-180, 0-(-180)
	float joystickXComponent;
	float joystickYComponent;

	UPROPERTY(EditAnywhere, Category = "Spell Selection | Controller", meta = (AllowPrivateAccess = true))
	float deadZone = 0.4;
#pragma endregion 

#pragma region Haptic Feedbacks
	//Haptics
	UPROPERTY()
	APlayerController* playerController;
	
	UPROPERTY(EditAnywhere, Category = "Spell Selection | Haptics", meta = (AllowPrivateAccess = true))
	class UHapticFeedbackEffect_Curve* hFC_SpellSelection;

	UPROPERTY(EditAnywhere, Category = "Spell Selection | Haptics", meta = (AllowPrivateAccess = true))
	class UHapticFeedbackEffect_Curve* hFC_SpellSelectionBump;

	void SpellSelectionHapticFeedback( bool isSpellDifferent);

	void CheckActiveDeadzoneBump();
	bool isInNewQuadrant;

	bool hasBumped = false;
#pragma endregion

#pragma region Spell Selection Animation
	//void SpellSelectionAnimation(class UWidgetComponent* spellWidget, FVector spellArrowComponentLocation);

	//float currentTime;

	//UPROPERTY(EditAnywhere)
	//float animationTime = 1;


	//UPROPERTY()
	//class UUserWidget* leviosoWidget;
	//UPROPERTY()
	//class UUserWidget* accioWidget;
	//UPROPERTY()
	//class UUserWidget* depulsoWidget;
	//UPROPERTY()
	//class UUserWidget* avadaKedavraWidget;

	//UPROPERTY(EditAnywhere, Category = "UI Settings|Score Animation", meta = (BindWidgetAnim), Transient)
	//class UWidgetAnimation* widgetAnimation;
#pragma endregion

};
