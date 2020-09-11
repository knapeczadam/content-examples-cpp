#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CEBomb.generated.h"

DECLARE_EVENT(ACEBomb, FBombExplodedEvent);

UENUM()
enum class ECEBombCurve
{
    Fuse
};

UENUM()
enum class ECEBombStaticMesh
{
    Bomb
};

UENUM()
enum class ECEBombParticleSystem
{
    Explosion,
    Fuse
};

UCLASS()
class CONTENTEXAMPLESCPP_API ACEBomb : public AActor
{
    GENERATED_BODY()

    UPROPERTY()
    UStaticMeshComponent* Bomb;

    UPROPERTY(EditAnywhere)
    UParticleSystemComponent* FuseEffect;

    UPROPERTY()
    class UTimelineComponent* FuseTimeline;

    UPROPERTY()
    UMaterialInstanceDynamic* FuseMaterial;

    UPROPERTY(EditAnywhere)
    UCurveFloat* FuseCurve;

    UPROPERTY()
    TMap<ECEBombCurve, UCurveFloat*> Curves;

    UPROPERTY()
    TMap<ECEBombStaticMesh, UStaticMesh*> StaticMeshes;

    UPROPERTY()
    TMap<ECEBombParticleSystem, UParticleSystem*> ParticleSystems;

    FTimerHandle FinishedHandler;

    FBombExplodedEvent BombExplodedEvent;
    
private:
    void InitializeAssets();

    void InitializeStaticMeshes();

    void InitializeCurves();

    void InitializeParticleSystems();

    UFUNCTION()
    void UpdateTimelineCallback();

    UFUNCTION()
    void FinishedTimelineCallback();
    
protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    FORCEINLINE void BroadcastBombExploded() { BombExplodedEvent.Broadcast(); }

public:
    // Sets default values for this actor's properties
    ACEBomb();
    
    FORCEINLINE FBombExplodedEvent& OnBombExploded() { return BombExplodedEvent; }
};
