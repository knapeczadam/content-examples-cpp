#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "CEBlueprintCommunicationLevelScriptActor.generated.h"

/**
 * 
 */
UCLASS()
class CONTENTEXAMPLESCPP_API ACEBlueprintCommunicationLevelScriptActor : public ALevelScriptActor
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere)
    class ACELightBulbBasic* TargetActor = nullptr;

    UPROPERTY(EditAnywhere)
    class ACEButtonDispatch* ButtonDispatcher = nullptr;

    UPROPERTY(EditAnywhere)
    class ATriggerBox* TriggerBox = nullptr;
    
private:
    UFUNCTION()
    void OnTriggerBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

    UFUNCTION()
    void OnTriggerEndOverlap(AActor* OverlappedACtor, AActor* OtherActor);
protected:
    virtual void BeginPlay() override;
};
