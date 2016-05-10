#pragma once

#include "IHack.h"

#include "CTargetHelper.h"
#include "CAimHelper.h"

class CAutoAirblast : public IHack
{
	bool enabled;
	bool breakMode;

	var enabled_bool{"Enabled", &enabled};
	var break_bool{"Break mode", &breakMode};

	// std::unordered_map<int, float> targets;

	// maps entity indexes to their distance
	CDistanceTargetSystem targs;

	CSnapAimSystem aimer;

public:
	CAutoAirblast();

	// Inherited via IHack
	virtual const char *name() const override;
	virtual bool move(CUserCmd *pUserCmd) override;
	virtual bool inEntityLoop(int index) override;

private:
	// int findBestTarget();
};
