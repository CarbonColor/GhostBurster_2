#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextBlock.h"
#include "EndingActor.generated.h"

UCLASS()
class GHOSTBURSTER_API AEndingActor : public AActor
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

protected:
    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (BindWidget))
    UTextBlock* EscapeTimeText;

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (BindWidget))
    UTextBlock* GhostsDefeatedText;

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (BindWidget))
    UTextBlock* TotalScoreText;
};
