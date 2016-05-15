#include "Vector.h"

#include "SDK.h"
#include "CEntity.h"

inline void silentMovementFix(CUserCmd *pUserCmd, Vector angles)
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

inline Vector calculatePosition(float time, Vector startPosition, Vector velocity, Vector accel = {0,0,0}) 
{
	return startPosition + (velocity * time) + (accel * 0.5 * pow(time, 2));
}

inline Vector calculateAddition(float time, Vector velocity, Vector accel = {0,0,0})
{
	return (velocity * time) + (accel * 0.5 * pow(time, 2));
}

inline float asymetricInterceptionTime(Vector position1, Vector position2, Vector velocity1, Vector velocity2)
{
	Vector dX = position2 - position1;
	Vector dV = velocity1 - velocity2;
	return (dX / dV).Length();
}

inline float asymetricSingleAcceleratedInterceptionTime(Vector position1, Vector position2, Vector velocity1, Vector velocity2, Vector acceleration2) 
{
    /*
		sqrt( (2 * a2 * x1) + (2 * a2 * x2) + v1^2 - (2 * v1 * v2) + v2^2 ) - v1 + v2
    t = ------------------------------------------------------------------------------
		a2
    */
    if (acceleration2.x == 0 && acceleration2.y == 0 && acceleration2.z == 0) {
        return asymetricInterceptionTime(position1, position2, velocity1, velocity2);
    }
 
    return (sqrt(
        (acceleration2 * position1 * 2).Length()
        + (acceleration2 * position2 * 2).Length()
        + pow(velocity1.Length(), 2)
        - (velocity1 * velocity2 * 2).Length()
        + pow(velocity2.Length(), 2))
        - velocity1.Length()
        + velocity2.Length()
        ) / (acceleration2).Length();
}

inline float asymetricAcceleratedInterceptionTime(Vector position1, Vector position2, Vector velocity1, Vector velocity2, Vector acceleration1, Vector acceleration2) 
{
	/*
		sqrt( (-2 * a1 * x1) + (2 * a1 * x2) + (2 * a2 * x1) - (2 * a2 * x2) + v1^2 - (2 * v1 * v2) + v2^2 ) - v1 + v2
	t = ----------------------------------------------------------------------------------------------------------------
		a1 - a2
	*/
    Vector nullvec(0, 0, 0);
 
    if (acceleration2 == nullvec && acceleration1 != nullvec) 
	{
        return asymetricSingleAcceleratedInterceptionTime(position1, position2, velocity1, velocity2, acceleration1);
    }
    else if (acceleration2 != nullvec && acceleration1 == nullvec) 
	{
        return asymetricSingleAcceleratedInterceptionTime(position1, position2, velocity1, velocity2, acceleration2);
    }
 
    return (sqrt(
        -(acceleration1 * position1 * 2).Length()
        + (acceleration1 * position2 * 2).Length()
        + (acceleration2 * position1 * 2).Length()
        - (acceleration2 * position2 * 2).Length()
        + pow(velocity1.Length(), 2)
        - (velocity1 * velocity2 * 2).Length()
        + pow(velocity2.Length(), 2))
        - velocity1.Length()
        + velocity2.Length()
        ) / (acceleration1 - acceleration2).Length();
}

class CBaseAimSystem
{
public:
	virtual void aim(CUserCmd *pUserCmd, Vector v) { return; }
};

class CSnapAimSystem : public CBaseAimSystem
{
public:
	virtual void aim(CUserCmd *pUserCmd, Vector v, bool silentMoveFix)
	{
		Vector angles;
		VectorAngles(v - (gLocalPlayerVars.pred.origin + CEntity<>{me}.get<Vector>(gEntVars.vecViewOffset)), angles);
		ClampAngle(angles);
		if(silentMoveFix)
			silentMovementFix(pUserCmd, angles);
		pUserCmd->viewangles = angles;
		return;
	}
};
