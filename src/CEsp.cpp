#include "CEsp.h"

#include "SDK.h"
#include "CDrawManager.h"
#include "CEntity.h"
#include "Util.h"

const char *CESP::name() const
{
	return "EXTRA-SENSORY PERCEPTION";
}

bool CESP::inEntityLoop(int index)
{
	if(!enabled)
		return false;

	if(index == me) // we have no reason to perform esp on ourselves
		return false;

	// get the player
	CEntity<> player(index);

	// no nulls
	if(player.isNull())
		return false;

	// no dormants
	if(player->IsDormant())
		return false;

	Vector vecWorld, vecScreen;
	Vector min, max, origin, localOrigin;
	//Vector bot, top;

	player->GetRenderBounds(min, max);

	origin = player->GetAbsOrigin();

	player->GetWorldSpaceCenter(vecWorld);

	if(!gDrawManager.WorldToScreen(vecWorld, vecScreen))
		return false;

	//if(!gDrawManager.WorldToScreen(Vector(origin.x, origin.y, origin.z + min.z), bot) || !gDrawManager.WorldToScreen(Vector(origin.x, origin.y, origin.z + max.z), top))
	//	return false;

	//float flHeight = bot.y - top.y;
	//float flWidth = flHeight / 4.0f;
	DWORD teamColor = COLORCODE(0,0,0,255);

	if(renderFriends)
	{
		if(isPlayerOnFriendsList(index))
		{
			teamColor = COLORCODE(0, 255, 0, 255);
		}
		else
		{
			teamColor = gDrawManager.dwGetTeamColor(player.get<int>(gEntVars.iTeam));
		}
	}
	else
	{
		teamColor = gDrawManager.dwGetTeamColor(player.get<int>(gEntVars.iTeam));
	}

	//Draw on the player.

	classId id = player->GetClientClass()->iClassID;

	if(id == classId::CTFPlayer)
	{

		// no deads
		if(player.get<BYTE>(gEntVars.iLifeState) != LIFE_ALIVE)
			return false;

		player_info_t info;
		if(!gInts.Engine->GetPlayerInfo(index, &info))
			return false;

		if(renderBox)
		{
			//gDrawManager.OutlineRect(top.x - flWidth, top.y, flWidth * 2, flHeight, teamColor); // player box.
			//FrameHitbox(player, 0);
			DynamicBox(player, info.name, teamColor);
		}

		if(renderGUID)
		{
			gDrawManager.DrawString("esp", vecScreen.x, vecScreen.y, COLOR_OBJ, XorString("%s"), info.guid);
			vecScreen.y += gDrawManager.GetESPHeight();
		}

		if(renderName)
		{
			gDrawManager.DrawString("esp", vecScreen.x, vecScreen.y, teamColor, XorString("%s"), info.name);
			vecScreen.y += gDrawManager.GetESPHeight();
		}

		if(renderHealth)
		{
			gDrawManager.DrawString("esp", vecScreen.x, vecScreen.y, teamColor, XorString("%i"), player.get<int>(gEntVars.iHealth) /*gInts.GameResource->getHealth(index)*/); //Draw on the player.
			vecScreen.y += gDrawManager.GetESPHeight();
		}

		if(renderIndex)
		{
			gDrawManager.DrawString("esp", vecScreen.x, vecScreen.y, teamColor, XorString("%i"), player.index()); //Draw on the player.
			vecScreen.y += gDrawManager.GetESPHeight();
		}

		if(renderViewESP)
		{

			Vector angles = player->GetPrevLocalAngles();
			//gDrawManager.DrawString("esp", vecScreen.x, vecScreen.y, COLOR_OBJ, "%f, %f, %f", angles.x, angles.y, angles.z);
			//vecScreen.y += gDrawManager.GetESPHeight();
			Vector forward;
			AngleVectors(angles, &forward);
			Vector eyepos = player->GetAbsOrigin() + player.get<Vector>(gEntVars.vecViewOffset);
			forward = forward * viewESPLength + eyepos;

			// we cant use the '1 tick' trick here as in paintTraverse, we are run multiple times per tick!
			// we should use debugoverlay here for speed reasons, but also so that it doesnt show through walls
			gInts.DebugOverlay->AddLineOverlay(eyepos, forward, RED(teamColor), GREEN(teamColor), BLUE(teamColor), false, 1);

			//Vector screenForward, screenEyepos;
			//if(gDrawManager.WorldToScreen(eyepos, screenEyepos) && gDrawManager.WorldToScreen(forward, screenForward))
			//{
			//	gDrawManager.drawLine(screenForward.x, screenForward.y, screenEyepos.x, screenEyepos.y, teamColor);
			//}
		}
	}
	else if(id == classId::CObjectDispenser || id == classId::CObjectSapper || id == classId::CObjectSentrygun || id == classId::CObjectTeleporter)
	{
		// just draw the name now

		if(renderObjectID)
		{
			gDrawManager.DrawString("esp", vecScreen.x, vecScreen.y, teamColor, XorString("%s"), player->GetClientClass()->chName);
			vecScreen.y += gDrawManager.GetESPHeight();
		}
	}
	return true;
}

void CESP::FrameHitbox(CEntity<> player, int iHitbox)
{
	const PDWORD pModel = player->GetModel();
	if(!pModel)
		return;

	DWORD *pStudioHdr = gInts.ModelInfo->GetStudiomodel(pModel);
	if(!pStudioHdr)
		return;

	/*mstudiohitboxset_t* pSet = pStudioHdr->pHitboxSet(pPlayer->GetHitboxSet());
	if(!pSet)
		return;

	mstudiobbox_t* pBox = pSet->pHitbox(iHitbox);
	if(!pBox)
		return;*/

	int HitboxSetIndex = *(int *)((DWORD)pStudioHdr + 0xB0);
	mstudiohitboxset_t *pSet = (mstudiohitboxset_t *)(((PBYTE)pStudioHdr) + HitboxSetIndex);

	auto pBox =  (mstudiobbox_t *)pSet->pHitbox(iHitbox);

	matrix3x4 vMatrix[128];

	if(!player->SetupBones(vMatrix, 128, 0x100, gInts.Globals->curtime))
		return;

	Vector vMin = pBox->bbmin;
	Vector vMax = pBox->bbmax;

	Vector vPointList[] =
	{
		Vector(vMin.x, vMin.y, vMin.z),
		Vector(vMin.x, vMax.y, vMin.z),
		Vector(vMax.x, vMax.y, vMin.z),
		Vector(vMax.x, vMin.y, vMin.z),
		Vector(vMax.x, vMax.y, vMax.z),
		Vector(vMin.x, vMax.y, vMax.z),
		Vector(vMin.x, vMin.y, vMax.z),
		Vector(vMax.x, vMin.y, vMax.z)
	};

	Vector vTransformed[8];

	for(int i = 0; i < 8; i++)
		VectorTransform(vPointList[i], vMatrix[pBox->bone], vTransformed[i]);

	gDrawManager.DrawBox(vTransformed, COLOR_OBJ);
}

void CESP::DynamicBox(CEntity<> ent, const char* szName, DWORD dwColor)
{
	const matrix3x4& trans = *reinterpret_cast<matrix3x4*>(ent.getPtr<DWORD>(gEntVars.rgflCoordinateFrame));

	Vector min = ent.get<Vector>(gEntVars.collision + 0x8);
	Vector max = ent.get<Vector>(gEntVars.collision + 0x14);

	Vector pointList[] = {
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	Vector transformed[8];

	for(int i = 0; i < 8; i++)
		VectorTransform(pointList[i], trans, transformed[i]);

	Vector flb, brt, blb, frt, frb, brb, blt, flt;

	if(!gDrawManager.WorldToScreen(transformed[3], flb) ||
		!gDrawManager.WorldToScreen(transformed[0], blb) ||
		!gDrawManager.WorldToScreen(transformed[2], frb) ||
		!gDrawManager.WorldToScreen(transformed[6], blt) ||
		!gDrawManager.WorldToScreen(transformed[5], brt) ||
		!gDrawManager.WorldToScreen(transformed[4], frt) ||
		!gDrawManager.WorldToScreen(transformed[1], brb) ||
		!gDrawManager.WorldToScreen(transformed[7], flt))
		return;

	Vector arr[] = {flb, brt, blb, frt, frb, brb, blt, flt};

	float left = flb.x;
	float top = flb.y;
	float right = flb.x;
	float bottom = flb.y;

	for(int i = 0; i < 8; i++)
	{
		if(left > arr[i].x)
			left = arr[i].x;
		if(top < arr[i].y)
			top = arr[i].y;
		if(right < arr[i].x)
			right = arr[i].x;
		if(bottom > arr[i].y)
			bottom = arr[i].y;
	}

	float x = left;
	float y = bottom;
	float w = right - left;
	float h = top - bottom;


	tf_classes Class = ent.get<tf_classes>(gEntVars.iClass);

	gDrawManager.DrawCornerBox(x - 1, y - 1, w + 2, h + 1, 3, 5, COLOR_BLACK);
	gDrawManager.DrawCornerBox(x, y, w, h - 1, 3, 5, dwColor);

	int health = ent.get<int>(gEntVars.iHealth);

	int maxHealth = getMaxHealth(Class);

	if(health > maxHealth)
		health = maxHealth;

	//int r = 255 - health * 2.55;
	//int g = health * 2.55;

	int healthBar = h / maxHealth * health;
	int healthBarDelta = h - healthBar;

	gDrawManager.OutlineRect(x - 5, y - 1, 5, h + 2, COLOR_BLACK);
	gDrawManager.OutlineRect(x - 4, y + healthBarDelta, 3, healthBar, redGreenGradiant(health, maxHealth));
	//-------------------------------------------------------------------------------------------------------------
	FrameHitbox(ent, 0);
}