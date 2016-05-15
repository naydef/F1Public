#pragma once

//#include "Trace.h"

#include "IHack.h"
#include "Vector.h"

class CTrigger : public IHack
{

	bool enabled, hitAll, useHitbox, zoomedOnly;
	int hitbox, hitgroup;

	var enabled_bool{"Enabled", &enabled};
	var hitAll_bool{"Hit all", &hitAll};
	var hitbox_bool{"Hitbox?", &useHitbox};
	var hitbox_int{"Hitbox Number", &hitgroup, 0, 17, 1, 0};
	var hitgroup_int{"Hitgroup Number", &hitgroup, 0, 7, 1, 0};
	var zoomed_bool{"Zoomed Only", &zoomedOnly};

public:
	CTrigger()
	{
		variables.push_back(enabled_bool);
		variables.push_back(hitAll_bool);
		variables.push_back(hitbox_bool);
		variables.push_back(hitbox_int);
		variables.push_back(hitgroup_int);
		variables.push_back(zoomed_bool);
	}

	const char *name() const override;
	bool paint() override;
	bool move(CUserCmd *) override;

private:
	// Vector curr_angles, curr_direction, curr_ray, curr_endpos, curr_pos;

	// int curr_hitbox, curr_hitgroup, curr_physicsbone, curr_class_id;

	// bool curr_ent_null;
};
