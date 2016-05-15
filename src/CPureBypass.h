#pragma once

#include "IHack.h"

#include "SDK.h"

class CPureBypass : public IHack
{
	DWORD dwPureLoc = NULL;

	VMTBaseManager *demoPlayerHook;

public:
	CPureBypass() { demoPlayerHook = nullptr; };
	~CPureBypass() { delete demoPlayerHook; };

	const char *name() const override;

	bool init() override;

	bool paint(/*CUserCmd **/) override;
};
