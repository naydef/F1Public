#include "CGlow.h"

#include "SDK.h"
#include "CDrawManager.h"
#include "CEntity.h"
#include "Util.h"
#include "Color.h"

// team 3: 0.490000 0.660000 0.770000 blue
// team 2: 0.740000 0.230000 0.230000 red

// these are some HORRIBLE MACROS
// one of the worst things i have ever done
#define REDCOLORFLOAT 0.49f, 0.66f, 0.77f
#define BLUCOLORFLOAT 0.74f, 0.23f, 0.23f

Color healthToColor(int health, int maxHealth)
{
	int r = 0, g = 0, b = 0;

	if(health > maxHealth)
	{
		b		= health - maxHealth;
		health = maxHealth;
	}

	int percent = (health / maxHealth) * 100;

	// TODO use hsl or hsv and then convert it to rgba

	r = 0 + percent * (255 - 0);
	g = 255 + percent * (0 - 255);
	// double resultBlue = color1.blue + percent * (color2.blue - color1.blue);

	return Color{r, g, b, 255};
}

int CGlowManager::registerGlowObject(CBaseEntity *ent, float r, float g, float b, float a, bool bRenderWhenOccluded, bool bRenderWhenUnoccluded, int nSplitScreenSlot)
{
	// assumes the glow object is real

	//typedef int(__thiscall * registerFn)(CGlowManager *, CBaseEntity *, Vector &, float, bool, bool, int);
	//static DWORD dwFn			= gSignatures.GetClientSignature("55 8B EC 51 53 56 8B F1 57 8B 5E 14");
	//static registerFn Register = (registerFn)dwFn;

	//return Register(pGlowObjectManger, ent, Vector{r, g, b}, a, bRenderWhenOccluded, bRenderWhenUnoccluded, nSplitScreenSlot);

	int nIndex;
	if(m_nFirstFreeSlot == GlowObjectDefinition_t::END_OF_FREE_LIST)
	{
		nIndex = glowObjects.AddToTail();
	}
	else
	{
		nIndex = m_nFirstFreeSlot;
		m_nFirstFreeSlot = glowObjects[nIndex].m_nNextFreeSlot;
	}

	//glowObjects[nIndex].hEntity = ent;
	glowObjects[nIndex].r = r;
	glowObjects[nIndex].g = g;
	glowObjects[nIndex].b = b;
	glowObjects[nIndex].a = a;
	glowObjects[nIndex].m_bRenderWhenOccluded = bRenderWhenOccluded;
	glowObjects[nIndex].m_bRenderWhenUnoccluded = bRenderWhenUnoccluded;
	glowObjects[nIndex].m_nSplitScreenSlot = nSplitScreenSlot;
	glowObjects[nIndex].m_nNextFreeSlot = GlowObjectDefinition_t::ENTRY_IN_USE;

	return nIndex;
}

void CGlowManager::unregisterGlowObject(int glowIndex)
{
	glowObjects[glowIndex].m_nNextFreeSlot = m_nFirstFreeSlot;
	glowObjects[glowIndex].hEntity = NULL;
	m_nFirstFreeSlot = glowIndex;
}

const char *CGlow::name() const { return "GLOW"; }

bool CGlow::init()
{
	// get the glow manager from this sig ( which is 2 bytes into the sig )
	DWORD dwGlowLoc = gSignatures.GetClientSignature("8B 0D ? ? ? ? A1 ? ? ? ? 56 8B 37") + 0x2;
	XASSERT(dwGlowLoc);
	pGlowObjectManger = *(CGlowManager **)dwGlowLoc;
	XASSERT(pGlowObjectManger);

	return true;
}

bool CGlow::paint()
{
	if(variables[2].bGet() || !variables[0].bGet())
		return true;

	/*
	for(auto &glowObj : pGlowObjectManger->glowObjects)
	{
		if(glowObj.hEntity != INVALID_EHANDLE_INDEX)
		{
			CEntity<> ent{HANDLE2INDEX(glowObj.hEntity)};

			if(ent.isNull())
				continue;

			if(ent->IsDormant())
				continue;

			//Color c = healthToColor(ent.get<int>(gEntVars.iHealth), getMaxHealth(ent));

			//glowObj.r = c.r() / 255.0f; // red
			//glowObj.g = c.g() / 255.0f; // green
			//glowObj.b = c.b() / 255.0f; // blue
			//glowObj.a = 1.0f; // 100% alpha

			Log::Console("team %d: %f %f %f", ent.get<int>(gEntVars.iTeam), glowObj.r, glowObj.g, glowObj.b);
		}
	}
	*/

	return true;
}

bool CGlow::inEntityLoop(int index)
{
	if(index == me) // we have no reason to perform glow on ourselves
		return false;

	// get the player
	CEntity<> player(index);

	// no nulls
	if(player.isNull())
	{
		if(glowObjects[index])
			glowObjects[index] = false;
		return false;
	}

	classId id = player->GetClientClass()->iClassID;

	if(id == classId::CTFPlayer)
	{
		// no dormants or deads - included check for if glow is disabled here since we have to tell the engine to stop the glow rather than just ending the loop
		if(player->IsDormant() || player.get<BYTE>(gEntVars.iLifeState) != LIFE_ALIVE || !enabled ||
		   (teamColor && gLocalPlayerVars.team == player.get<int>(gEntVars.iTeam)))
		{
			player.set<bool>(gEntVars.bGlowEnabled, false);
			player.castToPointer<CBaseCombatCharacter>()->DestroyGlowEffect();
			return false;
		}

		// if glow is not enabled
		if(player.get<bool>(gEntVars.bGlowEnabled))
			return false;

		// enable glow
		player.set<bool>(gEntVars.bGlowEnabled, true);
		// update the glow
		player.castToPointer<CBaseCombatCharacter>()->UpdateGlowEffect();

		return true;
	}
	// TODO replace with entity traits when that is done
	//else if(id == classId::CObjectDispenser || id == classId::CObjectSentrygun || id == classId::CObjectTeleporter || id == classId::CBaseObject /*||
	//		id == classId::CTFProjectile_Arrow || id == classId::CTFProjectile_Cleaver || id == classId::CTFProjectile_Flare ||
	//		id == classId::CTFProjectile_Rocket || id == classId::CTFProjectile_SentryRocket || id == classId::CTFGrenadePipebombProjectile*/)
	//{

	//	if(!enabled)
	//	{
	//		if(glowObjects[index] != -1 || player->IsDormant())
	//		{
	//			pGlowObjectManger->unregisterGlowObject(glowObjects[index]);
	//			glowObjects[index] = -1;
	//		}
	//	}
	//	else
	//	{
	//		int team = player.get<int>(gEntVars.iTeam);
	//		// create a glow object for this entity
	//		if(glowObjects[index] == -1 && (teamColor && gLocalPlayerVars.team == team))
	//		{
	//			if(team == 3)
	//			{
	//				glowObjects[index] = pGlowObjectManger->registerGlowObject(player.castToPointer<CBaseEntity>(), BLUCOLORFLOAT, 1.0f, true, true, -1);
	//			}
	//			else if(team == 2)
	//			{
	//				glowObjects[index] = pGlowObjectManger->registerGlowObject(player.castToPointer<CBaseEntity>(), REDCOLORFLOAT, 1.0f, true, true, -1);
	//			}
	//		}
	//		else if(player->IsDormant())
	//		{
	//			if(glowObjects[index] != -1)
	//			{
	//				pGlowObjectManger->unregisterGlowObject(glowObjects[index]);
	//				glowObjects[index] = -1;	
	//			}
	//		}
	//	}
	//}
	return false;
}
