#pragma once

#include "IHack.h"

#include "CEntity.h"

class CESP : public IHack
{

	bool enabled;
	bool renderBox;
	bool renderGUID;
	bool renderName;
	bool renderHealth;
	bool renderFriends;
	bool renderIndex;
	bool renderObjectID;
	bool renderViewESP;
	float viewESPLength;

	var enabled_bool{"Enabled", &enabled};
	var box_bool{"ESP Box", &renderBox};
	var guid_bool{"GUID", &renderGUID};
	var name_bool{"Name", &renderName};
	var health_bool{"Health", &renderHealth};
	var friend_bool{"Friends?", &renderFriends};
	var index_bool{"Index", &renderIndex};
	var id_bool{"Object ID", &renderObjectID};
	var view_bool{"View ESP", &renderViewESP};
	var view_float{"View Length", &viewESPLength, 100.0f, 5000.0f, 500.0f, 100.0f};

	// creds to Roskonix for these nice esp boxes
	void DynamicBox(CEntity<> ent, const char* szName, DWORD dwColor);

	void FrameHitbox(CEntity<> player, int iHitbox);

public:
	CESP()
	{
		variables.push_back(enabled_bool);
		variables.push_back(box_bool);
		variables.push_back(guid_bool);
		variables.push_back(name_bool);
		variables.push_back(health_bool);
		variables.push_back(friend_bool);
		variables.push_back(index_bool);
		variables.push_back(id_bool);
		variables.push_back(view_bool);
		variables.push_back(view_float);
	}

	const char *name() const override;

	bool inEntityLoop(int index) override;
};
