#pragma once

#include "IHack.h"

class CAntiaim : public IHack
{
	bool enabled;
	bool copyPasta;
	var enabled_bool{"Enabled", &enabled};
	var copy_pasta_bool{"CP From Pastebin", &copyPasta};

public:

	CAntiaim()
	{
		variables.push_back(enabled_bool);
		variables.push_back(copy_pasta_bool);
	}

	// Inherited via IHack
	virtual const char *name() const override;
	virtual bool move(CUserCmd *) override;
};