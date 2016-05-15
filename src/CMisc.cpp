#include "CMisc.h"

#include "SDK.h"
#include "CEntity.h"

#include "Util.h"

const char *CMisc::name() const
{
	return "MISC ACTIONS";
}

bool CMisc::move(CUserCmd *pUserCmd)
{
	CEntity<> local{me};

	if(local.isNull())
		return false;

	if(bunnyhop)
	{
		//if(pUserCmd->buttons & IN_JUMP)
		//{
		//	if(!(local.get<int>(gEntVars.iFlags) & FL_ONGROUND))
		//		pUserCmd->buttons &= ~IN_JUMP;
		//
		//	return true;
		//}

		static bool firstjump = 0, fakejmp;

		if(pUserCmd->buttons & IN_JUMP)
			if(!firstjump)
				firstjump = fakejmp = 1;
			else if(!(gLocalPlayerVars.flags & FL_ONGROUND))
				if(fakejmp)
					fakejmp = 0;
				else
					pUserCmd->buttons &= ~IN_JUMP;
			else
				fakejmp = 1;
		else
			firstjump = 0;
	}

	if(tauntsilde)
	{
		if(local.get<int>(gEntVars.iPlayerCond) & tf_cond::TFCond_Taunting)
			local.set<int>(gEntVars.iPlayerCond, local.get<int>(gEntVars.iPlayerCond) & ~tf_cond::TFCond_Taunting);

		return true;
	}

	return false;
}
