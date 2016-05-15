#include "CAntiaim.h"
#include "CAimHelper.h"

#include "SDK.h"

const char *CAntiaim::name() const
{
	return "ANTI-AIM";
}

bool CAntiaim::move(CUserCmd *pUserCmd)
{
	if(!enabled)
		return false;
	//if ( pUserCmd->buttons & IN_ATTACK )
	//	return true;

	pUserCmd->viewangles.z -= 180;
	//newAngles.y -= 180;
	//pUserCmd->forwardmove = -pUserCmd->forwardmove;

	//if(!(pUserCmd->buttons & IN_ATTACK))
	pUserCmd->sidemove = -pUserCmd->sidemove;
	//pUserCmd->forwardmove = -pUserCmd->forwardmove;

	if(!(pUserCmd->buttons & IN_ATTACK))
	{
		if(!copyPasta)
		{
			// fakeup
			pUserCmd->viewangles.x = -271;

			int random = rand() % 100;

			// Small chance of starting backwards
			if(random < 2)
				// Look forwards
				pUserCmd->viewangles.y = 180;

			// Some gitter
			if(random < 15)
			{
				float change = -70 + (rand() % (int)(140 + 1));
				pUserCmd->viewangles.y += change;
			}
			if(random == 69)
			{
				float change = -90 + (rand() % (int)(180 + 1));
				pUserCmd->viewangles.y += change;
			}
		}
		else
		{

			Vector newAngles = pUserCmd->viewangles;

			float flTime = gInts.Globals->curtime;
			Vector aimalways(271, 271, 271);

			static bool stetikleol = false;
			if(pUserCmd->buttons & IN_JUMP)
			{
				static bool fuk = false;
				static bool sheisse = false;
				fuk = !fuk;
				if(fuk)
				{
					sheisse = !sheisse;
					newAngles.x = 180;
					newAngles.y = (aimalways.y - (sheisse ? -90 : 30));
				}
				else
				{
					newAngles.x = 180;
					newAngles.y = (aimalways.y + 180);
					//*bSendPackets = false;
				}
			}
			else if(pUserCmd->buttons & IN_DUCK)
			{
				static bool lelfuck;
				if(lelfuck)
				{
					newAngles.x = 180;
					newAngles.y = 360;
					lelfuck = true;
				}
				else
				{
					newAngles.x = 180;
					newAngles.y = 180;
					lelfuck = false;
				}
			}
			else if(pUserCmd->buttons & IN_FORWARD)
			{
				stetikleol = !stetikleol; // 
				if(stetikleol)
				{
					newAngles.x = 180;
					newAngles.y = (aimalways.y + 180);
				}
				else
				{

					newAngles.x = 180;
					newAngles.y = (aimalways.y + 90, 45, 270);
					//*bSendPackets = false;
				}
			}
			else if(pUserCmd->buttons & IN_BACK)
			{
				stetikleol = !stetikleol; // 
				if(stetikleol)
				{
					newAngles.x = 180;
					newAngles.y = (aimalways.y + 180);
				}
				else
				{

					newAngles.x = 180;
					newAngles.y = (aimalways.y + 90, 45, 270);
					//*bSendPackets = false;
				}
			}
			else if(pUserCmd->buttons & IN_MOVELEFT)
			{
				static bool fuk = false;
				static bool sheisse = false;
				fuk = !fuk;
				if(fuk)
				{
					sheisse = !sheisse;
					newAngles.x = 180;
					newAngles.y = (float)(fmod(flTime / 0.01f * 360, 360));
				}
				else
				{
					newAngles.x = 180;
					newAngles.y = (aimalways.y + 180);
					//*bSendPackets = false;
				}
			}
			else if(pUserCmd->buttons & IN_MOVERIGHT)
			{
				static bool fuk = false;
				static bool sheisse = false;
				fuk = !fuk;
				if(fuk)
				{
					sheisse = !sheisse;
					newAngles.x = 180;
					newAngles.y = (float)(fmod(flTime / 0.01f * 320, 320));
				}
				else
				{
					newAngles.x = 180;
					newAngles.y = (aimalways.y + 360);
					//*bSendPackets = false;
					//static bool fuk = false;
					//static bool sheisse = false;
					//fuk = !fuk;
					//if (fuk){
					//	sheisse = !sheisse;
					//	newAngles.x = 180;
					//	newAngles.y = (aimalways.y + (sheisse ? +70 : 80));
					//}
					//else
					//{
					//	newAngles.x = 180;
					//	newAngles.y = (aimalways.y + 270);
					//	*bSendPackets = false;
				}
			}
			else
			{
				static bool fuk = false;
				static bool sheisse = false;
				fuk = !fuk;
				if(fuk)
				{
					sheisse = !sheisse;
					newAngles.x = 180;
					newAngles.y = (aimalways.y + (sheisse ? -180 : 270));
				}
				else
				{
					newAngles.x = 180;
					newAngles.y = (aimalways.y + 270);
					//*bSendPackets = false;
				}
			}

			silentMovementFix(pUserCmd, newAngles);

			pUserCmd->viewangles = newAngles;

			pUserCmd->forwardmove = -pUserCmd->forwardmove;
		}

	}

	return true;
}
