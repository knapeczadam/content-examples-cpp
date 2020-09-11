#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CEButtonDispatch.generated.h"

DECLARE_EVENT_OneParam(ACEButtonDispatch, FButtonToggledEvent, bool);

UENUM()
enum class ECEButtonDispatchStaticMesh
{
    Button
};

UCLASS()
class CONTENTEXAMPLESCPP_API ACEButtonDispatch : public AActor
{
    GENERATED_BODY()
    
    UPROPERTY()
    USceneComponent* Base;

    UPROPERTY()
    class UCapsuleComponent* TriggerCollision;

    UPROPERTY()
    UStaticMeshComponent* ButtonMesh;

    UPROPERTY()
    UMaterialInstanceDynamic* ButtonLightMaterial;

    FButtonToggledEvent ButtonToggledEvent;

    UPROPERTY()
    TMap<ECEButtonDispatchStaticMesh, UStaticMesh*> StaticMeshes;

private:
    void InitializeAssets();

    void InitializeStaticMeshes();

    UFUNCTION()
    void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    FORCEINLINE void BroadcastButtonToggled(bool bIsOn) { ButtonToggledEvent.Broadcast(bIsOn); }

public:
    // Sets default values for this actor's properties
    ACEButtonDispatch();
    
    FORCEINLINE FButtonToggledEvent& OnButtonToggled() { return ButtonToggledEvent; }
};
