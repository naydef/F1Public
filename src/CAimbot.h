#pragma once

#include "IHack.h"
#include "Vector.h"
#include <unordered_map>
#include "CEntity.h"
#include "CTargetHelper.h"

class CBaseEntity;

typedef unsigned long DWORD;
struct mstudiobbox_t;
typedef float matrix3x4[3][4];

class CAimbot : public IHack
{

	bool enabled;
	int hitbox;
	bool zoomOnly;
	bool respectChargedDamage;
	float zoomLowestDamage;
	bool ignoreCond;
	bool ignoreFriends;
	bool velocityPrediction;
	bool aimOnClick;
	bool silentFix;
	bool projectilePrediction;
	bool noSilentAim;
	bool runSimulation;
	int tSystem;
	int fovLimit;
	bool classPriorityEnabled;

	vecVars priority_list;

	var enabled_bool{"Enabled", &enabled};
	var hitbox_int{"Hitbox Number", &hitbox, 0, 17, 0, 1};
	var zoomed_bool{"Zoomed Only", &zoomOnly};
	var charged_bool{"Charged damage", &respectChargedDamage};
	var zoom_low_float{"Lowest damage", &zoomLowestDamage, 0.0f, 100.0f, 15.0f, 5.0f};
	var cond_bool{"Ignore Cond Players", &ignoreCond};
	var friend_bool{"Ignore Friends", &ignoreFriends};
	var vel_pred_bool{"Velocity Pred", &velocityPrediction};
	var click_bool{"Aim on click", &aimOnClick};
	var silent_bool{"Silent Movement fix", &silentFix};
	var proj_bool{"Projectile Prediction", &projectilePrediction};
	// var proj_mult_float{"projMult", type_t::Float);
	var noSilent_bool{"No Silent", &noSilentAim};
	var runSim_bool{"Run Simulation", &runSimulation};
	var target_int{"Target System", &tSystem, 0, 1, 0, 1};
	var fov_int{"Max fov", &fovLimit, 0, 180, 60, 5};

	// vars for priority
	var priority_list_switch{"Priority", &priority_list};

	int classPriorities[9];

	var enable_pri{"Enable priority", &classPriorityEnabled};
	var scout_pri{"Scout", &classPriorities[0], 1, 10, 1, 1};
	var sniper_pri{"Sniper", &classPriorities[1], 1, 10, 1, 1};
	var soldier_pri{"Soldier", &classPriorities[2], 1, 10, 1, 1};
	var demo_pri{"Demoman", &classPriorities[3], 1, 10, 1, 1};
	var medic_pri{"Medic", &classPriorities[4], 1, 10, 1, 1};
	var heavy_pri{"Heavy", &classPriorities[5], 1, 10, 1, 1};
	var pyro_pri{"Pyro", &classPriorities[6], 1, 10, 1, 1};
	var spy_pri{"Spy", &classPriorities[7], 1, 10, 1, 1};
	var engi_pri{"Engi", &classPriorities[8], 1, 10, 1, 1};

	matrix3x4 BoneToWorld[128];
	Vector Min, Max;
	mstudiobbox_t *box;
	bool valid;
	Vector predPos, currPos;
	float speed, asymTime, time, distance;
	Vector oldAngles;

	// std::vector<Point> positions;

	// maps entitys to their distance
	// if an entity is on the list it has to both be valid and visible

	// std::unordered_map<int, float> stats;

	//CDistanceTargetSystem targetSystem;

	IBaseTargetSystem *targetSystem = nullptr;

public:
	CAimbot()
	{
		// init the prioritys
		// scout_pri.init(1, 10, 1, 1);
		// sniper_pri.init(1, 10, 1, 1);
		// soldier_pri.init(1, 10, 1, 1);
		// demo_pri.init(1, 10, 1, 1);
		// medic_pri.init(1, 10, 1, 1);
		// heavy_pri.init(1, 10, 1, 1);
		// pyro_pri.init(1, 10, 1, 1);
		// spy_pri.init(1, 10, 1, 1);
		// engi_pri.init(1, 10, 1, 1);

		variables.push_back(enabled_bool);
		variables.push_back(hitbox_int);
		variables.push_back(zoomed_bool);
		variables.push_back(charged_bool);
		variables.push_back(zoom_low_float);
		variables.push_back(cond_bool);
		variables.push_back(friend_bool);
		variables.push_back(vel_pred_bool);
		variables.push_back(click_bool);
		variables.push_back(silent_bool);
		// add the priority ones
		priority_list.push_back(enable_pri);
		priority_list.push_back(scout_pri);
		priority_list.push_back(sniper_pri);
		priority_list.push_back(soldier_pri);
		priority_list.push_back(demo_pri);
		priority_list.push_back(medic_pri);
		priority_list.push_back(heavy_pri);
		priority_list.push_back(pyro_pri);
		priority_list.push_back(spy_pri);
		priority_list.push_back(engi_pri);

		// always set up the parent
		priority_list.parent = &variables;
		variables.push_back(priority_list_switch);
		variables.push_back(proj_bool);
		// variables.push_back(proj_mult_float);
		variables.push_back(noSilent_bool);
		variables.push_back(runSim_bool);
		// variables.push_back(proj_iter_int);
		variables.push_back(target_int);
		variables.push_back(fov_int);

		valid   = false;
		predPos = currPos = {0.0f, 0.0f, 0.0f};

		targetSystem = new CDistanceTargetSystem();

		// extrapPos = new Vector[maxExtrap];
	}

	// Inherited via IHack
	virtual const char *name() const override;
	virtual bool move(CUserCmd *) override;
	virtual bool paint() override;
	virtual bool inEntityLoop(int index) override;

private:
	// inline float getDistance(CEntity<> &ent);

	// inline float getDistanceToVector(Vector v);

	inline bool visible(CEntity<> &ent);

	inline void aim(CUserCmd *pUserCmd);

	inline bool isValidTarget(CEntity<> &ent);

	// this assumes the index is the target
	inline Vector getHitBoxVector(CEntity<> &ent);

	inline mstudiobbox_t *GetHitbox(int iHitbox, DWORD *pHeader);

	inline bool isPlayer(CEntity<> &ent);

	// inline int selectTarget();

	inline bool checkCond(CEntity<> &ent);

	// takes a vector and entity and scales the vector by the entitys velocity
	inline Vector predict(CEntity<> &ent, Vector v);

	inline void silentMovementFix(CUserCmd *pUserCmd, Vector angles);

	inline bool isValidBuilding(CEntity<> &ent);
};
