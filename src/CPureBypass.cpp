#include "CPureBypass.h"

#include "SDK.h"

// just return true in order to bypass pure
bool __stdcall Hooked_IsPlayingBack() { return true; }

const char *CPureBypass::name() const { return "PURE-BYPASS"; }

bool CPureBypass::init()
{
	dwPureLoc = *reinterpret_cast<PDWORD>(gSignatures.GetEngineSignature("A1 ? ? ? ? 56 33 F6 85 C0") + 0x1);
	XASSERT(dwPureLoc);

	// demoPlayerHook = new VMTBaseManager();
	// demoPlayerHook->Init(gInts.DemoPlayer);
	// demoPlayerHook->HookMethod(&Hooked_IsPlayingBack, gOffsets.isPlayingBack);
	// demoPlayerHook->Rehook();

	return true;
}

bool CPureBypass::paint(/*CUserCmd **/)
{
	// bytepatch to bypass sv_pure checks

	// Works as expected.
	if(*(PDWORD)dwPureLoc)
	{
		*(PDWORD)dwPureLoc = NULL;
	}

	// gInts.DemoPlayer->m_bInterpolateView = true;

	return true;
}
