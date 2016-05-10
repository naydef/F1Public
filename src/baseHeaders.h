#pragma once

// base, common and standalone headers are defined here

// auto define lean and mean
#include "windows.h"
#include <math.h>
#include <xstring>
#include <vector>
#include <math.h>
#include <float.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <thread>
//#include <vld.h>

// steamtypes contains most of the basic source types that are used
#include "steam/SteamTypes.h"

// main class for steamworks
#include "steam\\Steamworks.h"

// valve platform defines and functions
#include "valvePlatform.h"

// standalone headers
#include "CUtlVector.h"
#include "getvfunc.h"
#include "dt_recv2.h"
#include "CGlobalVars.h"
#include "VMTHooks.h"
#include "Color.h"
#include "crc32.h"
#include "ClassId.h"
#include "bitvec.h"
#include "factory.h"
#include "notNullInterface.h"
#include "customKills.h"
#include "gameEvent.h"
#include "enum.h"

#include "backtrace.h"

// xorstring stuff
#ifdef __XOR
#include "XorString.h" // compile time string encryption
#else
#define XorString(X) X
#define XorS(X, String) String
#endif

#define CHECK_VALID(...) ((void)0)

// moved out of sdk.h
class ClientClass
{
private:
	BYTE _chPadding[8];

public:
	char *chName;
	RecvTable *Table;
	ClientClass *pNextClass;
	classId iClassID;
};

#pragma region network stuff
#define FLOW_OUTGOING 0
#define FLOW_INCOMING 1
#define MAX_FLOWS 2 // in & out
#define MAX_STREAMS 2

#define MAX_OSPATH 260

// TODO to enums atleast pls

#define SUBCHANNEL_FREE 0	// subchannel is free to use
#define SUBCHANNEL_TOSEND 1  // subchannel has data, but not send yet
#define SUBCHANNEL_WAITING 2 // sbuchannel sent data, waiting for ACK
#define SUBCHANNEL_DIRTY 3   // subchannel is marked as dirty during changelevel

// typedef DWORD IDemoRecorder;
typedef void *FileHandle_t;
typedef int QueryCvarCookie_t;

typedef enum
{
	NA_NULL = 0,
	NA_LOOPBACK,
	NA_BROADCAST,
	NA_IP,
} netadrtype_t;

typedef enum
{
	eQueryCvarValueStatus_ValueIntact   = 0, // It got the value fine.
	eQueryCvarValueStatus_CvarNotFound  = 1,
	eQueryCvarValueStatus_NotACvar		 = 2, // There's a ConCommand, but it's not a ConVar.
	eQueryCvarValueStatus_CvarProtected = 3  // The cvar was marked with FCVAR_SERVER_CAN_NOT_QUERY, so the server is not allowed to have its value.
} EQueryCvarValueStatus;
#pragma endregion

inline void quickCrash()
{
	// volatile ensures that it gets compiled
	volatile int x = 0;
	*(int *)x = 10;
}
//===================================================================================================
#define me gInts.Engine->GetLocalPlayer()
#define MASK_AIMBOT 0x200400B
#define CONTENTS_HITBOX 0x40000000
#define CASUAL_HACKER_MASK 0x4600400B
#define PI 3.14159265358979323846f
#define square(x) (x * x)
#define RADPI 57.295779513082f
#define BLU_TEAM 3
#define RED_TEAM 2
#define FL_ONGROUND (1 << 0)
#define FL_DUCKING (1 << 1)
// disgusting but expressive and obvious
// works because CHandle is just a wrapper for an int
#define HANDLE2INDEX(handle) (handle & 0xFFF)
//int constexpr HANDLE2INDEX(CHandle handle){ return handle & 0xFFF; }
#define SAFEDELETE(x) {if(x) delete x; x = nullptr;}
//===================================================================================================

#define RED(COLORCODE) ((int)(COLORCODE >> 24))
#define BLUE(COLORCODE) ((int)(COLORCODE >> 8) & 0xFF)
#define GREEN(COLORCODE) ((int)(COLORCODE >> 16) & 0xFF)
#define ALPHA(COLORCODE) ((int)COLORCODE & 0xFF)
#define COLORCODE(r, g, b, a) ((DWORD)((((r)&0xff) << 24) | (((g)&0xff) << 16) | (((b)&0xff) << 8) | ((a)&0xff)))

BETTER_ENUM(moveTypes, int, none = 0, isometric, walk, step, fly, flygravity, vphysics, push, noclip, ladder, observer, custom)