#pragma once

#include "IHack.h"

class CNoise : public IHack
{

	bool enabled;

	var enabled_bool{"Enabled", &enabled};

public:
	CNoise()
	{
		variables.push_back(enabled_bool);
	}

	// Inherited via IHack
	virtual const char *name() const override;
	virtual bool move(CUserCmd *) override;

};