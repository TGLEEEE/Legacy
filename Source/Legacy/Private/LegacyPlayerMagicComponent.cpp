// Fill out your copyright notice in the Description page of Project Settings.


#include "LegacyPlayerMagicComponent.h"
#include "EnhancedInputComponent.h"
#include "LegacyPlayer.h"

#include "Enemy.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EnemyState.h"
#include "LegacyPlayerUIComponent.h"
#include "Niagara/Public/NiagaraFunctionLibrary.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "LegacyGameMode.h"

#include "MotionControllerComponent.h"
#include "Camera/CameraComponent.h"


void ULegacyPlayerMagicComponent::BeginPlay()
{
	Super::BeginPlay();

	//what does this do?? makes the hand follow the camera?
	if (!UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled()) {
		me->rightHand->SetRelativeRotation(me->cameraComp->GetRelativeRotation());
	}
}


void ULegacyPlayerMagicComponent::SetupPlayerInput(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInput(PlayerInputComponent);

#pragma region Input Action Binding
	auto inputSystem = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (inputSystem) {
		inputSystem->BindAction(me->iA_CastSpell, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionCastSpellPressed);
		inputSystem->BindAction(me->iA_CastSpell, ETriggerEvent::Completed, this, &ULegacyPlayerMagicComponent::OnActionCastSpellReleased);

		inputSystem->BindAction(me->iA_Grab, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionGrabPressed);
		inputSystem->BindAction(me->iA_Grab, ETriggerEvent::Completed, this, &ULegacyPlayerMagicComponent::OnActionGrabReleased);

		inputSystem->BindAction(me->iA_Spell1, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionSpell1Pressed);
		inputSystem->BindAction(me->iA_Spell2, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionSpell2Pressed);
		inputSystem->BindAction(me->iA_Spell3, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionSpell3Pressed);
		inputSystem->BindAction(me->iA_Spell4, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionSpell4Pressed);

		inputSystem->BindAction(me->iA_SpellCombo, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionSpellComboPressed);
		inputSystem->BindAction(me->iA_SpellCombo, ETriggerEvent::Completed, this, &ULegacyPlayerMagicComponent::OnActionSpellComboReleased);

		inputSystem->BindAction(me->iA_SpellUltimate, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionSpellUltimateHold);
		inputSystem->BindAction(me->iA_SpellUltimate, ETriggerEvent::Completed, this, &ULegacyPlayerMagicComponent::OnActionSpellUltimateReleased);

		//temporary
		inputSystem->BindAction(me->iA_SpellCancel, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionSpellCancelPressed);
	}
#pragma endregion

}

#pragma region Input Action
void ULegacyPlayerMagicComponent::OnActionCastSpellPressed()
{
	isSpellCast = true;
}

void ULegacyPlayerMagicComponent::OnActionCastSpellReleased()
{
	isSpellCast = false;
}


void ULegacyPlayerMagicComponent::OnActionGrabPressed()
{
	isGrab = true;
}

void ULegacyPlayerMagicComponent::OnActionGrabReleased()
{
	isGrab = false;
}

void ULegacyPlayerMagicComponent::OnActionSpell1Pressed()
{
	isLevioso = true;
}

void ULegacyPlayerMagicComponent::OnActionSpell2Pressed()
{
	isAccio = true;
}


void ULegacyPlayerMagicComponent::OnActionSpell3Pressed()
{
	isDepulso = true;
}

void ULegacyPlayerMagicComponent::OnActionSpell4Pressed()
{
	isAvadaKedavra = true;
}

void ULegacyPlayerMagicComponent::OnActionSpellComboPressed()
{
	isSpellCombo = true;
	comboCount++;
}
void ULegacyPlayerMagicComponent::OnActionSpellComboReleased()
{
	isSpellCombo = false;
}

void ULegacyPlayerMagicComponent::OnActionSpellUltimateHold()
{
	isSpellUltimate = true;
}

void ULegacyPlayerMagicComponent::OnActionSpellUltimateReleased()
{
	isSpellUltimate = false;
}

//bug: temporary; dont need this because will do timer
void ULegacyPlayerMagicComponent::OnActionSpellCancelPressed()
{
	isSpellCancel = true;
}


#pragma endregion

void ULegacyPlayerMagicComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DetectTarget();

	UpdateSpellState();
	UpdateWandEffects();

	CheckSpellActivation();
	CheckSpellComboActivation();
}

void ULegacyPlayerMagicComponent::CheckSpellComboActivation()
{
	// only pressing combo button, the wand is active, and there is a hit result
	if (isGrab || !isSpellCombo || !isWandActive) {return;}

	AEnemy* wideSphereTraceResult = WideSphereTrace();

	//if hit enemy and there is no previous hit result
	if (wideSphereTraceResult && !previousWideSphereTraceHitEnemy) {
		//Spawning Combo Niagara impact effect
		SpawnSpellComboNiagaraEffect();

		wideSphereTraceHitEnemy = wideSphereTraceResult;

		//apply damage to enemy
		if(wideSphereTraceHitEnemy){
			wideSphereTraceHitEnemy->enemyState->OnDamageProcess(1);
		}

		//comboCountOnEnemy++;					//for debugging
	}
	//if there is a result, but it's still hitting, don't count as hit
	else if(wideSphereTraceResult && previousWideSphereTraceHitEnemy){
		return;
	}

	//assign previous hit result according to current hit result
	previousWideSphereTraceHitEnemy = wideSphereTraceResult;
}

void ULegacyPlayerMagicComponent::SpawnSpellComboNiagaraEffect()
{
//	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastAvadaKedavra - !isAvadaKedavra"));

	//potential bug; neet to spawn and attach, not just location? Also, need to destroy and not loop
	spellComboNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), spellComboNiagaraSystem, comboImpactPoint,
		FRotator(0),FVector(0.2), true, true, ENCPoolMethod::None, true);

}


class AEnemy* ULegacyPlayerMagicComponent::WideSphereTrace()
{
	//wand position, trace channel, actors to ignore
	FVector wandPosition = me->wandLightArrowComponent->GetComponentLocation();
	TEnumAsByte<ECollisionChannel> traceChannel;
	traceChannel = ECollisionChannel::ECC_Visibility;
	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(me);


	FVector traceShortStartLocation = wandPosition + me->wandLightArrowComponent->GetForwardVector() * nearSphereTraceDetectionRadius;				//add with detection radius so that the trace doesn't start from the back of the camera
	FVector traceShortEndLocation = traceShortStartLocation + me->wandStaticMeshComponent->GetForwardVector() * nearSphereTraceDistance;
	//short and thin single sphere trace
	FHitResult hitResult;
	bool isShortSphereTraceHit = UKismetSystemLibrary::SphereTraceSingle(this, traceShortStartLocation, traceShortEndLocation, nearSphereTraceDetectionRadius, UEngineTypes::ConvertToTraceType(traceChannel), true, actorsToIgnore, EDrawDebugTrace::None, hitResult, true);

	//far and thick multi sphere trace
	FVector traceFarStartLocation = traceShortEndLocation + me->wandLightArrowComponent->GetForwardVector() * farSphereTraceDetectionRadius;				//add with detection radius so that the trace doesn't start from the back of the camera
	FVector traceFarEndLocation = traceFarStartLocation + me->wandStaticMeshComponent->GetForwardVector() * farSphereTraceDistance;
	TArray<FHitResult> farSphereMultipleHitResults;
	bool isLongSphereTrace1Hit = UKismetSystemLibrary::SphereTraceMulti(this, traceFarStartLocation, traceFarEndLocation, farSphereTraceDetectionRadius, UEngineTypes::ConvertToTraceType(traceChannel), true, actorsToIgnore, EDrawDebugTrace::None, farSphereMultipleHitResults, true);

	//if short 
	if (isShortSphereTraceHit){
		AEnemy* enemyFromNearTrace = Cast<AEnemy>(hitResult.GetActor());
		if(enemyFromNearTrace){
	//		UE_LOG(LogTemp, Warning, TEXT("ShortSphereTraceHit"));
			comboImpactPoint = hitResult.ImpactPoint;
			comboImpactNormal = hitResult.ImpactNormal;
			return enemyFromNearTrace;
		}
	}

	else if (!isShortSphereTraceHit && isLongSphereTrace1Hit) {
		AEnemy* enemyFromFarTrace = Cast<AEnemy>(farSphereMultipleHitResults[0].GetActor());
	//	UE_LOG(LogTemp, Warning, TEXT("LongSphereTraceHit"));
		comboImpactPoint = farSphereMultipleHitResults[0].ImpactPoint;
		comboImpactNormal = farSphereMultipleHitResults[0].ImpactNormal;
		return enemyFromFarTrace;
	}

	return nullptr;
}


void ULegacyPlayerMagicComponent::CheckSpellActivation()
{
	//if in region and acceleration is high enough
	if (me->isInMagicRegion && (me->rightCurrentAccelerationMagnitude > accelerationThreshold) && me->rightCurrentVelocityMagnitude > velocityThreshold) {
	//if (me->isInMagicRegion && me->rightCurrentAccelerationDifferenceMagnitude > accelerationDiffernceThreshold) {

		//turn on light
		//wandLightNiagaraComponent->SetVisibility(true);
		isWandActive = true;
		//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CheckSpellActivation - Spell Activated Velocity %f"), me->rightCurrentVelocityMagnitude);
		//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CheckSpellActivation - Spell Activated Acceleration %f"), me->rightCurrentAccelerationMagnitude);
		//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CheckSpellActivation - Spell Activated Acceleration Difference %f"), me->rightCurrentAccelerationDifferenceMagnitude);
	}
	else if (me->isInMagicRegion && (me->rightCurrentAccelerationMagnitude < accelerationThreshold)) {
	//else if (me->isInMagicRegion && (me->rightCurrentAccelerationDifferenceMagnitude < accelerationDiffernceThreshold)) {
		isWandActive = false;
		/*UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CheckSpellActivation - In region - small Velocity %f"), me->rightCurrentVelocityMagnitude);
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CheckSpellActivation - In region - small Acceleration %f"), me->rightCurrentAccelerationMagnitude);
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CheckSpellActivation - In region - small Acceleration Difference%f"), me->rightCurrentAccelerationDifferenceMagnitude);*/
	}
	//if in region and acceleration is too low
	else if (!me->isInMagicRegion) {
		//UE_LOG(LogTemp, Warning, TEXT("Spell Reset"));
		//wandLightNiagaraComponent->SetVisibility(false);
		isWandActive = false;
		//UE_LOG(LogTemp, Error, TEXT("ULegacyPlayerMagicComponent::CheckSpellActivation - Spell Deactivated"));
	}

	if (wandLightNiagaraComponent && wandTrailsNiagaraComponent){
		if (me->rightCurrentAccelerationMagnitude > accelerationThreshold && me->rightCurrentVelocityMagnitude > velocityThreshold) {
			wandLightNiagaraComponent->SetVisibility(true);
			wandTrailsNiagaraComponent->SetVisibility(true);
		}
		else{
			wandLightNiagaraComponent->SetVisibility(false);
			wandTrailsNiagaraComponent->SetVisibility(false);
		}
	}
}

void ULegacyPlayerMagicComponent::UpdateSpellState()
{
	switch (spellState) {
		case SpellState::Rest:
			CheckSpellState(me->uIComponent->quadrantNumber);
			break;
		case SpellState::Levioso:
			CastLevioso();
			break;
		case SpellState::Accio:
			CastAccio();
			break;
		case SpellState::Depulso:
			CastDepulso();
			break;
		case SpellState::AvadaKedavra:
			CastAvadaKedavra();
			break;
		case SpellState::Ultimate:
			CastUltimate();
			break;
		case SpellState::Grab:
			CastGrab();
			break;
		case SpellState::Cancel:
			SpellCancel();
			break;
	}
}

void ULegacyPlayerMagicComponent::CheckSpellState(int32& quadrantNumber)
{

	//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CheckSpellState"));

	//potential bug: delete or keep
	//if (!isSpellCast) { return; }								//have to press spell cast to activate other spells

	//if on PC
	if (!me->legacyGameMode->isHMDActivated) {
		if (isLevioso && isSpellCast){
			spellState = SpellState::Levioso;
			//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CheckSpellState - SpellState::Levioso"));
		}
		else if (isAccio && isSpellCast){
			spellState = SpellState::Accio;
			//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CheckSpellState - SpellState::Accio"));
		}
		else if (isDepulso && isSpellCast){
			spellState = SpellState::Depulso;
			//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CheckSpellState - SpellState::Depulso"));
		}
		else if (isAvadaKedavra && isSpellCast){
			spellState = SpellState::AvadaKedavra;
			//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CheckSpellState - SpellState::Avada Kedavra"));
		}
		else if (isSpellUltimate){
			spellState = SpellState::Ultimate;
			//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CheckSpellState - SpellState::Ultimate"));
		}
		else if (isGrab){
			spellState = SpellState::Grab;
			//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CheckSpellState - SpellState::Grab"));
		}
		else if (isSpellCancel){										//for PC
			spellState = SpellState::Cancel;
			//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CheckSpellState - SpellState::Cancel"));
		}
	}
	else{
		if(quadrantNumber == 1 && isSpellCast){
			spellState = SpellState::Levioso;
			//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CheckSpellState - SpellState::Levioso"));
		}
		else if(quadrantNumber == 2 && isSpellCast){
			spellState = SpellState::Accio;
			//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CheckSpellState - SpellState::Accio"));
		}
		else if(quadrantNumber == 3 && isSpellCast){
			spellState = SpellState::Depulso;
			//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CheckSpellState - SpellState::Depulso"));
		}
		else if(quadrantNumber == 4 && isSpellCast){
			spellState = SpellState::AvadaKedavra;
			//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::OnActionCastSpellPressed - SpellState::AvadaKedavra"));
		}
		else if (isGrab){
			spellState = SpellState::Grab;
		}
	}
}






void ULegacyPlayerMagicComponent::UpdateWandEffects()
{
	//if there is no Wand Light Niagara Component
	if (!wandLightNiagaraComponent) {
		//spawn at the Wand Light Arrow
		wandLightNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(wandLightNiagaraSystem, me->wandLightArrowComponent, NAME_None, FVector(0), FRotator(0),
			EAttachLocation::KeepRelativeOffset, true, true, ENCPoolMethod::None, true);
		return;				//don't run the code below, or else nullptr
	}

	if(!wandTrailsNiagaraComponent){
		wandTrailsNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(wandTrailsNiagaraSystem, me->wandLightArrowComponent, NAME_None, FVector(0), FRotator(0),
			EAttachLocation::KeepRelativeOffset, true, true, ENCPoolMethod::None, true);
		return;
	}

	//update the position of the Niagara Wand Light
	wandLightNiagaraComponent->SetNiagaraVariableVec3(FString("EffectPosition"), me->wandLightArrowComponent->GetComponentLocation());
	wandTrailsNiagaraComponent->SetNiagaraVariableVec3(FString("Position"), me->wandLightArrowComponent->GetComponentLocation());
}

void ULegacyPlayerMagicComponent::CastLevioso()
{
	//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastLevioso"));

	//unflag other spell casts; this in in case it transitioned from another spell
	isAccio = false;
	isDepulso = false;

	//set physics handle settings
	me->physicsHandleComp->SetLinearDamping(5);
	me->physicsHandleComp->SetLinearStiffness(50);
	me->physicsHandleComp->SetInterpolationSpeed(60);

	//if detected a component from DetectTarget but haven't assigned grabbedComponent yet
	if (detectedComponent && !grabbedComponent){
		//make detected component the grabbed component
		grabbedComponent = detectedComponent;

		//cache object's initial height
		objectInitialHeight = grabbedComponent->GetComponentLocation();				//should be at wand's offset?

		//potential bug: is this needed? or is it already turned on in the enemy scripts
		grabbedComponent->SetSimulatePhysics(true);
		//grab the component with physics handle
		me->physicsHandleComp->GrabComponentAtLocation(grabbedComponent, NAME_None, grabbedComponent->GetComponentLocation());
		currentLocation = objectInitialHeight + objectOffsetHeight;

		enemy = Cast<AEnemy>(detectedComponent->GetOwner());
		if(enemy){
			enemy->enemyState->bIsGrabbed = true;
		//	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastLevioso - enemy grabbed"));
		}

		detectedComponent = nullptr;
	}
	else if (grabbedComponent){
		//Cancel Timer inside the grabbedComponent, so that it'll only run when grabbed object
		//set timer and check if levioso spell has expired; if so go to cancel state
		CancelSpellTimer(leviosoCancelTime);

		//Grab
		me->physicsHandleComp->SetTargetLocation(currentLocation);
		//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastLevioso - enemy set to levioso location"));
	}
	//if cast Levioso but it didn't have any target, go to Cancel state
	else if(!grabbedComponent){
		spellState = SpellState::Cancel; 
	}

	if(isSpellCombo){ CastGrabbedSpellCombo(); }
	
	//transition to another state
	if (isAccio || (me->uIComponent->quadrantNumber == 2 && isSpellCast)) { spellState = SpellState::Accio; }
	if (isDepulso || (me->uIComponent->quadrantNumber == 3 && isSpellCast)) { spellState = SpellState::Depulso; }
	if (isGrab) { spellState = SpellState::Grab; }

	//only for PC
	if(isSpellCancel){ spellState = SpellState::Cancel; }
}

void ULegacyPlayerMagicComponent::CastAccio()
{
	//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastAccio"));

	isLevioso = false;
	isDepulso = false;

	me->physicsHandleComp->SetLinearDamping(10);
	me->physicsHandleComp->SetLinearStiffness(50);
	me->physicsHandleComp->SetInterpolationSpeed(20);

	if (detectedComponent && !grabbedComponent) {
		//make detected component the grabbed component
		grabbedComponent = detectedComponent;

		//cache object's initial height
		objectInitialHeight = grabbedComponent->GetComponentLocation();				//should be at wand's offset?

		grabbedComponent->SetSimulatePhysics(true);

		//grab the component with physics handle
		me->physicsHandleComp->GrabComponentAtLocation(grabbedComponent, NAME_None, grabbedComponent->GetComponentLocation());

		enemy = Cast<AEnemy>(detectedComponent->GetOwner());
		if (enemy) {
			enemy->enemyState->bIsGrabbed = true;
			//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastAccio - Enemy Grabbed"));
		}

		//cache the accioHoverRegion
		accioHoverLocation = me->accioHoverRegionArrowComponent->GetComponentLocation();

		detectedComponent = nullptr;
	}
	else if (grabbedComponent) {
		//set timer and check if accio spell has expired; if so go to cancel state
		CancelSpellTimer(accioCancelTime);
		//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastAccio - set enemy in accio location"));
		me->physicsHandleComp->SetTargetLocation(accioHoverLocation);

		//constantly move grab object to the Accio Hover 
		//me->physicsHandleComp->SetTargetLocation(me->accioHoverRegionArrowComponent->GetComponentLocation());
	}

	//working progress
	if (isSpellCombo) {
		CastGrabbedSpellCombo();
	}

	//transition to another state
	if (isLevioso || (me->uIComponent->quadrantNumber == 1 && isSpellCast)) { spellState = SpellState::Levioso; }
	if (isDepulso || (me->uIComponent->quadrantNumber == 3 && isSpellCast)) { spellState = SpellState::Depulso; }
	if (isGrab) { spellState = SpellState::Grab; }

	//only for PC
	if (isSpellCancel) { spellState = SpellState::Cancel; }
}

//Combo spell when the something is grabbed
void ULegacyPlayerMagicComponent::CastGrabbedSpellCombo()
{

	if (!grabbedComponent){
		spellState = SpellState::Rest;
		return;
	}

	me->physicsHandleComp->SetInterpolationSpeed(100);


	FVector comboDirection = grabbedComponent->GetComponentLocation() - me->wandStaticMeshComponent->GetComponentLocation();
	comboDirection.Normalize();				//maybe already normalized

	if (comboCount < 5) {
		currentLocation += comboDirection * 100;
		me->physicsHandleComp->SetTargetLocation(currentLocation);
		isSpellCombo = false;
	}
	else {
		comboCount = 0;
		currentLocation += comboDirection * 300;
		me->physicsHandleComp->SetTargetLocation(currentLocation);
		spellState = SpellState::Cancel;
	}
}

//Not Really a Bug: Don't need??
void ULegacyPlayerMagicComponent::CastSpellCombo()
{
	//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastSpellCombo"));
}

void ULegacyPlayerMagicComponent::CastUltimate()
{
	CancelSpellTimer(2);
}

void ULegacyPlayerMagicComponent::CastDepulso()
{
	//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastDepulso"));

	isLevioso = false;
	isAccio = false;


	if (detectedComponent) {
		enemy = Cast<AEnemy>(detectedComponent->GetOwner());
	}
	else if(grabbedComponent){
		enemy = Cast<AEnemy>(grabbedComponent->GetOwner());
	}

	if (enemy) {
		if (grabbedComponent) { me->physicsHandleComp->ReleaseComponent(); }

		enemy->GetCapsuleComponent()->SetSimulatePhysics(true);

		GetWorld()->SpawnActor<AActor>(depulsoExplosionFactory, enemy->GetCapsuleComponent()->GetComponentLocation(), FRotator::ZeroRotator);

		FVector throwDirection = me->wandStaticMeshComponent->GetForwardVector();
		throwDirection.Normalize();
		//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastDepulso - Throw"));
		enemy->enemyState->Throw(throwDirection * 300000, 10);
		enemy->enemyState->bIsGrabbed = false;

		detectedComponent = nullptr;

		//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastDepulso - Go to Cancel"));
	}

	isDepulso = false;
	spellState = SpellState::Cancel;
}

void ULegacyPlayerMagicComponent::CastAvadaKedavra()
{
	//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastAvadaKedavra"));


	if(!avadaKedavraNiagaraComponent){
		//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastAvadaKedavra - !isAvadaKedavra"));
		avadaKedavraNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(avadaKedavraNiagaraSystem, me->wandStaticMeshComponent, NAME_None, FVector(0), FRotator(0), 
			EAttachLocation::KeepRelativeOffset, true, true, ENCPoolMethod::None, true);
	}

	////need to do line trace
	//FVector startPosition = me->wandLightArrowComponent->GetComponentLocation();
	//FVector endPosition;
	//FVector impactNormal;
	//FHitResult hitResult;
	//FCollisionQueryParams params;
	//params.AddIgnoredActor(me);

	//bool isHit = GetWorld()->LineTraceSingleByChannel(hitResult, startPosition, startPosition + me->wandLightArrowComponent->GetComponentLocation() * 1000000, ECollisionChannel::ECC_Visibility, params);

	AEnemy* wideSphereTraceResult = WideSphereTrace();

	//if hits a surface, then control the startpoint and endpoint of the beam
	if (wideSphereTraceResult) {
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastAvadaKedavra - isHit"));

		//endPosition = hitResult.ImpactPoint;
		enemy = Cast<AEnemy>(wideSphereTraceResult);
		if(enemy){
			UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastAvadaKedavra - enemy"))
			FVector startPosition = me->wandLightArrowComponent->GetComponentLocation();
			FVector endPosition = enemy->GetCapsuleComponent()->GetComponentLocation();
			//impactNormal = hitResult.ImpactNormal;					//not used
			if(avadaKedavraNiagaraComponent){
				//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastAvadaKedavra - avadaKedavraNiagaraComponent"))
				avadaKedavraNiagaraComponent->SetNiagaraVariableVec3(FString("beamStartPoint"), startPosition);
				avadaKedavraNiagaraComponent->SetNiagaraVariableVec3(FString("beamEndPoint"), endPosition);
				//avadaKedavraNiagaraComponent->SetNiagaraVariableVec3(FString("impactNormal"), impactNormal);
			}

			enemy->enemyState->OnDamageProcess(100);
		}
	}

	//if niagara is done and there is still , then go to cancel
	if((avadaKedavraNiagaraComponent) && avadaKedavraNiagaraComponent->IsComplete()){
	//	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastAvadaKedavra - Niagara Complete"));
	//	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastAvadaKedavra - Go to Cancel"));
		spellState = SpellState::Cancel;
	}
}

void ULegacyPlayerMagicComponent::CastGrab()
{
	//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastGrab"));

	isLevioso = false;
	isAccio = false;
	isDepulso = false;

	//change wand light
	if (wandLightNiagaraComponent){
		wandLightNiagaraComponent->SetNiagaraVariableLinearColor(FString("Color"), FLinearColor::White);
	}


	me->physicsHandleComp->SetLinearDamping(5);
	me->physicsHandleComp->SetLinearStiffness(50);
	me->physicsHandleComp->SetInterpolationSpeed(60);

	//bug:: need to reset grabbed object in rest state? why??

	if(detectedComponent && !grabbedComponent){
		//make detected component the grabbed component
		grabbedComponent = detectedComponent;
		// since updated grabbedComponent component, dereference detectedComponent for the next detection
		detectedComponent = nullptr;

		//cache object's initial height
		objectInitialHeight = grabbedComponent->GetComponentLocation();				//should be at wand's offset?

		grabbedComponent->SetSimulatePhysics(true);

		//grab the component with physics handle
		me->physicsHandleComp->GrabComponentAtLocation(grabbedComponent, NAME_None, grabbedComponent->GetComponentLocation());
		if(enemy){
			enemy->enemyState->bIsGrabbed = true;
		}
	}
	else if(grabbedComponent){
		//me->physicsHandleComp->SetTargetLocation(objectInitialHeight + objectOffsetHeight);
		me->physicsHandleComp->SetTargetLocation(me->grabHoverRegionArrowComponent->GetComponentLocation());
	}

	if ((isLevioso || me->uIComponent->quadrantNumber == 1)&&isSpellCast) { spellState = SpellState::Levioso; }
	if ((isAccio || me->uIComponent->quadrantNumber == 2) && isSpellCast) { spellState = SpellState::Accio; }
	if ((isDepulso || me->uIComponent->quadrantNumber == 3) && isSpellCast) { spellState = SpellState::Depulso; }

	//Grab
	if(!isGrab || isSpellCancel){
		if(enemy){
			enemy->enemyState->bIsGrabbed = false;
			enemy->GetCapsuleComponent()->SetSimulatePhysics(true);
		}
	//	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastGrab - Go to Cancel"));
		spellState = SpellState::Cancel;
	}
}

void ULegacyPlayerMagicComponent::DetectTarget()
{
	//if the player casts a spell on a detected component, return
	//bug; if grabbed then cant do other spells
	//try deleting
	//if (grabbedComponent) { return; }

	FVector wandHandPosition = me->wandStaticMeshComponent->GetComponentLocation();
	FVector traceStartLocation = wandHandPosition + me->wandStaticMeshComponent->GetForwardVector() * detectionRadius;				//add with detection radius so that the trace doesn't start from the back of the camera
	FVector traceEndLocation = traceStartLocation + me->wandStaticMeshComponent->GetForwardVector() * 100000;

	TEnumAsByte<ECollisionChannel> traceChannel;
	traceChannel = ECollisionChannel::ECC_Visibility;
	TArray<AActor*> actorsToIgnore;

	FHitResult hitResult;

	//do a sphere trace
	bool isHit = UKismetSystemLibrary::SphereTraceSingle(this, traceStartLocation, traceEndLocation, detectionRadius, UEngineTypes::ConvertToTraceType(traceChannel), true, actorsToIgnore, EDrawDebugTrace::None, hitResult, true);

	//if it hits something
	if (isHit) {

		//potential bug; constantly updating enemy, so it could end up something else; can be nullptr once you point at something else
		//cast that enemy to the actor
		enemy = Cast<AEnemy>(hitResult.GetActor());

		//if succeeded in casting; if the actor is indeed an enemy class
		if(enemy){
			#pragma region Debug
			//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::DetectTarget - found enemy"));
			#pragma endregion 
			detectedComponent = hitResult.GetComponent();
			//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::DetectTarget - Get Enemy Capsule"));
			#pragma region Debug
			/*if (detectedComponent) {
				UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::DetectTarget - detectedComponent"));
			}*/
			#pragma endregion
		}
		#pragma region Debug
		else{
			//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::DetectTarget - can't find enemy"));
		}
		#pragma endregion
	}
}

void ULegacyPlayerMagicComponent::CancelSpellTimer(float spellTime)
{
	currentTime += GetWorld()->DeltaTimeSeconds;

	//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CancelSpellTimer - currentTime %f: "), currentTime);

	if(currentTime > spellTime){
		currentTime = 0;
	//	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CancelSpellTimer - Go to Cancel"));
		spellState = SpellState::Cancel;
	}
}




void ULegacyPlayerMagicComponent::DereferenceVariables()
{
	//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::DereferenceVariables"));
	isSpellCast = false;						//don't need this
	isAccio = false;
	isLevioso = false;
	isDepulso = false;
	isAvadaKedavra = false;
	isSpellUltimate = false;
	isSpellCombo = false;
	isSpellCancel = false;


	//AvadaKedavra
	avadaKedavraNiagaraComponent = nullptr;

	//Combo
	comboCount = 0;

	me->uIComponent->quadrantNumber = 0;



	if (!grabbedComponent) { return; }

	enemy = Cast<AEnemy>(grabbedComponent->GetOwner());;
	if(enemy){
		enemy->enemyState->bIsGrabbed = false;
		enemy->GetCapsuleComponent()->SetSimulatePhysics(true);
		enemy = nullptr;
	}

	//Bug: might need to take this to rest
	me->physicsHandleComp->ReleaseComponent();

	//dereference grabbedComponent
	grabbedComponent = nullptr;
}

void ULegacyPlayerMagicComponent::SpellCancel()
{
//	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::SpellCancel"));

	DereferenceVariables();
	spellState = SpellState::Rest;
}

