#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CEPlayerInteractions.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCEPlayerInteractions : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CONTENTEXAMPLESCPP_API ICEPlayerInteractions
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	FORCEINLINE virtual void PushedButton() { }

	virtual FVector GrabInteract(FVector2D PlayerAimMovement = FVector2D(0.0f, 0.0f), bool bIsInteracting = false);
};
