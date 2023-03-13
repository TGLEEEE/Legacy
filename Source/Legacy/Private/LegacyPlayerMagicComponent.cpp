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

#include "MotionControllerComponent.h"
#include "Camera/CameraComponent.h"


void ULegacyPlayerMagicComponent::BeginPlay()
{
	Super::BeginPlay();

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

		inputSystem->BindAction(me->iA_Grab, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionGrabPressed);
		inputSystem->BindAction(me->iA_Grab, ETriggerEvent::Completed, this, &ULegacyPlayerMagicComponent::OnActionGrabReleased);

		inputSystem->BindAction(me->iA_Spell1, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionSpell1Pressed);
		inputSystem->BindAction(me->iA_Spell2, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionSpell2Pressed);
		inputSystem->BindAction(me->iA_Spell3, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionSpell3Pressed);

		//temporary
		inputSystem->BindAction(me->iA_SpellCombo, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionSpellComboPressed);
		inputSystem->BindAction(me->iA_SpellCancel, ETriggerEvent::Triggered, this, &ULegacyPlayerMagicComponent::OnActionSpellCancelPressed);
	}
#pragma endregion

}

#pragma region Input Action
void ULegacyPlayerMagicComponent::OnActionCastSpellPressed()
{
	isSpellCast = true;
	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::OnActionCastSpellPressed - isSpellCast"));
}


void ULegacyPlayerMagicComponent::OnActionGrabPressed()
{
	isGrab = true;
	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::OnActionGrabPressed - isGrab"));
}

void ULegacyPlayerMagicComponent::OnActionGrabReleased()
{
	isGrab = false;
	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::OnActionGrabReleased - !isGrab"));
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
		/*case SpellState::AvadaKedavra:
			CastAvadaKedavra();
			break;*/
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

	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CheckSpellState"));


	if (!isSpellCast) { return; }								//have to press spell cast to activate other spells

	//if on PC
	if (!UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled()) {
		if (isLevioso) { spellState = SpellState::Levioso; }
		else if (isAccio) { spellState = SpellState::Accio; }
		else if (isDepulso) { spellState = SpellState::Depulso; }
		else if (isGrab) { spellState = SpellState::Grab; }
		else if (isSpellCancel) { spellState = SpellState::Cancel; }
	}
	else{
		if(quadrantNumber == 1 && isSpellCast){ spellState = SpellState::Levioso; UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::OnActionCastSpellPressed - SpellState::Levioso"));}
		else if(quadrantNumber == 2 && isSpellCast){ spellState = SpellState::Accio; UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::OnActionCastSpellPressed - SpellState::Accio"));}
		else if(quadrantNumber == 3 && isSpellCast){ spellState = SpellState::Depulso; UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::OnActionCastSpellPressed - SpellState::Depulso"));}
		//else if(quadrantNumber == 4 && isSpellCast){ spellstate = SpellState::AvadaKedavra; UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::OnActionCastSpellPressed - SpellState::AvadaKedavra"));}
		else if (isGrab) { spellState = SpellState::Grab; }
	}
}


void ULegacyPlayerMagicComponent::CastLevioso()
{
	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastLevioso"));

	isAccio = false;
	isDepulso = false;

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
			enemy->enemyState->bIsGrabbed = true;
			UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastLevioso - enemy grabbed"));
		}
	}
	else if (grabbedComponent){
		grabbedComponent->SetSimulatePhysics(true);
		me->physicsHandleComp->SetTargetLocation(currentLocation);
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastLevioso - enemy set to levioso location"));
	}

	//set timer and check if levioso spell has expired; if so go to cancel state
	CancelSpellTimer(leviosoCancelTime);

	

	if(isSpellCombo){ SpellCombo(); }
	
	//transition to another state
	if (isGrab) { spellState = SpellState::Grab; }
	if (isAccio) { spellState = SpellState::Accio; }
	if (isDepulso) { spellState = SpellState::Depulso; }

	//only for PC
	if(isSpellCancel){ spellState = SpellState::Cancel; }
}



void ULegacyPlayerMagicComponent::CastAccio()
{
	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastAccio"));


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

		//grab the component with physics handle
		me->physicsHandleComp->GrabComponentAtLocation(grabbedComponent, NAME_None, grabbedComponent->GetComponentLocation());

		if (enemy) {
			enemy->enemyState->bIsGrabbed = true;
			UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastAccio - Grab Enemy"));
		}
	}
	else if (grabbedComponent) {
		grabbedComponent->SetSimulatePhysics(true);

		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastAccio - set enemy in accio location"));
		me->physicsHandleComp->SetTargetLocation(me->accioHoverRegionArrowComponent->GetComponentLocation());
	}

	//set timer and check if accio spell has expired; if so go to cancel state
	CancelSpellTimer(accioCancelTime);

	//working progress
	if (isSpellCombo) {
		SpellCombo();
	}

	//transition to another state
	if (isDepulso) { spellState = SpellState::Depulso; }
	if (isGrab) { spellState = SpellState::Grab; }

	//only for PC
	if (isSpellCancel) { spellState = SpellState::Cancel; }
}


void ULegacyPlayerMagicComponent::SpellCombo()
{

	if (!enemy) { return; }

	me->physicsHandleComp->SetInterpolationSpeed(100);

	if (!grabbedComponent) { spellState = SpellState::Rest; return; }

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
		spellState = SpellState::Cancel;
	}
}



void ULegacyPlayerMagicComponent::CastDepulso()
{

	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastDepulso"));

	isLevioso = false;
	isAccio = false;

	if (!enemy) { return; }

	enemy = Cast<AEnemy>(detectedComponent->GetOwner());
	if(enemy){
		if(grabbedComponent){ me->physicsHandleComp->ReleaseComponent();  }

		FVector throwDirection = enemy->GetActorLocation() - me->GetActorForwardVector();
		throwDirection.Normalize();
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastDepulso - Throw"));
		enemy->enemyState->Throw(throwDirection * 300000, 1);
		enemy->enemyState->bIsGrabbed = false;

		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastDepulso - Go to Cancel"));
		enemy->GetCapsuleComponent()->SetSimulatePhysics(true);
		spellState = SpellState::Cancel;
	}
}

void ULegacyPlayerMagicComponent::CastAvadaKedavra()
{
	if(!isAvadaKedavraCast){
		avadaKedavraNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(avadaKedavraNiagara, me->wandStaticMeshComponent, NAME_None, FVector(0), FRotator(0), 
			EAttachLocation::KeepRelativeOffset, true, true, ENCPoolMethod::None, true);
		isAvadaKedavraCast = true;
	}

	//need to do line trace
	FVector startPosition = me->wandStaticMeshComponent->GetComponentLocation();
	FVector endPosition;
	FVector impactNormal;
	FHitResult hitResult;
	FCollisionQueryParams params;
	params.AddIgnoredActor(me);

	bool isHit = GetWorld()->LineTraceSingleByChannel(hitResult, startPosition, startPosition + me->wandStaticMeshComponent->GetForwardVector() * 1000000, ECollisionChannel::ECC_Visibility, params);

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
	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastGrab"));

	isLevioso = false;
	isAccio = false;
	isDepulso = false;

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
			enemy->enemyState->bIsGrabbed = false;
			enemy->GetCapsuleComponent()->SetSimulatePhysics(true);
		}
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CastGrab - Go to Cancel"));
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
	bool isHit = UKismetSystemLibrary::SphereTraceSingle(this, traceStartLocation, traceEndLocation, detectionRadius, UEngineTypes::ConvertToTraceType(traceChannel), true, actorsToIgnore, EDrawDebugTrace::ForOneFrame, hitResult, true);

	//if it hits something
	if (isHit) {
		//cast that enemy to the actor
		enemy = Cast<AEnemy>(hitResult.GetActor());

		//if succeeded in casting; if the actor is indeed an enemy class
		if(enemy){
			#pragma region Debug
			//UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::DetectTarget - found enemy"));
			#pragma endregion 
			//detectedComponent = hitResult.GetComponent();
			//get the enemy's capsule component and cache it into detected component
			detectedComponent = enemy->GetCapsuleComponent();
			UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::DetectTarget - Get Enemy Capsule"));
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
	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::DereferenceVariables"));
	isAccio = false;
	isLevioso = false;
	isDepulso = false;
	isSpellCombo = false;
	isSpellCancel = false;
	isSpellCast = false;

	comboCount = 0;
	if(enemy){
		enemy->enemyState->bIsGrabbed = false;
		enemy = nullptr;
	}

	//Bug: might need to take this to rest
	me->physicsHandleComp->ReleaseComponent();

	//dereference grabbedComponent
	grabbedComponent = nullptr;
}

void ULegacyPlayerMagicComponent::CancelSpellTimer(float spellTime)
{
	currentTime += GetWorld()->DeltaTimeSeconds;

	if(currentTime > spellTime){
		currentTime = 0;
		UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::CancelSpellTimer - Go to Cancel"));
		spellState = SpellState::Cancel;
	}
}


void ULegacyPlayerMagicComponent::SpellCancel()
{
	UE_LOG(LogTemp, Warning, TEXT("ULegacyPlayerMagicComponent::SpellCancel"));

	DereferenceVariables();

	spellState = SpellState::Rest;
}

