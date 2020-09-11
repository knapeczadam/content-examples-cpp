#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CEButtonLightBulbBasic.generated.h"

UENUM()
enum class ECEButtonLightBulbBasicStaticMesh
{
    Button
};

UCLASS()
class CONTENTEXAMPLESCPP_API ACEButtonLightBulbBasic : public AActor
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

    UPROPERTY()
    TMap<ECEButtonLightBulbBasicStaticMesh, UStaticMesh*> StaticMeshes;

    UPROPERTY(EditAnywhere)
    class ACELightBulbBasic* TargetLight;

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
    
public:
    // Sets default values for this actor's properties
    ACEButtonLightBulbBasic();
};
