// LeaveMeAlone Game by Netologiya. All RightsReserved


#include "LMAGameModeBase.h"
#include "LMADefaultCharacter.h"
#include "LMAPlayerController.h"

ALMAGameModeBase::ALMAGameModeBase() 
{
	DefaultPawnClass = ALMADefaultCharacter::StaticClass();
	PlayerControllerClass = ALMAPlayerController::StaticClass();
}
