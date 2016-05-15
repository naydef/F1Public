#pragma once

#include "IHack.h"
#include "CEntity.h"

class CBaseCombatWeapon;

class CBackstab : public IHack
{
	bool enabled;
	var enabled_bool{"Enabled", &enabled};

public:
	CBackstab()
	{
		variables.push_back(enabled_bool);
	}

	const char *name() const override;
	bool move(CUserCmd *pCommand) override;

private:
	// internal
	// replaces netvar
	bool canBackstab(CEntity<CBaseCombatWeapon> &weap_entity, CEntity<> &local_entity);

	bool isBehind(CEntity<> &other_entity, CEntity<> &local_entity);

	bool engineCanBackstab(CBaseCombatWeapon *weapon, CBaseEntity *target);

	bool predicts(CEntity<> &local, CEntity<> &other);
};
