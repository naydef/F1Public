#pragma once

#include "IHack.h"

class CNospread : public IHack
{

	bool enabled;
	bool forceNospread;

	var enabled_bool{"Enabled", &enabled};

	var cmd_bool{"Forced Nospread", &forceNospread};

public:
	CNospread();

	// Inherited via IHack
	virtual const char *name() const override;
	virtual bool move(CUserCmd *pUserCmd) override;

private:
	int cmdNum;
	int seed;
};
