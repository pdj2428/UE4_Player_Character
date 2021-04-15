// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character_BRGameMode.h"
#include "Character_BRCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACharacter_BRGameMode::ACharacter_BRGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
