// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LegacyPlayerBaseComponent.h"
#include "LegacyPlayerMagicComponent.generated.h"

UENUM(BlueprintType)
enum class SpellState : uint8 {
	Rest, 
	Levioso,
	Accio,
	Depulso,
	Grab
};

UCLASS()
class LEGACY_API ULegacyPlayerMagicComponent : public ULegacyPlayerBaseComponent
{
	GENERATED_BODY()

public:

	virtual void SetupPlayerInput(class UInputComponent* PlayerInputComponent) override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void BeginPlay() override;

#pragma region
	UFUNCTION()
	void OnActionCastSpell();
	UFUNCTION()
	void OnActionGrabPressed();
	UFUNCTION()
	void OnActionGrabReleased();

	UFUNCTION()
	void OnActionSpell1Pressed();
	void OnActionSpell1Released();
	UFUNCTION()
	void OnActionSpell2Pressed();
	void OnActionSpell2Released();
	UFUNCTION()
	void OnActionSpell3Pressed();
	void OnActionSpell3Released();
#pragma endregion Input Action

#pragma region Magic
	void UpdateSpellState();
	void CheckSpellState();

	SpellState spellstate;

	void CastLevioso();
	void CastAccio();
	void CastDepulso();
	void CastGrab();


	bool isSpellCast;
	bool isGrab;

	bool isLevioso;
	bool isAccio;
	bool isDepulso;
#pragma endregion 

#pragma region Detect Target
	void DetectTarget();

	class AEnemy* enemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Magic Settings | Object Detection")
	UPrimitiveComponent* detectedComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Magic Settings | Object Detection")
	UPrimitiveComponent* grabbedComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic Settings | Object Detection")
	float detectionRadius = 25;
#pragma endregion

#pragma region Lift
	void LiftTarget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector objectInitialHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector objectOffsetHeight = FVector(0, 0, 100);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float errorTolerance = 2;
#pragma endregion 



};
