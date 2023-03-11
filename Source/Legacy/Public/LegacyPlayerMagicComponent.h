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
	AvadaKedavra,
	Grab,
	Cancel
};

UCLASS()
class LEGACY_API ULegacyPlayerMagicComponent : public ULegacyPlayerBaseComponent
{
	GENERATED_BODY()

public:
	virtual void SetupPlayerInput(class UInputComponent* PlayerInputComponent) override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void BeginPlay() override;

#pragma region Input Actions
	UFUNCTION()
	void OnActionCastSpell();
	UFUNCTION()
	void OnActionGrabPressed();
	UFUNCTION()
	void OnActionGrabReleased();

	UFUNCTION()
	void OnActionSpell1Pressed();
	UFUNCTION()
	void OnActionSpell2Pressed();
	UFUNCTION()
	void OnActionSpell3Pressed();


	UFUNCTION()
	void OnActionSpellComboPressed();

	UFUNCTION()
	void OnActionSpellCancelPressed();
#pragma endregion 

#pragma region Magic
	void UpdateSpellState();
	void CheckSpellState(int32& quadrantNumber);

	SpellState spellstate;

	int32 comboCount = 0;

	FVector currentLocation;

	bool isLevioso;
	bool isAccio;
	bool isDepulso;

	bool isSpellCast;
	bool isGrab;
	bool isSpellCombo;
	bool isSpellCancel;
#pragma endregion 

#pragma region Spells
	void CastLevioso();
	void CastAccio();
	void CastDepulso();
	void CastAvadaKedavra();
	void CastGrab();
	void SpellCombo();
	void SpellCancel();
#pragma endregion 

#pragma region Detect Target
	void DetectTarget();

	UPROPERTY()
	class AEnemy* enemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Magic Settings | Object Detection")
	UPrimitiveComponent* detectedComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Magic Settings | Object Detection")
	UPrimitiveComponent* grabbedComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic Settings | Object Detection")
	float detectionRadius = 25;
#pragma endregion

#pragma region Lift
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector objectInitialHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector objectOffsetHeight = FVector(0, 0, 100);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float errorTolerance = 2;
#pragma endregion

	void DereferenceVariables();

	//fx
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* avadaKedavraNiagara;

	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* avadaKedavraNiagaraComponent;

	bool isAvadaKedavraCast;


	//update
	UPROPERTY()
	class ALegacyGameMode* legacyGameMode;

};
