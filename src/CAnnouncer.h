#pragma once

#include "baseHeaders.h"

#include "IHack.h"

// this is gir's stuff mixed with the magic of IHack

class CAnnouncer : public IHack, public IGameEventListener2
{
public:
	CAnnouncer()
	{
		killStreakTimeout = 4.0f;
	}

	// IGameEventListener2 inherits
	void FireGameEvent(IGameEvent *pEvent) override;

	// IHack inherits
	bool init() override;

	const char *name() const override
	{
		return "ANNOUNCER";
	}

	bool move(CUserCmd *) override;

private:
	void PlaySound(const char *soundFile);
	char chKillstreakSound[50] = "";

protected:
	float lastKillTime;
	int killCounter;
	int killStreakCounter;
	float killStreakTimeout;
};
