#pragma once

#include "IHack.h"
#include "SDK.h"
//#include "Trace.h"
#include "CUtlVector.h"

#include <unordered_map>

// straight from volvo's sdk
struct GlowObjectDefinition_t
{
	CHandle hEntity;
	float r; // Vector m_vGlowColor;
	float g; 
	float b;
	float a;

	bool m_bRenderWhenOccluded;
	bool m_bRenderWhenUnoccluded;
	int m_nSplitScreenSlot;

	// Linked list of free slots
	int m_nNextFreeSlot;

	// Special values for GlowObjectDefinition_t::m_nNextFreeSlot
	static const int END_OF_FREE_LIST = -1;
	static const int ENTRY_IN_USE	 = -2;
};

class CGlowManager
{
public:
	// unsigned char padding[ 0x004 ];
	// CGlowObject glowObjects[ 1 ];

	CUtlVector<GlowObjectDefinition_t> glowObjects;
	int m_nFirstFreeSlot;

	int registerGlowObject(CBaseEntity *ent, float r, float g, float b, float a, bool bRenderWhenOccluded, bool bRenderWhenUnoccluded, int nSplitScreenSlot);
	void unregisterGlowObject(int glowIndex);
};

class CGlow : public IHack
{
	// the pointer to our glow object manager
	CGlowManager *pGlowObjectManger;

	bool enabled;
	bool enemyOnly;
	bool teamColor;

	var enabled_bool{"Enabled", &enabled};
	var enemy_bool{"Enemy only", &enemyOnly};
	var team_color_bool{"Team Colors?", &teamColor};

	// int s = sizeof(GlowObjectDefinition_t);

	// maps entity index to whether the glow object has been created
	std::unordered_map<int, int> glowObjects;

public:
	CGlow()
	{
		variables.push_back(enabled_bool);
		variables.push_back(enemy_bool);
		variables.push_back(team_color_bool);
	}

	const char *name() const override;
	bool init() override;
	bool paint() override;
	bool inEntityLoop(int index) override;
};
