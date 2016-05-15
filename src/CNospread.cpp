#include "CNospread.h"

#include "SDK.h"
#include "CEntity.h"

CNospread::CNospread()
{
	// witteks magic number
	// i think it results in a seed of 39
	cmdNum = 2076615043;
	seed = 39;

	Log::Console("SEED IS %d", seed);

	variables.push_back(enabled_bool);
	variables.push_back(cmd_bool);
}

const char *CNospread::name() const { return "NO-SPREAD"; }

float getSpread(int id)
{
	switch(id)
	{
	case 16:
	case 203:
	case 24:
	case 210:
	case 460:
	case 61:
	case 224:
	case 751:
	case 1149:
	case 161:
	case 525:
	case 1006:
	case 1142:
		return 0.025f;
	case 15:
	case 202:
	case 41:
	case 298:
	case 312:
	case 424:
	case 654:
	case 793:
	case 802:
	case 811:
	case 832:
	case 850:
	case 882:
	case 891:
	case 900:
	case 909:
	case 958:
	case 967:
		return 0.1f;
	case 22:
	case 209:
	case 160:
	case 294:
	case 23:
		return 0.04f;
	}
	return 0;
}

bool CNospread::move(CUserCmd *pUserCmd)
{
	if(!enabled)
		return false;

	if(!forceNospread)
	{
		if(pUserCmd->buttons & IN_ATTACK)
		{

			CEntity<> local{me};

			Vector vForward, vRight, vUp, vSpread;
			Vector qFinal;

			//C_Random randGen;

			int iSeed = pUserCmd->random_seed & 0xFF;
			gInts.RandomStream->SetSeed(iSeed);

			float flRandomX = gInts.RandomStream->RandomFloat(-0.5, 0.5) + gInts.RandomStream->RandomFloat(-0.5, 0.5);
			float flRandomY = gInts.RandomStream->RandomFloat(-0.5, 0.5) + gInts.RandomStream->RandomFloat(-0.5, 0.5);

			ClampAngle(pUserCmd->viewangles);

			// get the spread from the weapon from the local player
			// lets hope none of them are nulls
			// also if this is called on a melee weapon then woopsies
			CEntity<CBaseCombatWeapon> baseWeapon{HANDLE2INDEX(local.get<int>(gEntVars.hActiveWeapon))};

			if(baseWeapon.isNull())
				return false;

			// this is a simple check that might fail for some guns
			// TODO get a better check ( maybe a huge switch statement? )
			// TODO convert this to an etags check for this.
			if(baseWeapon->GetMaxClip1() == -1 || baseWeapon->GetMaxClip2() == -1)
				return false;

			float flSpread = 0.0f;

			// if((gInts.Globals->cur_time - local.get<float>(gEntVars.flLastAttackTime)) < 2.0f)
			//flSpread = baseWeapon.castToPointer<CTFBaseWeaponGun>()->WeaponGetSpread();

			flSpread = getSpread(baseWeapon.castToPointer<CTFBaseWeapon>()->getWeaponID());

			if(flSpread == 0.0f)
			{
				Log::Console("No spread!");
				return false;
			}

			Log::Console("flspread: %f", flSpread);

			vSpread[0] = flRandomX * flSpread;
			vSpread[1] = flRandomY * flSpread;
			vSpread[2] = 0;

			AngleVectors(pUserCmd->viewangles, &vForward, &vRight, &vUp);

			Vector vNewAngles = vForward + (vRight * vSpread[ 0 ]) + (vUp * vSpread[ 1 ]);

			// Vector vNewAngles = vForward + (vRight * flRandomX * flSpread) + (vUp * flRandomY * flSpread);

			vNewAngles.NormalizeInPlace();

			//vForward = vForward + (vForward - vNewAngles);

			VectorAngles(vNewAngles, qFinal);

			pUserCmd->viewangles += pUserCmd->viewangles - qFinal;

			ClampAngle(pUserCmd->viewangles);


			// TODO remove me
			// gInts.Engine->SetViewAngles(qFinal);
		}
	}
	else
	{
		pUserCmd->command_number = cmdNum;
		pUserCmd->random_seed	= seed;
	}

	return true;
}
