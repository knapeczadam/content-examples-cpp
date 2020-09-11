#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CEButtonLightBulbPulse.generated.h"

UENUM()
enum class ECEButtonLightBulbPulseStaticMesh
{
    Button
};

UCLASS()
class CONTENTEXAMPLESCPP_API ACEButtonLightBulbPulse : public AActor
{
    GENERATED_BODY()
    
    UPROPERTY()
    USceneComponent* Base;

    UPROPERTY()
    class UCapsuleComponent* TriggerCollision;

    UPROPERTY()
    UStaticMeshComponent* ButtonMesh;

    UPROPERTY()
    UMaterialInstanceDynamic* ButtonLightMaterial;

    UPROPERTY()
    TMap<ECEButtonLightBulbPulseStaticMesh, UStaticMesh*> StaticMeshes;

private:
    void InitializeAssets();

    void InitializeStaticMeshes();

    UFUNCTION()
    void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Sets default values for this actor's properties
    ACEButtonLightBulbPulse();
};
