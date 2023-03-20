// Fill out your copyright notice in the Description page of Project Settings.


#include "TimerWidget.h"

#include "LegacyGameMode.h"
#include "LegacyPlayer.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 게임모드 캐스팅
	gm = Cast<ALegacyGameMode>(GetWorld()->GetAuthGameMode());
	// 0.2초마다 업데이트 되게 타이머 세팅
	GetWorld()->GetTimerManager().SetTimer(hdForTimerWidget, FTimerDelegate::CreateLambda([&]()
		{
			UpdateTimerWidget();
		}), 0.2f, true);
	// 플레이어 체력 읽어오기 위해 캐스팅
	player = Cast<ALegacyPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter());
}

void UTimerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (gm->currentWave == 2 && !bDoOnce)
	{
		PlayAnimation(waveAnim);
		bDoOnce = true;
	}
	else if (gm->currentWave == 3 && bDoOnce)
	{
		PlayAnimation(waveAnim);
		bDoOnce = false;
	}

	if(gm->currentWave > 3 && !bClearOnce)
	{
		PlayAnimation(clearAnim);
		bClearOnce = true;
	}

	// 플레이어 죽으면 게임오버 출력되게 애님플레이
	if (player->currentHealth <= 0 && !bDieOnce)
	{
		bDieOnce = true;
		PlayAnimation(dieAnim);
		// 플레이어 죽었을때 게임 정지
		FTimerHandle hd;
		GetWorld()->GetTimerManager().SetTimer(hd, FTimerDelegate::CreateLambda([&]()
			{
				UGameplayStatics::SetGamePaused(GetWorld(), true);
			}), 2.f, false);
	}
}

void UTimerWidget::UpdateTimerWidget()
{
	WorldtimeMin->SetText(FText::AsNumber(gm->worldTimeMin));
	WorldtimeSec->SetText(FText::AsNumber(gm->worldTimeSec));
	EnemyCount->SetText(FText::AsNumber(gm->enemyKillCount));
	EnemyCountTotal->SetText(FText::AsNumber(gm->enemyCountTotal));
	WaveTextbox->SetText(FText::FromString(FString("WAVE ") + FString::FromInt(gm->currentWave)));
}
