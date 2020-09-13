#pragma once

#include "CoreMinimal.h"
#include "CEPlayerInteractions.h"
#include "GameFramework/Actor.h"
#include "CESpinningLogo.generated.h"

UENUM()
enum class ECESpinningLogoCurve
{
    Yaw
};

UENUM()
enum class ECESpinningLogoStaticMesh
{
    DoorLock
};

UENUM()
enum class ECESpinningLogoMaterial
{
    Chrome,
    Lit,
    Unlit
};

UCLASS()
class CONTENTEXAMPLESCPP_API ACESpinningLogo : public AActor, public ICEPlayerInteractions
{
    GENERATED_BODY()

    UPROPERTY()
    USceneComponent* Base = nullptr;

    UPROPERTY()
    UStaticMeshComponent* Logo = nullptr;

    UPROPERTY()
    class UTimelineComponent* RotationTimeline = nullptr;

    UPROPERTY()
    UCurveFloat* YawCurve = nullptr;

    bool bIsA = true;

    UPROPERTY()
    TMap<ECESpinningLogoCurve, UCurveFloat*> Curves;

    UPROPERTY()
    TMap<ECESpinningLogoStaticMesh, UStaticMesh*> StaticMeshes;

    UPROPERTY()
    TMap<ECESpinningLogoMaterial, UMaterialInterface*> Materials;
    
private:
    void InitializeAssets();

    void InitializeStaticMeshes();

    void InitializeCurves();

    void InitializeMaterials();

    virtual void PushedButton() override;

    UFUNCTION()
    void UpdateTimelineCallback();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Sets default values for this actor's properties
    ACESpinningLogo();
};
