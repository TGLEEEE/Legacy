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

#pragma region Input Action
	UFUNCTION()
	void OnActionCastSpell();
	UFUNCTION()
	void OnActionGrabPressed();
	UFUNCTION()
	void OnActionGrabReleased();

	UFUNCTION()
	void OnActionSpell1();
	UFUNCTION()
	void OnActionSpell2();
	UFUNCTION()
	void OnActionSpell3();

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
#pragma endregion Magic

};
