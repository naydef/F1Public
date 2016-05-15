#include "CAimbot.h"
#include "SDK.h"
#include "CEntity.h"
#include "trace.h"
#include "Util.h"
#include "CDrawManager.h"
#include "CAimHelper.h"

// simple define for sniper zoom
#define ZOOM_BASE_DAMAGE 150.0f
#define SNIPERRIFLE_BASE_DAMANGE 50.0f

const char *CAimbot::name() const { return "AIMBOT"; }

bool CAimbot::move(CUserCmd *pUserCmd)
{
	if(enabled)
	{

		oldAngles = pUserCmd->viewangles;

		// this does the silent movement fix
		aim(pUserCmd);

		return true;
	}
	return false;
}

bool CAimbot::paint()
{
	if(!enabled || !projectilePrediction)
		return false;

	// draw the current fov
	if(tSystem == targetHelpers::FOV)
		gDrawManager.DrawCircle(gScreenSize.iScreenWidth / 2, gScreenSize.iScreenHeight / 2, fovLimit, 10, COLOR_LINE);

	if(!projectilePrediction)
		return false;

	int y = 500;
	gDrawManager.DrawString("hud", 0, y, COLOR_OBJ, "time: %f", time);
	y += gDrawManager.GetHudHeight();
	gDrawManager.DrawString("hud", 0, y, COLOR_OBJ, "asymetric time: %f", asymTime);
	y += gDrawManager.GetHudHeight();
	gDrawManager.DrawString("hud", 0, y, COLOR_OBJ, "speed: %f", speed);
	y += gDrawManager.GetHudHeight();
	gDrawManager.DrawString("hud", 0, y, COLOR_OBJ, "dist: %f", distance);
	y += gDrawManager.GetHudHeight();

	return true;
}

// update the entitys stats in the entity loop
bool CAimbot::inEntityLoop(int index)
{
	CEntity<> ent{index};

	// if its not a player
	//if(!isPlayer(ent))
	//{
	//	// TODO add world check here
	//	// we need to make sure we dont aim at entities that are not players
	//	// so remove it from the map
	//	//stats.erase(index);
	//	targetSystem.removeTarget(index);
	//	return false;
	//}

	// if its not valid then its not valid
	if(!isValidTarget(ent))
	{
		this->targetSystem->removeTarget(index);
		return false;
	}

	targetSystem->addTarget({index, getHitBoxVector(ent)});


	//if(stats[index] == 8192.0f)
	//{
	//	stats.erase(index);
	//	return false;
	//}

	if(!isValidBuilding(ent))
	{
		if(!checkCond(ent))
		{
			targetSystem->removeTarget(index);
			return false;
		}
		if(ignoreFriends)
		{
			// we dont want to hurt friends
			if(isPlayerOnFriendsList(index))
			{
				targetSystem->removeTarget(index);
				return false;
			}
		}
	}

	//projectileHelper::PredictPath(index);

	return true;
}

//float CAimbot::getDistance(CEntity<> &ent)
//{
//
//	if(ent.isNull())
//		return 8192.0f;
//
//	return getDistanceToVector(ent->GetAbsOrigin());
//}
//
//inline float CAimbot::getDistanceToVector(Vector v)
//{
//	CBaseEntity *pLocal = gInts.EntList->GetClientEntity(me);
//
//	if(!pLocal)
//		return 8192.0f;
//
//	Vector delta = pLocal->GetAbsOrigin() - v;
//
//	float dist = sqrtf(delta.Length());
//
//	if(dist < 1.0f)
//		return 1.0f;
//
//	return dist;
//}

bool CAimbot::visible(CEntity<> &ent)
{
	trace_t trace;
	Ray_t ray;
	CBaseFilter filter;

	CEntity<> local{me};

	filter.SetIgnoreEntity(local.castToPointer<CBaseEntity>());

	// the get hitbox already attempts to predict it ( if that is enabled )
	Vector hit = getHitBoxVector(ent);

	// use the predicted origin the view offset is insignificant
	ray.Init(gLocalPlayerVars.pred.origin + local.get<Vector>(gEntVars.vecViewOffset), hit);

	gInts.EngineTrace->TraceRay(ray, MASK_AIMBOT | CONTENTS_HITBOX, &filter, &trace);

	if(trace.m_pEnt)
	{
		return (trace.m_pEnt == ent.castToPointer<CBaseEntity>() && trace.hitGroup != 0);
	}

	return true;
}

void CAimbot::aim(CUserCmd *pUserCmd)
{
	Vector angles;

	if(targetSystem->getType() != targetHelpers::_from_integral(tSystem))
	{
		targetSystem = switchTargetSystem(targetSystem, targetHelpers::_from_integral(tSystem));
	}

	int target = targetSystem->getBestTarget();

	if(target == -1)
	{
		valid = false;
		return;
	}

	switch(targetHelpers::_from_integral(tSystem))
	{
	case targetHelpers::Distance:
		if(targetSystem->getBestStat() == 8192.0f)
			return;
	case targetHelpers::FOV:
		if(targetSystem->getBestStat() > fovLimit)
			return;
	default:
		break;
	}

	//Log::Console("Target is index %d", target);

	CEntity<> local{me};
	CEntity<> other{target};

	CEntity<> localWeapon{HANDLE2INDEX(local.get<int>(gEntVars.hActiveWeapon))};

	if(local.get<BYTE>(gEntVars.iLifeState) != LIFE_ALIVE)
		return;

	// if we manually attack, ALWAYS aim and attack
	if(!(pUserCmd->buttons & IN_ATTACK))
	{

		if(aimOnClick)
			return;

		if(gLocalPlayerVars.Class._to_integral() == tf_classes::TF2_Sniper)
		{
			if(gLocalPlayerVars.cond & tf_cond::TFCond_Zoomed)
			{
				// if we cant get them with the no charge
				if(other.get<int>(gEntVars.iHealth) > SNIPERRIFLE_BASE_DAMANGE)
				{
					float damage = localWeapon.get<float>(gEntVars.flChargedDamage);
					// minium charge to be able to headshot
					if(damage < zoomLowestDamage)
					{
						return;
					}
					// respect zoom damage
					if(respectChargedDamage && ZOOM_BASE_DAMAGE + damage < other.get<int>(gEntVars.iHealth))
					{
						return;
					}
				}

				// otherwise we can just shoot and kill :D
			}
			else
			{
				if(zoomOnly)
				{
					return;
				}
			}
		}
		else if(gLocalPlayerVars.Class._to_integral() == tf_classes::TF2_Spy)
		{
			if(gLocalPlayerVars.activeWeapon == classId::CTFRevolver)
			{
				CTFBaseWeaponGun *tfWeap = localWeapon.castToPointer<CTFBaseWeaponGun>();

				float spread = tfWeap->WeaponGetSpread();

				// Log::Console("%f", spread);

				if(spread > 0)
					return;
			}
			else
			{
				// if we cant fire, dont aim (bullettime is a little backwards)
				if(bulletTime(local, true))
					return;
			}
		}
		else
		{
			// if we cant fire, dont aim (bullettime is a little backwards)
			// if(bulletTime(local, true))
			//	return;
		}
	}

	// we need to call run simulation before even getting the hitbox
	// TODO dont do this
	if(runSimulation)
	{
		CUtilMove::safeRunSimulation(gInts.Prediction, other.castToPointer<CBaseEntity>());
	}

	Vector hit = getHitBoxVector(other);

	// once again use the predicted origin of our local player
	VectorAngles(hit - (gLocalPlayerVars.pred.origin + local.get<Vector>(gEntVars.vecViewOffset)), angles);

	ClampAngle(angles);

	// Vector oldView = pUserCmd->viewangles;

	if(silentFix)
	{
		CAimbot::silentMovementFix(pUserCmd, angles);
	}

	pUserCmd->viewangles = angles;

	if(noSilentAim)
		gInts.Engine->SetViewAngles(angles);
	// else
	//	gInts.Engine->SetViewAngles(oldAngles);

	// also attack
	pUserCmd->buttons |= IN_ATTACK;
	return;
}

bool CAimbot::isValidTarget(CEntity<> &ent)
{
	if(ent.isNull())
		return false;

	if(ent->IsDormant())
		return false;

	//if(!isPlayer(ent))
	//	return false;

	if(ent.index() == me)
		return false;

	if(ent->GetClientClass()->iClassID != classId::CTFPlayer)
	{
		if(!isValidBuilding(ent))
			return false;
	}
	else
	{
		if(ent.get<BYTE>(gEntVars.iLifeState) != LIFE_ALIVE)
			return false;
	}


	if(ent.get<int>(gEntVars.iTeam) == gLocalPlayerVars.team)
		return false;

	if(!visible(ent))
		return false;

	return true;
}

Vector CAimbot::getHitBoxVector(CEntity<> &ent)
{
	Vector vHitbox;

	if(!isValidBuilding(ent))
	{
		PDWORD model = ent->GetModel();

		if(!model)
			return vHitbox;

		PDWORD pStudioHdr = gInts.ModelInfo->GetStudiomodel(model);

		if(!pStudioHdr)
			return vHitbox;

		if(!ent->SetupBones(BoneToWorld, 128, 0x100, 0))
			return vHitbox;

		box = GetHitbox(hitbox, pStudioHdr);

		if(!box)
			return vHitbox;

		VectorTransform(box->bbmin, BoneToWorld[box->bone], Min);
		VectorTransform(box->bbmax, BoneToWorld[box->bone], Max);

		vHitbox = (Min + Max) / 2;

		// this either will predict or just return the same thing back
		Vector pred = predict(ent, vHitbox);

		return pred;

	}
	else
	{
		Vector temp = {0,0,0};
		ent->GetWorldSpaceCenter(temp);
		return temp;
	}
}

inline mstudiobbox_t *CAimbot::GetHitbox(int iHitbox, DWORD *pHeader)
{
	int HitboxSetIndex	   = *(int *)((DWORD)pHeader + 0xB0);
	mstudiohitboxset_t *pSet = (mstudiohitboxset_t *)(((PBYTE)pHeader) + HitboxSetIndex);

	return (mstudiobbox_t *)pSet->pHitbox(iHitbox);
}

inline bool CAimbot::isPlayer(CEntity<> &ent)
{
	if(!ent.isNull())
		return ent->GetClientClass()->iClassID == classId::CTFPlayer;

	return false;
}

//int CAimbot::selectTarget()
//{
//	std::pair<int, float> target;
//
//	// this is the furthest our gun can shoot
//	// therefore it should be reasonable that all other entitys should be closer than this
//	target.second = 8192.0f;
//	target.first  = -1;
//
//	// loop through data
//	for(auto &t : stats)
//	{
//		// if we have already got the lowest distance, stop looping
//		if(target.second == 1.0f)
//			break;
//
//		float currDist = t.second;
//
//		// if priority is enabled
//		if(variables[10].vars[0].bGet())
//		{
//			vecVars &vars = variables[10].vars;
//			// scale the distances by priority
//
//			CEntity<> tar{t.first};
//
//			if(!tar.isNull())
//			{
//				if(!tar->IsDormant())
//				{
//					tf_classes Class = (tf_classes)tar.get<int>(gEntVars.iClass);
//					currDist /= vars[Class].iGet();
//				}
//			}
//		}
//
//		if(currDist < target.second)
//		{
//			target = t;
//		}
//	}
//
//	return target.first;
//}

inline bool CAimbot::checkCond(CEntity<> &ent)
{
	if(!ignoreCond)
		return true;

	int cond = ent.get<int>(gEntVars.iPlayerCond);

	// people we shouldnt hit
	if(cond & tf_cond::TFCond_Cloaked || cond & tf_cond::TFCond_Ubercharged || cond & tf_cond::TFCond_UberchargeFading || cond & tf_cond::TFCond_CloakFlicker ||
	   cond & tf_cond::TFCond_DeadRingered || cond & tf_cond::TFCond_Bonked)
		return false;

	// TODO add netvars for cond ex and ex2

	return true;
}

namespace predictionFuncs
{

	float TimeToImpact(Vector from, Vector to, float flSpeed)
	{
		//float flSpeed = gPrediction.GetProjectileSpeed();

		float flTime = 1.0f;

		float flDistance = 1.0f;

		flDistance = (from - to).Length();

		flTime = flDistance / flSpeed;

		//float flNextPrimaryAttack = *(float*)((DWORD)m_weapon + 0x1660) - g_pGlobalVars->curtime;

		//if(flNextPrimaryAttack  > 0.0f)
		//	flTime += flNextPrimaryAttack;

		return flTime;
	}


	Vector PredictedPos(CBaseEntity *target, Vector pos, float time)
	{
		Vector vel, vpos;
		vel = EstimateAbsVelocity(target);

		vpos.x = pos.x + vel.x * time;
		vpos.y = pos.y + vel.y * time;
		vpos.z = pos.z + vel.z * time;

		/* FIX: this flag is based on CURRENT position, not the predicted position, so they could actually be on the floor when they aren't */
		if(!(CEntity<>{target->GetIndex()}.get<int>(gEntVars.iFlags) & FL_ONGROUND))
			vpos += (Vector{0, 0, -800.0f} * 0.5f * time * time);

		return vpos;
	}

	bool PredictProjectiles(CBaseEntity *target, Vector in, Vector &out, float speed)
	{
		Vector vShootPos, vTargetVelocity, vOutPos = in;
		Vector vAngles;

		float flDelta = 0.01f;
		float flTargetSpeed = 0.0f;
		float flProjTime = 1.0f;
		float flTimeFromPlayerToNewPos = 1.0f;

		Ray_t	ray;
		trace_t tr;

		out = in;

		if(!target)
			return false;

		//if(!ResetFrameData())
		//	return false;

		//if(!Util::HasProjectileLauncher(m_weaponID))
		//	return true;

		//Util::GetLocalEyePos( vShootPos );

		vShootPos = gLocalPlayerVars.pred.origin + gLocalPlayerVars.viewOffset;

		float flProjectileSpeed = speed;

		vTargetVelocity = EstimateAbsVelocity(target);

		flTargetSpeed = vTargetVelocity.Length();

		/* This could cause a crash! */
		if(flTargetSpeed == 0.0f)
			return true;

		float flDeltaOfTimes = 1.0f;

		//int nPoints = 0;

		/* Keep going until we find a point in space that it takes the same time for the projectile to reach as it does for the player, THAT will be the most probable hit location */
		while(flDeltaOfTimes > 0.0f)
		{
			vOutPos = PredictedPos(target, in, flDelta);	// the position at guestimated time x

			flProjTime = TimeToImpact(vShootPos, vOutPos, flProjectileSpeed); // the time it takes for our projectile to reach position predicted in space	

			//if(nPoints == 105)
			//{
			//	m_points.push_back(PredPoint(vOutPos, Colour(20, 20, 220, 255), 0.2f));
			//	nPoints = 0;
			//}

			//nPoints++;

			float flTimeFromPlayerToNewPos = (in - vOutPos).Length() / flTargetSpeed; //the time it takes to get from the current position to the predicted position in space

			flDeltaOfTimes = flProjTime - flTimeFromPlayerToNewPos;

			flDelta *= (1 + gInts.Globals->interval_per_tick);
		}

		//ray.Init(in, vOutPos);

		//CBaseFilter pTrace;

		//pTrace.SetIgnoreSelf(gInts.EntList->GetClientEntity(me));

		//gInts.EngineTrace->TraceRay(ray, MASK_SOLID, &pTrace, &tr);

		// /* If the predicted position intersected with an object, stop the prediction there */
		//if( /*tr.m_pEnt = g_pClientEntList->GetListedEntity( 0 )->GetBaseEntity( )*/tr.contents & CONTENTS_SOLID && tr.fraction != 1.0f)
		//{
		//	vOutPos = tr.end;

		//	if((vShootPos - tr.end).Length() < 400.0f)
		//		return false;

		//	float flDistanceToFloor = (in - tr.end).Length();

		//	float flTimeFromPlayerToFloor = flDistanceToFloor / flTargetSpeed; // how long will it take the player to get there?

		//	flProjTime = TimeToImpact(vShootPos, tr.end, speed); // how long will it take our bullet to get there

		//	//m_points.push_back(PredPoint(vOutPos, Colour(255, 20, 20, 255), 0.2f));

		//	if(fabs(flTimeFromPlayerToFloor - flProjTime) > 0.1f)
		//	{
		//		return false;
		//	}
		//}

		//m_points.push_back(PredPoint(vOutPos, Colour(20, 255, 20, 255), 0.2f));

		out = vOutPos;

		return true;
	}
}

// does all of the possible predictions that are needed
inline Vector CAimbot::predict(CEntity<> &ent, Vector v)
{
	// Vector vel{0.f,0.f,0.f};

	// clear the positions
	//positions.clear();

	if(projectilePrediction)
	{
		float latency = gInts.Engine->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING) + gInts.Engine->GetNetChannelInfo()->GetLatency(FLOW_INCOMING);
		Vector velocity = EstimateAbsVelocity(ent.castToPointer<CBaseEntity>()) * latency;

		currPos = v;

		// prevent devide by 0 errors later on
		speed = 1.0f;

		CEntity<> local{me};
		CEntity<> localwep{HANDLE2INDEX(local.get<int>(gEntVars.hActiveWeapon))};

		if(!localwep.isNull())
		{
			classId wepID = localwep->GetClientClass()->iClassID;

			switch(wepID)
			{
			case classId::CTFRocketLauncher:
			case classId::CTFRocketLauncher_AirStrike:
			case classId::CTFRocketLauncher_Mortar:
				speed = 1100.0f;
				break;
			case classId::CTFRocketLauncher_DirectHit:
				speed = 1980.0f;
				break;
			default:
				break;
			}
		}

		//(pos + (vel * time)) + (1/2 * (grav * (time ^ 2)))


		Vector oldVector = local->GetAbsOrigin() - v;
		distance = oldVector.Length();
		time = 1.0f;

		Vector localPos = gLocalPlayerVars.pred.origin + gLocalPlayerVars.viewOffset;

		if(ent.get<int>(gEntVars.iFlags) & FL_ONGROUND)
		{
			// if they are on the ground then we can use simple methods to predict where they will be

			time = ((distance / speed) * latency) /* * projectileMult*/;
			asymTime = asymetricInterceptionTime(localPos, currPos, {speed, 0, 0},{0,0,0});

			//if(time == otherTime)
			//	Log::Console("TIMES ARE EQUAL!");
			//v += (velocity * time);

			v += calculatePosition(asymTime, v, velocity);
		}
		else
		{
			// TODO use sv_gravity instead of hardcoding this
			Vector grav = {0, 0, -800};

			// local position, target position, local velocity, target velocity, target gravity.
			time = asymetricSingleAcceleratedInterceptionTime(localPos, v, {speed, 0, 0}, velocity, grav) * latency;

			v += grav * powf(time, 2) * 0.5;

			v += velocity * time;
		}

		//Vector newV;

		//predictionFuncs::PredictProjectiles(ent.castToPointer<CBaseEntity>(), v, newV, speed);

		//v += projectileHelper::PredictCorrection(localwep.castToPointer<CBaseCombatWeapon>(), ent, currPos);

		//v = newV;

		// if they are not on the ground
		// v -= (0.5 * (grav * (powf(time, 2))) * gInts.Globals->frametime);

		if(velocityPrediction)
		{
			// scale the velocity by the latency
			// vel *= latency;
		}

		// no prediction is enabled just return the input

		// v += vel;
	}
	predPos = v;
	return v;
}

inline void CAimbot::silentMovementFix(CUserCmd *pUserCmd, Vector angles)
{
	Vector vecSilent(pUserCmd->forwardmove, pUserCmd->sidemove, pUserCmd->upmove);
	float flSpeed = sqrt(vecSilent.x * vecSilent.x + vecSilent.y * vecSilent.y);
	Vector angMove;
	VectorAngles(vecSilent, angMove);
	float flYaw			= DEG2RAD(angles.y - pUserCmd->viewangles.y + angMove.y);
	pUserCmd->forwardmove = cos(flYaw) * flSpeed;
	pUserCmd->sidemove	= sin(flYaw) * flSpeed;
	// pUserCmd->viewangles = angles;
}

bool CAimbot::isValidBuilding(CEntity<> &ent)
{

	ClientClass *pClass = ent->GetClientClass();

	if(pClass)
	{
		switch(ent->GetClientClass()->iClassID)
		{
		case classId::CObjectTeleporter:
		case classId::CObjectSentrygun:
		case classId::CObjectDispenser:
			return true;
		default:
			return false;
		}
	}

	return false;
}