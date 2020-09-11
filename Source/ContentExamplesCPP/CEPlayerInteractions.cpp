#include "CEPlayerInteractions.h"

// Add default functionality here for any IPlayerInteractions functions that are not pure virtual.
FVector ICEPlayerInteractions::GrabInteract(FVector2D PlayerAimMovement, bool bIsInteracting)
{
    return FVector::OneVector;
}
