// Copyright Epic Games, Inc. All Rights Reserved.


#include "ContentExamplesCPPGameModeBase.h"
#include "CEHUDComms.h"
#include "CEMyCharacterComms.h"

AContentExamplesCPPGameModeBase::AContentExamplesCPPGameModeBase()
{
    DefaultPawnClass = ACEMyCharacterComms::StaticClass();
    HUDClass = ACEHUDComms::StaticClass();

}
