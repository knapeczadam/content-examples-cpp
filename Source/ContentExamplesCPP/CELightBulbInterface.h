#pragma once

#include "CoreMinimal.h"
#include "CEPlayerInteractions.h"
#include "GameFramework/Actor.h"
#include "CELightBulbInterface.generated.h"

UENUM()
enum class ECELightBulbInterfaceCurve
{
    Flash
};

UENUM()
enum class ECELightBulbInterfaceStaticMesh
{
    LightBulb
};

UCLASS()
class CONTENTEXAMPLESCPP_API ACELightBulbInterface : public AActor, public ICEPlayerInteractions
{
    GENERATED_BODY()

    UPROPERTY()
    UStaticMeshComponent* LightBulb = nullptr;

    UPROPERTY()
    class UPointLightComponent* Light = nullptr;

    UPROPERTY()
    class UTimelineComponent* LightFlashTimeline = nullptr;

    UPROPERTY()
    UCurveFloat* FlashCurve = nullptr;

    UPROPERTY()
    UMaterialInstanceDynamic* LightBulbMaterial1 = nullptr;

    UPROPERTY()
    UMaterialInstanceDynamic* LightBulbMaterial2 = nullptr;

    bool bIsA = true;

    UPROPERTY()
    TMap<ECELightBulbInterfaceCurve, UCurveFloat*> Curves;

    UPROPERTY()
    TMap<ECELightBulbInterfaceStaticMesh, UStaticMesh*> StaticMeshes;
    
private:
    void InitializeAssets();

    void InitializeStaticMeshes();

    void InitializeCurves();

    void UpdateLightColor(FLinearColor NewColor);

    UFUNCTION()
    void UpdateTimelineCallback();

    void PushedButton() override;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Sets default values for this actor's properties
    ACELightBulbInterface();
};
