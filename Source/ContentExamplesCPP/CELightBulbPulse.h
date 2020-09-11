#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CELightBulbPulse.generated.h"

UENUM()
enum class ECELightBulbPulseCurve
{
    Flash
};

UENUM()
enum class ECELightBulbPulseStaticMesh
{
    LightBulb
};

UCLASS()
class CONTENTEXAMPLESCPP_API ACELightBulbPulse : public AActor
{
    GENERATED_BODY()
    
    UPROPERTY()
    UStaticMeshComponent* LightBulb;

    UPROPERTY()
    class UPointLightComponent* Light;

    UPROPERTY()
    class UTimelineComponent* LightPulseTimeline;

    UPROPERTY()
    UCurveFloat* FlashCurve;

    UPROPERTY()
    UMaterialInstanceDynamic* LightBulbMaterial1;

    UPROPERTY()
    UMaterialInstanceDynamic* LightBulbMaterial2;

    FLinearColor LightColor;

    UPROPERTY()
    TMap<ECELightBulbPulseStaticMesh, UStaticMesh*> StaticMeshes;

    UPROPERTY()
    TMap<ECELightBulbPulseCurve, UCurveFloat*> Curves;
    
private:
    void InitializeAssets();

    void InitializeStaticMeshes();

    void InitializeCurves();

    void UpdateLightColor(FLinearColor NewColor);

    UFUNCTION()
    void UpdateTimelineCallback();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Sets default values for this actor's properties
    ACELightBulbPulse();
    
    void ToggleLight(bool bIsOn);
};
