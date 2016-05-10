#pragma once

#include <unordered_map>

#include "baseHeaders.h"

#include "CEntity.h"

namespace __CTargetHelper
{
	// makes me ill
	inline Vector CalcAngle(Vector PlayerPos, Vector EnemyPos)
	{
		Vector AimAngles;
		Vector delta = PlayerPos - EnemyPos;
		float hyp = sqrtf((delta.x * delta.x) + (delta.y * delta.y)); //SUPER SECRET IMPROVEMENT CODE NAME DONUT STEEL
		AimAngles.x = atanf(delta.z / hyp) * RADPI;
		AimAngles.y = atanf(delta.y / delta.x) * RADPI;
		AimAngles.z = 0.0f;
		if(delta.x >= 0.0)
			AimAngles.y += 180.0f;

		return AimAngles;
	}

	inline void MakeVector(Vector angle, Vector& vector)
	{
		float pitch = float(angle[0] * PI / 180);
		float yaw = float(angle[1] * PI / 180);
		float tmp = float(cos(pitch));
		vector[0] = float(-tmp * -cos(yaw));
		vector[1] = float(sin(yaw)*tmp);
		vector[2] = float(-sin(pitch));
	}

	inline float GetFov(Vector angle, Vector src, Vector dst)
	{
		Vector ang, aim;
		ang = CalcAngle(src, dst);

		MakeVector(angle, aim);
		MakeVector(ang, ang);

		float mag = sqrtf(pow(aim.x, 2) + pow(aim.y, 2) + pow(aim.z, 2));
		float u_dot_v = aim.Dot(ang);

		return RAD2DEG(acos(u_dot_v / (pow(mag, 2))));
	}

	inline float getDistanceToVector(Vector v)
	{
		CBaseEntity *pLocal = gInts.EntList->GetClientEntity(me);

		if(!pLocal)
			return 8192.0f;

		Vector delta = pLocal->GetAbsOrigin() - v;

		float dist = sqrtf(delta.Length());

		if(dist < 1.0f)
			return 1.0f;

		return dist;
	}
}

struct CTarget
{
	int ent;
	Vector target;
};

//enum class targetHelpers
//{
//	distance,
//	fov,
//};

BETTER_ENUM(targetHelpers, int, Distance, FOV)

// allows the user to get the best target of that system without needing to know the impl
class IBaseTargetSystem
{
public:
	virtual int getBestTarget() = 0;

	virtual void removeTarget(int index) = 0;

	virtual void addTarget(CTarget index) = 0;

	virtual float getBestStat() = 0;

	virtual targetHelpers getType() = 0;
};

// base class for target system
class CBaseTargetSystem : public IBaseTargetSystem
{
public:
	virtual int getBestTarget() override { return -1; }
};

// target system based on distance
// gets the closest target
class CDistanceTargetSystem : public CBaseTargetSystem
{
	using target = std::pair<int, float>;

	std::unordered_map<int, float> targets;

	int lastBestTarget;

public:
	CDistanceTargetSystem() { targets[-1] = 8192.0f; }

	int getBestTarget() override
	{
		target bestTarg(-1, 8192.f);

		for(auto &targ : targets)
		{
			if(targ.second < bestTarg.second)
			{
				bestTarg		 = targ;
				lastBestTarget = bestTarg.first;
			}
		}

		return bestTarg.first;
	}

	// doesnt recalculate best target
	float getBestStat() override { return targets[lastBestTarget]; }

	void addTarget(CTarget targ)
	{
		// add it to tge list
		targets[targ.ent] = __CTargetHelper::getDistanceToVector(targ.target);
	}

	void removeTarget(int index) override { targets.erase(index); }

	targetHelpers getType() override { return targetHelpers::Distance; };
};

class CFovTargetSystem : public CBaseTargetSystem
{

	using target = std::pair<int, float>;

	std::unordered_map<int, float> targets;

	int lastBestTarget;

public:
	CFovTargetSystem() { targets[-1] = 8192.0f; };

	int getBestTarget() override
	{
		target bestTarg = {-1, 8192.0f};

		for(auto &targ : targets)
		{
			if(targ.second < bestTarg.second)
			{
				bestTarg = targ;
				lastBestTarget = bestTarg.first;
			}
		}
		return bestTarg.first;
	}

	float getBestStat() override { return targets[lastBestTarget]; }

	void addTarget(CTarget index) override
	{
		CEntity<> local{me};

		float fov = __CTargetHelper::GetFov(local->GetAbsAngles(), local->GetAbsOrigin() + gLocalPlayerVars.viewOffset, index.target);
		//Log::Console("Adding Target %d with fov %f", index, fov);
		targets[index.ent] = fov;
	}

	void removeTarget(int index) { targets.erase(index); }

	targetHelpers getType() override { return targetHelpers::FOV; };
};

inline IBaseTargetSystem *switchTargetSystem(IBaseTargetSystem *oldPtr, targetHelpers newType)
{
	if(oldPtr)
		delete oldPtr;

	switch(newType)
	{
	case targetHelpers::Distance:
		return new CDistanceTargetSystem();
	case targetHelpers::FOV:
		return new CFovTargetSystem();
	default:
		Log::Debug("INVALID TARGET SYSTEM ID: %d", newType._to_integral());
		return oldPtr;
		break;
	}
}

