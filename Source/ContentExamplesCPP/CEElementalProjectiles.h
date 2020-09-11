#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CEElementalProjectiles.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCEElementalProjectiles : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CONTENTEXAMPLESCPP_API ICEElementalProjectiles
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void ElementalDamage(float HeatDamage=0.0f, float WaterDamage=0.0f) = 0;
};
