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
	Ultimate,
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
	//PC
	UFUNCTION()
	void OnActionCastSpellPressed();
	UFUNCTION()
	void OnActionCastSpellReleased();
	//will unflag castspell with onoverlap
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
	void OnActionSpell4Pressed();
	UFUNCTION()
	void OnActionSpellComboPressed();
	UFUNCTION()
	void OnActionSpellComboReleased();
	UFUNCTION()
	void OnActionSpellUltimateHold();
	UFUNCTION()
	void OnActionSpellUltimateReleased();


	//temporary
	UFUNCTION()
	void OnActionSpellCancelPressed();
#pragma endregion 

#pragma region Magic
	void UpdateSpellState();
	void CheckSpellState(int32& quadrantNumber);

	void CheckSpellActivation();

	void CheckSpellComboActivation();
#pragma region Wide Sphere Trace
	//for debugging
	int32 comboCountOnEnemy;

	FVector comboImpactPoint;
	FVector comboImpactNormal;

	class AEnemy* WideSphereTrace();

	UPROPERTY()
	AEnemy* wideSphereTraceHitEnemy;
	UPROPERTY()
	AEnemy* previousWideSphereTraceHitEnemy;

	UPROPERTY(EditAnywhere)
	float farSphereTraceDetectionRadius = 100;
	UPROPERTY(EditAnywhere)
	float nearSphereTraceDetectionRadius = 20;
	UPROPERTY(EditAnywhere)
	float farSphereTraceDistance = 100000;
	UPROPERTY(EditAnywhere)
	float nearSphereTraceDistance = 300;
#pragma endregion 

	bool isWandActive;

	UPROPERTY(EditAnywhere)
	float accelerationThreshold = 7300;
	UPROPERTY(EditAnywhere)
	float velocityThreshold = 140;			//vel = 198, accel -148000
	UPROPERTY(EditAnywhere)
	float accelerationDiffernceThreshold = 8000;


	SpellState spellState;

	int32 comboCount = 0;

	FVector currentLocation;

	bool isGrab;
	bool isSpellCast;
	bool isLevioso;
	bool isAccio;
	bool isDepulso;
	bool isAvadaKedavra;

	bool isSpellCombo;
	bool isSpellUltimate;

	bool isSpellCancel;
#pragma endregion 

#pragma region Spells
	void CastGrab();
	void CastLevioso();
	void CastAccio();
	void CastDepulso();
	void CastAvadaKedavra();
	void CastGrabbedSpellCombo();
	void CastSpellCombo();
	//PC
	void CastUltimate();
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


#pragma region Accio
	FVector accioHoverLocation;

#pragma endregion


#pragma region Spell VFX
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* avadaKedavraNiagaraSystem;
	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* avadaKedavraNiagaraComponent;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* wandLightNiagaraSystem;
	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* wandLightNiagaraComponent;

	void UpdateWandLight();

	//Attack
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* spellComboNiagaraSystem;
	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* spellComboNiagaraComponent;

	void SpawnSpellComboNiagaraEffect();
#pragma endregion 

#pragma region Cancel
	void DereferenceVariables();

	void CancelSpellTimer(float spellTime);
	FTimerHandle spellCancelTimerHandle;

	//write category
	UPROPERTY(EditAnywhere)
	float leviosoCancelTime = 3;
	UPROPERTY(EditAnywhere)
	float accioCancelTime = 2;
	UPROPERTY(EditAnywhere)
	float avadaKedavraCancelTime = 3;
	UPROPERTY(EditAnywhere)
	float ultimateCancelTime = 2;

	float currentTime;
#pragma endregion 

};


