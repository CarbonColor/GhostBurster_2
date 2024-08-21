#include "MyGameModeBase.h"
#include "GameData/MyGameInstance.h"

void AMyGameModeBase::StartPlay()
{
    Super::StartPlay();

    // ゲーム開始時にデータをリセット
    ResetGameData();
}

void AMyGameModeBase::ResetGameData()
{
    UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
    if (GameInstance)
    {
        GameInstance->ResetGameData();
    }
}

void AMyGameModeBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
    if (GameInstance)
    {
        GameInstance->EscapeTime += DeltaTime;
    }
}
