#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CEButtonDispatchBomb.generated.h"

UENUM()
enum class ECEButtonDispatchBombStaticMesh
{
    Button
};

UCLASS()
class CONTENTEXAMPLESCPP_API ACEButtonDispatchBomb : public AActor
{
    GENERATED_BODY()

    UPROPERTY()
    USceneComponent* Base = nullptr;

    UPROPERTY()
    class UCapsuleComponent* TriggerCollision = nullptr;

    UPROPERTY()
    UStaticMeshComponent* ButtonMesh = nullptr;

    UPROPERTY()
    UMaterialInstanceDynamic* ButtonLightMaterial = nullptr;

    bool bIsButtonEnabled = true;

    UPROPERTY(EditAnywhere, meta=(MakeEditWidget))
    FVector SpawnPoint = FVector(0.0f, -300.0f, 150.0f);

    bool bIsOverlappingButton = false;

    FTimerHandle BombExplodedHandler;

    UPROPERTY()
    TMap<ECEButtonDispatchBombStaticMesh, UStaticMesh*> StaticMeshes;

private:
    void InitializeAssets();

    void InitializeStaticMeshes();
    
    UFUNCTION()
    void Activate();

    UFUNCTION()
    void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UFUNCTION()
    void BombExplodedCallback();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Sets default values for this actor's properties
    ACEButtonDispatchBomb();
};
