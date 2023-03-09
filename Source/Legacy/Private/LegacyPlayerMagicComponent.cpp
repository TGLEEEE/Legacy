// Fill out your copyright notice in the Description page of Project Settings.


#include "LegacyPlayerMagicComponent.h"
#include "EnhancedInputComponent.h"
#include "LegacyPlayer.h"

#include "Camera/CameraComponent.h"
#include "Enemy.h"
#include "MotionControllerComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EnemyFSM.h"
#include "EnemyState.h"
#include "Niagara/Public/NiagaraFunctionLibrary.h"
#include "Niagara/Public/NiagaraComponent.h"

void ULegacyPlayerMagicComponent::BeginPlay()
{
	Super::BeginPlay();

}

void ULegacyPlayerMagicComponent::SetupPlayerInput(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInput(PlayerInputComponent);

	auto inputSystem = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (inputSystem) {
		inputSystem->BindAction(me->iA_CastSpell, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionCastSpell);

		inputSystem->BindAction(me->iA_Grab, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionGrabPressed);
		inputSystem->BindAction(me->iA_Grab, ETriggerEvent::Completed, this, &ULegacyPlayerMagicComponent::OnActionGrabReleased);

		inputSystem->BindAction(me->iA_Spell1, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionSpell1Pressed);
		inputSystem->BindAction(me->iA_Spell2, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionSpell2Pressed);
		inputSystem->BindAction(me->iA_Spell3, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionSpell3Pressed);

		inputSystem->BindAction(me->iA_SpellCombo, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionSpellComboPressed);

		inputSystem->BindAction(me->iA_SpellCancel, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionSpellCancelPressed);
	}
}

#pragma region Input Action
void ULegacyPlayerMagicComponent::OnActionCastSpell()
{
	isSpellCast = true;
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

void ULegacyPlayerMagicComponent::OnActionSpellComboPressed()
{
	isSpellCombo = true;
	comboCount++;
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
	CastAvadaKedavra();

}


void ULegacyPlayerMagicComponent::UpdateSpellState()
{
	switch (spellstate) {
		case SpellState::Rest:
			CheckSpellState();
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
		case SpellState::Grab:
			CastGrab();
			break;
		case SpellState::Cancel:
			SpellCancel();
			break;
	}
}

void ULegacyPlayerMagicComponent::CheckSpellState()
{
	if (!isSpellCast) { return; }								//have to press spell cast to activate other spells

	if (isLevioso) { spellstate = SpellState::Levioso; }
	else if (isAccio) { spellstate = SpellState::Accio; }
	else if (isDepulso) { spellstate = SpellState::Depulso; }
	else if (isGrab) { spellstate = SpellState::Grab; }
	else if (isSpellCancel) { spellstate = SpellState::Cancel; }
}


void ULegacyPlayerMagicComponent::CastLevioso()
{

	me->physicsHandleComp->SetLinearDamping(5);
	me->physicsHandleComp->SetLinearStiffness(50);
	me->physicsHandleComp->SetInterpolationSpeed(60);

	if (detectedComponent && !grabbedComponent){
		//make detected component the grabbed component
		grabbedComponent = detectedComponent;

		//cache object's initial height
		objectInitialHeight = grabbedComponent->GetComponentLocation();				//should be at wand's offset?

		//grab the component with physics handle
		me->physicsHandleComp->GrabComponentAtLocation(grabbedComponent, NAME_None, grabbedComponent->GetComponentLocation());
		currentLocation = objectInitialHeight + objectOffsetHeight;
		if(enemy){
			enemy->enemyFSM->bIsInTheAir = true;
			enemy->enemyState->bIsGrabbed = true;
			//enemy->enemyFSM->SetState(EEnemyState::INTHEAIR);
		}
	}
	else if (grabbedComponent){
		grabbedComponent->SetSimulatePhysics(true);
		me->physicsHandleComp->SetTargetLocation(currentLocation);
	}

	if(isSpellCombo){ SpellCombo(); }
	
	//transition to another state
	if (isGrab) { spellstate = SpellState::Grab; }
	if (isAccio) { spellstate = SpellState::Accio; }
	if (isDepulso) { spellstate = SpellState::Depulso; }
	if(isSpellCancel){ spellstate = SpellState::Cancel; }
}



void ULegacyPlayerMagicComponent::CastAccio()
{

	me->physicsHandleComp->SetLinearDamping(10);
	me->physicsHandleComp->SetLinearStiffness(50);
	me->physicsHandleComp->SetInterpolationSpeed(20);

	if (detectedComponent && !grabbedComponent) {
		//make detected component the grabbed component
		grabbedComponent = detectedComponent;

		//cache object's initial height
		objectInitialHeight = grabbedComponent->GetComponentLocation();				//should be at wand's offset?

		//grab the component with physics handle
		me->physicsHandleComp->GrabComponentAtLocation(grabbedComponent, NAME_None, grabbedComponent->GetComponentLocation());

		if (enemy) {
			enemy->enemyFSM->bIsInTheAir = true;
			enemy->enemyState->bIsGrabbed = true;
			//enemy->enemyFSM->SetState(EEnemyState::INTHEAIR);
		}
	}
	else if (grabbedComponent) {
		grabbedComponent->SetSimulatePhysics(true);

		me->physicsHandleComp->SetTargetLocation(me->accioHoverRegionArrowComponent->GetComponentLocation());
	}

	if (isSpellCombo) {
		SpellCombo();
	}

	//transition to another state
	if (isDepulso) { spellstate = SpellState::Depulso; }
	if (isGrab) { spellstate = SpellState::Grab; }
	if (isSpellCancel) { spellstate = SpellState::Cancel; }

}


void ULegacyPlayerMagicComponent::SpellCombo()
{

	if (!enemy) { return; }
	enemy->enemyFSM->bIsInTheAir = true;
	//enemy->enemyFSM->SetState(EEnemyState::INTHEAIR);

	me->physicsHandleComp->SetInterpolationSpeed(100);

	if (!grabbedComponent) { spellstate = SpellState::Rest; return; }

	FVector comboDirection = me->GetActorForwardVector();
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
		spellstate = SpellState::Cancel;
	}
}



void ULegacyPlayerMagicComponent::CastDepulso()
{
	if (!enemy) { return; }
	enemy->enemyFSM->bIsInTheAir = true;
	///enemy->enemyFSM->SetState(EEnemyState::INTHEAIR);

	enemy = Cast<AEnemy>(detectedComponent->GetOwner());
	if(enemy){
		if(grabbedComponent){ me->physicsHandleComp->ReleaseComponent();  }


		FVector throwDirection = enemy->GetActorLocation() - me->GetActorForwardVector();
		throwDirection.Normalize();
		enemy->enemyState->Throw(throwDirection * 300000, 1);
		//enemy->enemyFSM->SetState(EEnemyState::IDLE);
		enemy->enemyFSM->bIsInTheAir = false;
		enemy->enemyState->bIsGrabbed = false;


		enemy->GetCapsuleComponent()->SetSimulatePhysics(true);
		
		isDepulso = false;
	}
}

void ULegacyPlayerMagicComponent::CastAvadaKedavra()
{
	if(!isAvadaKedavraCast){
		avadaKedavraNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(avadaKedavraNiagara, me->staticMeshCompWand, NAME_None, FVector(0), FRotator(0), 
			EAttachLocation::KeepRelativeOffset, true, true, ENCPoolMethod::None, true);
		isAvadaKedavraCast = true;
	}

	//need to do line trace
	FVector startPosition = me->staticMeshCompWand->GetComponentLocation();
	FVector endPosition;
	FVector impactNormal;
	FHitResult hitResult;
	FCollisionQueryParams params;
	params.AddIgnoredActor(me);

	bool isHit = GetWorld()->LineTraceSingleByChannel(hitResult, startPosition, startPosition + me->staticMeshCompWand->GetForwardVector() * 1000000, ECollisionChannel::ECC_Visibility, params);

	if (isHit) {
		endPosition = hitResult.ImpactPoint;
		impactNormal = hitResult.ImpactNormal;					//not used
		if(avadaKedavraNiagaraComponent){
			avadaKedavraNiagaraComponent->SetNiagaraVariableVec3(FString("beamStartPoint"), startPosition);
			avadaKedavraNiagaraComponent->SetNiagaraVariableVec3(FString("beamEndPoint"), endPosition);
			avadaKedavraNiagaraComponent->SetNiagaraVariableVec3(FString("impactNormal"), impactNormal);
		}
	}

	////bug: fill this in
	//if(enemy){
	//}
}


void ULegacyPlayerMagicComponent::CastGrab()
{
	me->physicsHandleComp->SetLinearDamping(5);
	me->physicsHandleComp->SetLinearStiffness(50);
	me->physicsHandleComp->SetInterpolationSpeed(60);

	//bug:: need to reset grabbed object in rest state?

	if(detectedComponent && !grabbedComponent){
		//make detected component the grabbed component
		grabbedComponent = detectedComponent;
		//turn on physics
		grabbedComponent->SetSimulatePhysics(true);
		// since updated grabbedComponent component, dereference detectedComponent for the next detection
		detectedComponent = nullptr;

		//cache object's initial height
		objectInitialHeight = grabbedComponent->GetComponentLocation();				//should be at wand's offset?

		//grab the component with physics handle
		me->physicsHandleComp->GrabComponentAtLocation(grabbedComponent, NAME_None, grabbedComponent->GetComponentLocation());
		if(enemy){
			enemy->enemyFSM->bIsInTheAir = true;
			enemy->enemyState->bIsGrabbed = true;
		}

	}
	else if(grabbedComponent){
		//me->physicsHandleComp->SetTargetLocation(objectInitialHeight + objectOffsetHeight);
		me->physicsHandleComp->SetTargetLocation(me->grabHoverRegionArrowComponent->GetComponentLocation());
	}

	//Grab
	if(!isGrab || isSpellCancel){
		if(enemy){
			enemy->enemyFSM->bIsInTheAir = false;
			enemy->enemyState->bIsGrabbed = false;
			enemy->GetCapsuleComponent()->SetSimulatePhysics(true);
		}
		
		spellstate = SpellState::Cancel;
	}
}



void ULegacyPlayerMagicComponent::DetectTarget()
{
	//if the player casts a spell on a detected component, return
	if (grabbedComponent) { return; }

	FVector wandHandPosition = me->staticMeshCompWand->GetComponentLocation();
	FVector traceStartLocation = wandHandPosition + me->staticMeshCompWand->GetForwardVector() * detectionRadius;				//add with detection radius so that the trace doesn't start from the back of the camera
	FVector traceEndLocation = traceStartLocation + me->staticMeshCompWand->GetForwardVector() * 100000;

	TEnumAsByte<ECollisionChannel> traceChannel;
	traceChannel = ECollisionChannel::ECC_Visibility;
	TArray<AActor*> actorsToIgnore;

	FHitResult hitResult;

	//do a sphere trace
	bool isHit = UKismetSystemLibrary::SphereTraceSingle(this, traceStartLocation, traceEndLocation, detectionRadius, UEngineTypes::ConvertToTraceType(traceChannel), true, actorsToIgnore, EDrawDebugTrace::ForOneFrame, hitResult, true);

	//if it hits something
	if (isHit) {
		enemy = Cast<AEnemy>(hitResult.GetActor());

		//bug: temporary
		detectedComponent = hitResult.GetComponent();

		if(enemy){
			#pragma region Debug
			//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::DetectTarget - found enemy"));
			#pragma endregion 
			detectedComponent = hitResult.GetComponent();
			#pragma region Debug
			if (detectedComponent) {
				UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::DetectTarget - grabbedComponent"));
			}
			#pragma endregion
		}
		#pragma region Debug
		else{
			//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::DetectTarget - can't find enemy"));
		}
		#pragma endregion
	}
}

void ULegacyPlayerMagicComponent::DereferenceVariables()
{
	//Bug: might need to take this to rest
	me->physicsHandleComp->ReleaseComponent();

	isAccio = false;
	isLevioso = false;
	isDepulso = false;
	isGrab = false;
	isSpellCombo = false;
	isSpellCancel = false;

	comboCount = 0;
	if(enemy){
		enemy->enemyFSM->bIsInTheAir = false;
		enemy->enemyState->bIsGrabbed = false;

		//enemy->enemyFSM->SetState(EEnemyState::IDLE);
		enemy = nullptr;
	}

	////turn off physics
	//grabbedComponent->SetSimulatePhysics(false);

	//dereference grabbedComponent
	grabbedComponent = nullptr;
}




void ULegacyPlayerMagicComponent::SpellCancel()
{

	DereferenceVariables();

	spellstate = SpellState::Rest;
}

