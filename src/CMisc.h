#pragma once

#include "IHack.h"

class CMisc : public IHack
{
	bool bunnyhop;
	bool tauntsilde;

	var bunny_bool{"Bunny hop", &bunnyhop};
	var taunt_bool{"Taunt slide", &tauntsilde};

public:
	CMisc()
	{
		variables.push_back(bunny_bool);
		variables.push_back(taunt_bool);
	}

	const char *name() const override;
	bool move(CUserCmd *) override;
};
