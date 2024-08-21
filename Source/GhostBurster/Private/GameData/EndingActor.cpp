#include "GameData/EndingActor.h"
#include "GameData/MyGameInstance.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void AEndingActor::BeginPlay()
{
    Super::BeginPlay();

    // GameInstance����f�[�^���擾
    UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
    if (GameInstance)
    {
        // UI�v�f�Ƀf�[�^�𔽉f
        if (EscapeTimeText)
        {
            EscapeTimeText->SetText(FText::AsNumber(GameInstance->EscapeTime));
        }

        if (GhostsDefeatedText)
        {
            GhostsDefeatedText->SetText(FText::AsNumber(GameInstance->GhostsDefeated));
        }

        if (TotalScoreText)
        {
            TotalScoreText->SetText(FText::FromString(GameInstance->TotalScore));
        }
    }
}
