#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawn11.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEnemySpawnDelegate);

UCLASS()
class GHOSTBURSTER_API AEnemySpawn11 : public AActor
{
    GENERATED_BODY()

public:
    AEnemySpawn11();

    UPROPERTY(BlueprintAssignable, Category = "EnemySpawn1")
    FEnemySpawnDelegate OnEnemySpawned;

protected:
    virtual void BeginPlay() override;

    void HandleSpawnEvent();

private:
    UFUNCTION()
    void HandleEnemySpawn();

    void SpawnEnemies(FVector SpawnLocation, FRotator SpawnRotation, int32 NumEnemies);
    void HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};