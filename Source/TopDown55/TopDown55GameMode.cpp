// Copyright Epic Games, Inc. All Rights Reserved.

#include "TopDown55GameMode.h"
#include "TopDown55PlayerController.h"
#include "TopDown55Character.h"
#include "UObject/ConstructorHelpers.h"

ATopDown55GameMode::ATopDown55GameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ATopDown55PlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}