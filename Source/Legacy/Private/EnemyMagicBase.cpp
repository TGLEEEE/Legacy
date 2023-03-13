// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyMagicBase.h"
#include "NiagaraComponent.h"
#include <Components/BoxComponent.h>
#include "LegacyPlayer.h"

// Sets default values
AEnemyMagicBase::AEnemyMagicBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	SetRootComponent(boxComp);

	mainFXComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Main FX"));
	mainFXComp->SetupAttachment(RootComponent);

	subFXComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Sub FX"));
	subFXComp->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AEnemyMagicBase::BeginPlay()
{
	Super::BeginPlay();
	boxComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemyMagicBase::OnOverlap);
	player = Cast<ALegacyPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	startLoc = GetActorLocation();
	target = player->GetActorLocation();
}

// Called every frame
void AEnemyMagicBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	alpha += DeltaTime * magicSpeed;
	SetActorLocation(FMath::Lerp(startLoc, target, alpha));

	if (alpha > 2)
	{
		Destroy();
	}
}

void AEnemyMagicBase::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Destroy();
}

