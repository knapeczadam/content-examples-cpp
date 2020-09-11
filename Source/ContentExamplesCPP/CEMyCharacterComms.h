#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CEMyCharacterComms.generated.h"

UCLASS()
class CONTENTEXAMPLESCPP_API ACEMyCharacterComms : public ACharacter
{
    GENERATED_BODY()
public:
    UPROPERTY()
    class UCameraComponent* FirstPersonCamera;

    UPROPERTY()
    USceneComponent* RightHand;

    UPROPERTY()
    USceneComponent* LeftHand;
    
    bool bPowersEnabled;
    
    bool bInteracting;

    UPROPERTY()
    AActor* ActorToInteractWith;

    float ClosestDistance;

    float DeltaSeconds;

    FTimerHandle ShootFireTimer;
    FTimerHandle ShootWaterTimer;

    bool bDoOnceFired;
    
private:
    bool GetClosestInteractiveActor(const TArray<AActor*> OtherActors);

    FTransform ProjectileTrace(FVector ProjectileOrigin);

    void ShootFireball();

    void ShootWater();

    void Turn(float AxisValue);

    void LookUp(float AxisValue);

    void MoveForward(float AxisValue);

    void MoveRight(float AxisValue);

    void RunPressed();

    void RunReleased();

    void ShootWaterPressed();

    void ShootWaterReleased();

    void ShootFirePressed();

    void ShootFireReleased();

    void ResetDoOnce();

public:
    // Sets default values for this character's properties
    ACEMyCharacterComms();
    
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
