#include "SDK.h"
#include "Client.h"

#include "Util.h"

//============================================================================================
bool __fastcall Hooked_CreateMove(PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd *pCommand)
{

	//void *EBP;

	// move ebp register into EBP
	//__asm mov EBP, ebp;

	// dereference EBP to get the CHLCLient Createmove

	DWORD createMoveEBP = NULL;

	return gHack.createMove(ClientMode, edx, input_sample_frametime, pCommand, createMoveEBP);
}

void __fastcall Hooked_CHLClient_CreateMove(PVOID CHLClient, int edx, int sequence_number, float input_sample_time, bool active)
{

	CUserCmd *pUserCmd = gInts.Input->GetUserCmd(sequence_number);

	if(!pUserCmd)
		return;

	//SPEEDHACK (only works on hvh servers)
	// creds gir
	// TODO move this into its own hack
	static int iSpeedCounter = 0; //Setup a global counter.
	if(iSpeedCounter > 0 && GetAsyncKeyState(VK_LSHIFT)) //If I'm pressing MOUSE4 and the counter was not 0.
	{
		iSpeedCounter--; //Decrement the counter.
		pUserCmd->tick_count--; //Normalize tick_count.
		_asm
		{
			push eax; //Preserve EAX to the stack.
			mov eax, dword ptr ss : [ebp]; //Move EBP in to EAX.
			mov eax, [ eax ]; //Derefrence the base pointer.
			lea eax, [ eax + 0x4 ]; //Load the return address in to EAX.
			sub[ eax ], 0x5; //Make it return to -5 where it would normally.
			pop eax; //Restore EAX
		}
	}
	else
	{
		iSpeedCounter = 7; //We want to run this 7 times.
	}

	VMTManager &hook = VMTManager::GetHook(CHLClient); // Get a pointer to the instance of your VMTManager with the function GetHook.
	hook.GetMethod<void(__thiscall *)(PVOID, int, float, bool)>(gOffsets.createMoveOffset)(CHLClient, sequence_number, input_sample_time,
		active); // Call the original.

	// resign the cmd
	CVerifiedUserCmd *pSafeCommand = (CVerifiedUserCmd *)(*(DWORD *)(gInts.Input.get() + 0xF8) + (sizeof(CVerifiedUserCmd) * (sequence_number % 90)));
	pSafeCommand->m_cmd			  = *pUserCmd;
	pSafeCommand->m_crc			  = GetChecksumForCmd(pSafeCommand->m_cmd);
}
//============================================================================================
int __fastcall Hooked_Key_Event(PVOID CHLClient, int edx, int eventcode, ButtonCode_t keynum, const char *currentBinding)
{
	return gHack.keyEvent(CHLClient, edx, eventcode, keynum, currentBinding);
}

// no checks here ;)
CUserCmd *__fastcall Hooked_GetUserCmd(PVOID pInput, int edx, int sequence_number)
{
	return &(*(CUserCmd **)((DWORD)gInts.Input.get() + 0xF4))[sequence_number % 90];
}
