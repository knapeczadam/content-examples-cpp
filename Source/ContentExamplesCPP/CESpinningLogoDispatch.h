#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CESpinningLogoDispatch.generated.h"

UENUM()
enum class ECESpinningLogoDispatchCurve
{
    Yaw
};

UENUM()
enum class ECESpinningLogoDispatchStaticMesh
{
    DoorLock
};

UENUM()
enum class ECESpinningLogoDispatchMaterial
{
    Chrome,
    Lit,
    Unlit
};

UCLASS()
class CONTENTEXAMPLESCPP_API ACESpinningLogoDispatch : public AActor
{
    GENERATED_BODY()
    
    UPROPERTY()
    USceneComponent* Base;

    UPROPERTY()
    UStaticMeshComponent* Logo;

    UPROPERTY()
    class UTimelineComponent* RotationTimeline;
    
    UPROPERTY(EditAnywhere)
    class ACEButtonDispatch* TargetButton;

    UPROPERTY()
    UCurveFloat* YawCurve;

    UPROPERTY()
    TMap<ECESpinningLogoDispatchCurve, UCurveFloat*> Curves;

    UPROPERTY()
    TMap<ECESpinningLogoDispatchStaticMesh, UStaticMesh*> StaticMeshes;

    UPROPERTY()
    TMap<ECESpinningLogoDispatchMaterial, UMaterialInterface*> Materials;
    
private:
    void InitializeAssets();

    void InitializeStaticMeshes();

    void InitializeCurves();

    void InitializeMaterials();

    UFUNCTION()
    void ToggleSpinning(bool bIsOn);

    UFUNCTION()
    void UpdateTimelineCallback();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Sets default values for this actor's properties
    ACESpinningLogoDispatch();
};
