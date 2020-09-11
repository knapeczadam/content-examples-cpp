#include "CEBlueprintCommunicationLevelScriptActor.h"
#include "CEButtonDispatch.h"
#include "CEHUDComms.h"
#include "CEMyCharacterComms.h"
#include "Engine/TriggerBox.h"
#include "Kismet/GameplayStatics.h"

void ACEBlueprintCommunicationLevelScriptActor::BeginPlay()
{
    Super::BeginPlay();
    check(ButtonDispatcher);
    check(TargetActor);
    check(TriggerBox);
    
    ButtonDispatcher->OnButtonToggled().AddUFunction(TargetActor, FName(TEXT("ToggleLight"))); // Call a custom event in the target to turn its light on/off
    TriggerBox->OnActorBeginOverlap.AddDynamic(this, &ACEBlueprintCommunicationLevelScriptActor::OnTriggerBeginOverlap);
    TriggerBox->OnActorEndOverlap.AddDynamic(this, &ACEBlueprintCommunicationLevelScriptActor::OnTriggerEndOverlap);
}

/*
 * Overlap event fires when a pawn touches the trigger actor
 */
void ACEBlueprintCommunicationLevelScriptActor::OnTriggerBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
    ACEMyCharacterComms* CurrentPlayer = Cast<ACEMyCharacterComms>(OtherActor); // Cast the overlapping pawn to a specific class in order to access its variables
    ACEHUDComms* CurrentHUD = Cast<ACEHUDComms>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD()); // Cast the HUD to a specific class in order to access its variables
    if (CurrentPlayer && CurrentHUD)
    {
        CurrentPlayer->bPowersEnabled = true;
        CurrentHUD->bPowersEnabled = true;
    }
}

/*
 * Overlap event fires when a pawn exits the trigger actor
 */
void ACEBlueprintCommunicationLevelScriptActor::OnTriggerEndOverlap(AActor* OverlappedACtor, AActor* OtherActor)
{
    ACEMyCharacterComms* CurrentPlayer = Cast<ACEMyCharacterComms>(OtherActor);
    ACEHUDComms* CurrentHUD = Cast<ACEHUDComms>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
    if (CurrentPlayer && CurrentHUD)
    {
        CurrentPlayer->bPowersEnabled = false;
        CurrentHUD->bPowersEnabled = false;
    }
}
