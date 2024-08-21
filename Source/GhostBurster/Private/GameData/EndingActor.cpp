#include "GameData/EndingActor.h"
#include "GameData/MyGameInstance.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void AEndingActor::BeginPlay()
{
    Super::BeginPlay();

    // GameInstanceからデータを取得
    UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
    if (GameInstance)
    {
        // UI要素にデータを反映
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
