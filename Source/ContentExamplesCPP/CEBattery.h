#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CEBattery.generated.h"

UENUM()
enum class ECEBatteryStaticMesh
{
    BatterySmall,
    BatteryMedium,
    BatteryBig
};

UENUM()
enum class ECEBatteryCurve
{
    Movement
};

UCLASS()
class CONTENTEXAMPLESCPP_API ACEBattery : public AActor
{
    GENERATED_BODY()
    
    UPROPERTY()
    USceneComponent* Base;

    UPROPERTY()
    UStaticMeshComponent* Battery;

    UPROPERTY()
    class UBoxComponent* ConnectorTrigger;

    UPROPERTY()
    class UTimelineComponent* MovementTimeline;

    UPROPERTY()
    UCurveFloat* MovementCurve;

    UPROPERTY(EditAnywhere)
    float Electricity;

    UPROPERTY()
    TMap<ECEBatteryStaticMesh, UStaticMesh*> StaticMeshes;

    UPROPERTY()
    TMap<ECEBatteryCurve, UCurveFloat*> Curves;

    UPROPERTY(EditAnywhere)
    UStaticMesh* BatteryMesh;
    
private:
    void InitializeAssets();

    void InitializeStaticMeshes();

    void InitializeCurves();
    
    UFUNCTION()
    void UpdateTimelineCallback();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Sets default values for this actor's properties
    ACEBattery();

    void Move(bool bIsUp);

    float GetElectricity() const;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
