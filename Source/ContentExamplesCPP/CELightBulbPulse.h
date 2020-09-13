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
    UStaticMeshComponent* LightBulb = nullptr;

    UPROPERTY()
    class UPointLightComponent* Light = nullptr;

    UPROPERTY()
    class UTimelineComponent* LightPulseTimeline = nullptr;

    UPROPERTY()
    UCurveFloat* FlashCurve = nullptr;

    UPROPERTY()
    UMaterialInstanceDynamic* LightBulbMaterial1 = nullptr;

    UPROPERTY()
    UMaterialInstanceDynamic* LightBulbMaterial2 = nullptr;

    FLinearColor LightColor = FLinearColor(1.0f, 0.7f, 0.25f, 0.0f);

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
