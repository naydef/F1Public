#include "CTrigger.h"
#include "SDK.h"
#include "CEntity.h"
#include "Trace.h"
#include "CDrawManager.h"
#include "bspflags.h"

const char *CTrigger::name() const { return "TRIGGERBOT"; }

bool CTrigger::paint()
{
	// int y = 500;
	// gDrawManager.DrawString( "hud", 0, y, 0xFFFFFFFF, "   Angles Vector: %f %f %f", curr_angles.x, curr_angles.y, curr_angles.z );
	// y += gDrawManager.GetHudHeight( );
	// gDrawManager.DrawString( "hud", 0, y, 0xFFFFFFFF, "Direction Vector: %f %f %f", curr_direction.x, curr_direction.y, curr_direction.z );
	// y += gDrawManager.GetHudHeight( );
	// gDrawManager.DrawString( "hud", 0, y, 0xFFFFFFFF, "       Ray Delta: %f %f %f", curr_ray.x, curr_ray.y, curr_ray.z );
	// y += gDrawManager.GetHudHeight( );
	// gDrawManager.DrawString( "hud", 0, y, 0xFFFFFFFF, "    Cur Position: %f %f %f", curr_pos.x, curr_pos.y, curr_pos.z );
	// y += gDrawManager.GetHudHeight( );
	// gDrawManager.DrawString( "hud", 0, y, 0xFFFFFFFF, "    End Position: %f %f %f", curr_endpos.x, curr_endpos.y, curr_endpos.z );
	// y += gDrawManager.GetHudHeight( );
	// gDrawManager.DrawString( "hud", 0, y, 0xFFFFFFFF, "  box|group|bone: %i %i %i", curr_hitbox, curr_hitgroup, curr_physicsbone );
	// y += gDrawManager.GetHudHeight( );
	// gDrawManager.DrawString( "hud", 0, y, 0xFFFFFFFF, "   ClientClassId: %i", curr_class_id );
	// y += gDrawManager.GetHudHeight();
	// gDrawManager.DrawString( "hud", 0, y, 0xFFFFFFFF, "         IsNull?: %s", curr_ent_null ? "true" : "false" );
	// y += gDrawManager.GetHudHeight();

	return false;
}

bool CTrigger::move(CUserCmd *pUserCmd)
{
	if(!enabled)
		return false;

	CEntity<> local{me};

	if(local.isNull())
		return false;

	if(zoomedOnly)
	{
		if(gLocalPlayerVars.Class._to_integral() == tf_classes::TF2_Sniper)
		{
			if(!(gLocalPlayerVars.cond & tf_cond::TFCond_Zoomed))
			{
				return false;
			}
		}
	}

	Ray_t ray;
	trace_t trace;
	Vector eyePos = local->GetAbsOrigin() + local.get<Vector>(gEntVars.vecViewOffset);

	Vector forward;
	AngleVectors(pUserCmd->viewangles, &forward);
	forward = forward * 8192.0f + eyePos;

	ray.Init(eyePos, forward);

	// CTraceFilter filter{ local.castToPointer<CBaseEntity>( ) };

	CBaseFilter filt;

	filt.SetIgnoreEntity(local.castToPointer<CBaseEntity>());

	gInts.EngineTrace->TraceRay(ray, MASK_AIMBOT | CONTENTS_HITBOX, &filt, &trace); // 0x4200400B

	if(!trace.m_pEnt)
		return false;

	if(!hitAll)
	{
		if(useHitbox)
		{
			if(trace.hitbox != hitbox)
				return false;
		}
		else
		{
			if(trace.hitGroup != hitgroup)
				return false;
		}
	}
	else
	{
		if(trace.hitGroup == 0)
			return false;
	}

	CEntity<> other{trace.m_pEnt->GetIndex()};

	// if ( other->GetClientClass( )->iClassID != static_cast<int>( classId::CTFPlayer ) )
	//	return false;

	if(other->IsDormant())
		return false;

	if(other.get<BYTE>(gEntVars.iLifeState) != LIFE_ALIVE)
		return false;

	if(other.get<int>(gEntVars.iTeam) == gLocalPlayerVars.team)
		return false;

	pUserCmd->buttons |= IN_ATTACK;

	return true;
}
