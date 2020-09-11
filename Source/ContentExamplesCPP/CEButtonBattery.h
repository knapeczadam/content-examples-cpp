#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CEButtonBattery.generated.h"

UENUM()
enum class ECEButtonBatteryStaticMesh
{
    Button
};

UCLASS()
class CONTENTEXAMPLESCPP_API ACEButtonBattery : public AActor
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

    UPROPERTY(EditAnywhere)
    class ACEBattery* TargetBattery;

    UPROPERTY()
    TMap<ECEButtonBatteryStaticMesh, UStaticMesh*> StaticMeshes;
    
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
    ACEButtonBattery();
};
