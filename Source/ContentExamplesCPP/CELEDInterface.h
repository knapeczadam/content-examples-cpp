#pragma once

#include "CoreMinimal.h"
#include "CEPlayerInteractions.h"
#include "GameFramework/Actor.h"
#include "CELEDInterface.generated.h"

UENUM()
enum class ECELEDInterfaceCurve
{
    Flash
};

UENUM()
enum class ECELEDInterfaceStaticMesh
{
    Led
};

UCLASS()
class CONTENTEXAMPLESCPP_API ACELEDInterface : public AActor, public ICEPlayerInteractions
{
    GENERATED_BODY()

    UPROPERTY()
    UStaticMeshComponent* LED = nullptr;

    UPROPERTY()
    class UPointLightComponent* Light = nullptr;

    UPROPERTY()
    class UTimelineComponent* LightFlashTimeline = nullptr;

    UPROPERTY()
    UMaterialInstanceDynamic* LightBulbMaterial1 = nullptr;

    UPROPERTY()
    UMaterialInstanceDynamic* LightBulbMaterial2 = nullptr;

    UPROPERTY()
    UCurveFloat* FlashCurve = nullptr;

    bool bIsA = true;

    UPROPERTY()
    TMap<ECELEDInterfaceCurve, UCurveFloat*> Curves;

    UPROPERTY()
    TMap<ECELEDInterfaceStaticMesh, UStaticMesh*> StaticMeshes;
    
private:
    void InitializeAssets();

    void InitializeStaticMeshes();

    void InitializeCurves();

    virtual void PushedButton() override;

    void UpdateLightColor(FLinearColor NewColor);

    UFUNCTION()
    void UpdateTimelineCallback();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Sets default values for this actor's properties
    ACELEDInterface();
};
