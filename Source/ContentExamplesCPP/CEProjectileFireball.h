#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CEProjectileFireball.generated.h"

UENUM()
enum class ECEProjectileFireballCurve
{
    Scale
};

UENUM()
enum class ECEProjectileFireballParticleSystem
{
    Explosion,
    Fireball
};

UCLASS()
class CONTENTEXAMPLESCPP_API ACEProjectileFireball : public AActor
{
    GENERATED_BODY()

    UPROPERTY()
    class USphereComponent* Collision = nullptr;

    UPROPERTY()
    UParticleSystemComponent* Effect = nullptr;

    UPROPERTY()
    class UTimelineComponent* LifeSpanTimeline = nullptr;

    UPROPERTY()
    UCurveFloat* ScaleCurve = nullptr;

    UPROPERTY()
    class UProjectileMovementComponent* ProjectileMovement1 = nullptr;

    FVector Velocity = FVector(1500.0f, 0.0f, 150.0f);

    FTimerHandle FinishedHandler;

    UPROPERTY()
    TMap<ECEProjectileFireballCurve, UCurveFloat*> Curves;

    UPROPERTY()
    TMap<ECEProjectileFireballParticleSystem, UParticleSystem*> ParticleSystems;
    
private:
    void InitializeAssets();

    void InitializeCurves();

    void InitializeParticleSystems();

    UFUNCTION()
    void UpdateTimelineCallback();

    UFUNCTION()
    void FinishedTimelineCallback();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Sets default values for this actor's properties
    ACEProjectileFireball();
    
    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

    virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
};
