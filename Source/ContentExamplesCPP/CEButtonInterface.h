#pragma once

#include "CoreMinimal.h"

#include "CEPlayerInteractions.h"
#include "GameFramework/Actor.h"
#include "CEButtonInterface.generated.h"

UENUM()
enum class ECEButtonInterfaceStaticMesh
{
    Button
};

UCLASS()
class CONTENTEXAMPLESCPP_API ACEButtonInterface : public AActor
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

    bool bIsPawnOverlapping;

    UPROPERTY(EditAnywhere)
    TArray<AActor*> Targets;

    UPROPERTY()
    TMap<ECEButtonInterfaceStaticMesh, UStaticMesh*> StaticMeshes;

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
    ACEButtonInterface();
};
