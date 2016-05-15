#include "CHack.h"
#include "Panels.h"
#include "CDrawManager.h"
#include "Util.h"
#include "CHackState.h"

// include hacks
#include "CEsp.h"
#include "CBackstab.h"
#include "CGlow.h"
#include "CMenu.h"
#include "CTrigger.h"
#include "CMisc.h"
#include "CNoise.h"
#include "CAimbot.h"
#include "CAutoAirblast.h"
#include "CAnnouncer.h"
#include "CNospread.h"
#include "CPureBypass.h"

CScreenSize gScreenSize;

CHack::CHack()
{
}
//===================================================================================

CHack::~CHack()
{
}
//===================================================================================

// for drawing on screen
void CHack::paintTraverse(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{
	_TRY
	{
		_INSTALL_SEH_TRANSLATOR();

		VMTManager &hook = VMTManager::GetHook(pPanels);																								 //Get a pointer to the instance of your VMTManager with the function GetHook.
		hook.GetMethod<void(__thiscall *)(PVOID, unsigned int, bool, bool)>(gOffsets.paintTraverseOffset)(pPanels, vguiPanel, forceRepaint, allowForce); //Call the original.

		static unsigned int vguiMatSystemTopPanel;

		if(vguiMatSystemTopPanel == 0) // check for null - unable to use nullptr D:
		{
			const char *szName = gInts.Panels->GetName(vguiPanel);
			if(szName[0] == 'M' && szName[3] == 'S') //Look for MatSystemTopPanel without using slow operations like strcmp or strstr.
			{
				vguiMatSystemTopPanel = vguiPanel;
				// run our intro
				Intro();
			}
		}

		if(vguiMatSystemTopPanel == vguiPanel) //If we're on MatSystemTopPanel, call our drawing code.
		{

			gInts.DebugOverlay->ClearAllOverlays();

			if(gInts.Engine->IsDrawingLoadingImage() || !gInts.Engine->IsInGame() || !gInts.Engine->IsConnected() || gInts.Engine->Con_IsVisible() || ((GetAsyncKeyState(VK_F12) || gInts.Engine->IsTakingScreenshot())))
				return; //We don't want to draw at the menu.

			//This section will be called when the player is not at the menu game and can see the screen or not taking a screenshot.
			gDrawManager.DrawString("hud", (gScreenSize.iScreenWidth / 2) - 55, 200, gDrawManager.dwGetTeamColor(gLocalPlayerVars.team), "Welcome to F1"); //Remove this if you want.

			// debug string stuff

			int y = 600;
			//gDrawManager.DrawString("hud", 0, y, COLOR_OBJ, "old: %f %f %f", gLocalPlayerVars.pred.oldOrigin[0], gLocalPlayerVars.pred.oldOrigin[1], gLocalPlayerVars.pred.oldOrigin[2]);
			//y += gDrawManager.GetHudHeight();
			//gDrawManager.DrawString("hud", 0, y, COLOR_OBJ, "new: %f %f %f", gLocalPlayerVars.pred.origin[0], gLocalPlayerVars.pred.origin[1], gLocalPlayerVars.pred.origin[2]);
			//y += gDrawManager.GetHudHeight();
			//gDrawManager.DrawString("hud", 0, y, COLOR_OBJ, "dif: %f %f %f", gLocalPlayerVars.pred.origin[0] - gLocalPlayerVars.pred.oldOrigin[0], gLocalPlayerVars.pred.origin[1] - gLocalPlayerVars.pred.oldOrigin[1], gLocalPlayerVars.pred.origin[2] - gLocalPlayerVars.pred.oldOrigin[2]);
			//y += gDrawManager.GetHudHeight();
			//gDrawManager.DrawString("hud", 0, y, COLOR_OBJ, "curtime: %f", gInts.Globals->curtime);
			//y += gDrawManager.GetHudHeight();
			//gDrawManager.DrawString("hud", 0, y, COLOR_OBJ, "flNextAttack: %f", gLocalPlayerVars.flNextAttack);
			//y += gDrawManager.GetHudHeight();
			//gDrawManager.DrawString("hud", 0, y, COLOR_OBJ, "diff: %f", gLocalPlayerVars.flNextAttack - gInts.Globals->curtime);
			//y += gDrawManager.GetHudHeight();

			// call painttraverse for all hacks
			for(auto &hack : men.hacks)
			{
				if(hack != nullptr)
				{
					_TRY
					{
						hack->paint(); // call each hacks paint function
					}
					_CATCH_SEH_REPORT_ERROR(hack, paint())
					_CATCH
					{
						//Log::Error(XorString("Error with hack `%s' in paint!"), hack->name());
						_REPORT_ERROR(hack, paint());
						throw;
					}
				}
			}
			// CWorld always occupies entity index 0
			// players are reserved from 1-32
			// therefore this loop can start from 1 rather than 0
			// TODO add seperate try/catch for entity loop since it is special
			for(int i = 1; i < gInts.EntList->GetHighestEntityIndex(); i++)
			{
				for(auto &hack : men.hacks)
				{
					if(hack != nullptr)
					{
						_TRY
						{
							hack->inEntityLoop(i); // call each hacks paint in entity loop function
						}
						_CATCH_SEH_REPORT_ERROR(hack, inEntityList())
						_CATCH
						{
							//Log::Error(XorString("Error with hack `%s' in paint!"), hack->name());
							//throw;
							_REPORT_ERROR(hack, inEntityLoop(index));
						}
					}
				}
			}
			men.menu(); // draw our cheat menu
		}
	}
	_CATCHMODULE
	{
		Log::Error("%s", e.what());
	}
	_CATCH
	{
		Log::Error("other error in CHack::paintTraverse");
	}
	//Log::Fatal(XorString("Failed PaintTraverse"));
}
//===================================================================================

// for commands and movement
bool CHack::createMove(PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd *pUserCmd, DWORD createMoveEBP)
{
	_TRY
	{
		_INSTALL_SEH_TRANSLATOR();

		auto &hook   = VMTManager::GetHook(ClientMode);
		bool bReturn = hook.GetMethod<bool(__thiscall *)(PVOID, float, CUserCmd *)>(gOffsets.createMoveOffset)(ClientMode, input_sample_frametime, pUserCmd);

		CEntity<> local{me};
		CBaseEntity *localEnt = local.castToPointer<CBaseEntity>();

		// this should NEVER happen
		if(local.isNull())
			throw;

		// update our stats every game tick
		gLocalPlayerVars.Class  = local.get<tf_classes>(gEntVars.iClass);
		gLocalPlayerVars.cond   = local.get<int>(gEntVars.iPlayerCond);
		gLocalPlayerVars.condEx = local.get<int>(gEntVars.iPlayerCondEx);
		gLocalPlayerVars.health = local.get<int>(gEntVars.iHealth);
		gLocalPlayerVars.team   = local.get<int>(gEntVars.iTeam);
		gLocalPlayerVars.cmdNum = pUserCmd->command_number;
		gLocalPlayerVars.info   = gInts.Engine->GetPlayerInfo(me);
		gLocalPlayerVars.flags  = local.get<int>(gEntVars.iFlags);

		CBaseEntity *pLocalWep = gInts.EntList->GetClientEntity(HANDLE2INDEX(local.get<int>(gEntVars.hActiveWeapon)));

		if(pLocalWep)
		{
			gLocalPlayerVars.activeWeapon = pLocalWep->GetClientClass()->iClassID;

			gLocalPlayerVars.flNextAttack = CEntity<>{pLocalWep->GetIndex()}.get<float>(gEntVars.flNextPrimaryAttack);
		}
		else
			gLocalPlayerVars.activeWeapon = static_cast<classId>(-1);


		// begin local client cmd prediction
		gLocalPlayerVars.pred.oldOrigin = localEnt->GetAbsOrigin();

		CMoveData moveData;

		memset(&moveData, 0, sizeof(CMoveData));

		// back up the globals
		float oldCurTime = gInts.Globals->curtime;
		float oldFrameTime = gInts.Globals->frametime;

		// set up the globals
		gInts.Globals->curtime = local.get<float>(gEntVars.nTickBase) * gInts.Globals->interval_per_tick;
		gInts.Globals->frametime = gInts.Globals->interval_per_tick;

		CBaseEntity *pLocal = local.castToPointer<CBaseEntity>();

		// set the current cmd
		local.set<CUserCmd *>(0x107C, pUserCmd);

		gInts.GameMovement->StartTrackPredictionErrors(pLocal);

		// do actual player cmd prediction
		gInts.Prediction->SetupMove(pLocal, pUserCmd, gInts.MoveHelper, &moveData);
		gInts.GameMovement->ProcessMovement(pLocal, &moveData);
		gInts.Prediction->RunCommand(pLocal, pUserCmd, gInts.MoveHelper);
		gInts.Prediction->FinishMove(pLocal, pUserCmd, &moveData);

		gInts.GameMovement->FinishTrackPredictionErrors(pLocal);

		// reset the current cmd
		local.set<CUserCmd *>(0x107C, 0);

		// restore the globals
		gInts.Globals->curtime = oldCurTime;
		gInts.Globals->frametime = oldFrameTime;

		gLocalPlayerVars.pred.origin = local->GetAbsOrigin();

		// set these before the hacks run
		// we cant have these in chlmove as by that point they have already run

		silentData.fMove = pUserCmd->forwardmove;
		silentData.sMove = pUserCmd->sidemove;
		silentData.view  = pUserCmd->viewangles;

		for(auto &hack : men.hacks)
		{
			if(hack != nullptr)
			{
				_TRY
				{
					hack->move(pUserCmd); // call each hacks move function
				}
				_CATCH_SEH_REPORT_ERROR(hack, move())
				_CATCH
				{
					//Log::Error(XorString("Error with hack `%s' in move!"), men.hacks[i]->name());
					//throw;
					_REPORT_ERROR(hack, move(pUserCmd));
				}
			}
			else
				break;
		}

	}
	_CATCHMODULE
	{
		Log::Error("%s", e.what());
	}
	_CATCH_SEH_REPORT_ERROR(this, createMove())

	return false;
}
//===================================================================================

// set up draw manager and netvars
void CHack::intro()
{
	_TRY
	{

		// wait until we have completely inited before running intro
		while(!HState::instance()->checkStatus(HState::FullyInited))
		{
			HState::instance()->think();
		}

		// creds lmaobox
		if(gInts.steam.steamApps->BIsDlcInstalled(459)) // http://steamdb.info/app/459/
		{
			Log::Error("Why are you trying to run a public hack on a premium account??\nIm warning you now!\nARE YOU SURE YOU WANT TO GET VAC BANNED??");
		}

		gDrawManager.Initialize(); //Initalize the drawing class.

		gEntVars.find(); // find our netvars

		// find gameResource offsets
		// these are seperate from the entVars due to the number of
		// netvars that there are
		//gInts.GameResource->findOffsets();

		LOGDEBUG(XorString("==========================================================="));

		//{
		//	CDumper nDumper;
		//	nDumper.SaveDump( );
		//}

		// TODO the hacks should add themselves
		// we should NOT add them

		// try to load all of the modules
		_TRY
		{
			men.addHack(new CESP());
			men.addHack(new CBackstab());
			men.addHack(new CGlow());
			men.addHack(new CTrigger());
			men.addHack(new CMisc());
			men.addHack(new CNoise());
			men.addHack(new CAimbot());
			men.addHack(new CAutoAirblast());
			//men.addHack(new CNospread());
			men.addHack(new CAnnouncer());
			men.addHack(new CPureBypass());
		}
		_CATCH
		{
			moduleException e{"Error initialing hacks in intro"};
			throw e;
		}

		// run their init function (this could be rolled into the constructor??
		for(auto &hack : men.hacks)
		{
			_TRY
			{
				if(hack != nullptr)
					hack->init(); // call each hacks init function
			}
			_CATCH
			{
				_REPORT_ERROR(hack, init());
			}
		}

		// intro printing stuff to console

		Color c(255, 0, 0, 255);
		gInts.Cvar->ConsolePrintf(XorString("_____________________________________________\n"));
		gInts.Cvar->ConsoleColorPrintf(c, XorString("  __| _ |  _ \\       |     | _)      \n"));
		gInts.Cvar->ConsoleColorPrintf(c, XorString("  _|    |  __/ |  |   _ \\  |  |   _| \n"));
		gInts.Cvar->ConsoleColorPrintf(c, XorString(" _|    _| _|  \\_,_| _.__/ _| _| \\__| \n"));
		gInts.Cvar->ConsoleColorPrintf(c, XorString("F1Public hack loaded successfully.\n"));
		gInts.Cvar->ConsoleColorPrintf(c, XorString("Have Fun!\n"));
		gInts.Cvar->ConsolePrintf(XorString("_____________________________________________\n"));

		CSteamID localID = gInts.steam.user->GetSteamID();

		gInts.Cvar->ConsoleColorPrintf(c, "SteamID:     %s\n", localID.Render());
		gInts.Cvar->ConsoleColorPrintf(c, "newSteamID:  %s\n", localID.SteamRender());
		gInts.Cvar->ConsoleColorPrintf(c, "PersonaName: %s\n", gInts.steam.friends->GetPersonaName());
		gInts.Cvar->ConsolePrintf(XorString("_____________________________________________\n"));

		//for ( int i = 0; i < gInts.steam.friends->GetFriendCount( k_EFriendFlagImmediate ); i++ )
		//{
		//	CSteamID friendID = gInts.steam.friends->GetFriendByIndex( i, k_EFriendFlagImmediate );
		//	const char *name = gInts.steam.friends->GetFriendPersonaName( friendID );
		//	gInts.Cvar->ConsoleColorPrintf( c, "Friend: %s, %s\n", friendID.Render( ), name );
		//	if ( !strcmp( name, "Aeix" ) )
		//	{
		//		const char *msg = "Cyka Blyat Idi Nahui";
		//		bool test = gInts.steam.friends->SendMsgToFriend( friendID, EChatEntryType::k_EChatEntryTypeChatMsg, (void *) msg, strlen( msg ) );
		//	}
		//}

		for(auto &hack : men.hacks)
		{
			gInts.Cvar->ConsoleColorPrintf(c, "Loaded %s\n", hack->name());
		}

		// unprotect / remove mins and maxes of all cvars
		// causes plenty of problems on some smac servers
		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)killCvars, nullptr, 0, nullptr);

		// finally log that we successfully hooked and injected
		Log::Msg(XorString("Injection Successful")); //If the module got here without crashing, it is good day.
	}
	_CATCHMODULE
	{
		//Log::Fatal("%s", XorString("Failed Intro"));

		// failing intro is kinda fatal
		Log::Fatal("%s", e.what());
	}
	_CATCH
	{
		Log::Fatal("Undefined exception during intro!\nExiting now!");
	}
}
//===================================================================================

// hooked in key event
int CHack::keyEvent(PVOID CHLClient, int edx, int eventcode, ButtonCode_t keynum, const char *currentBinding)
{
	int ret;
	_TRY
	{
		_INSTALL_SEH_TRANSLATOR();

		VMTManager &hook = VMTManager::GetHook(CHLClient);																														// Get a pointer to the instance of your VMTManager with the function GetHook.
		ret			  = hook.GetMethod<int(__thiscall *)(PVOID, int, int, const char *)>(gOffsets.keyEvent)(CHLClient, eventcode, static_cast<int>(keynum), currentBinding); // Call the original.

		if(eventcode == 1)
		{
			for(auto &hack : men.hacks)
			{
				if(hack != nullptr)
				{
					_TRY
					{
						hack->keyEvent(keynum); // call each hacks keyEvent function
					}
					_CATCH
					{
						//Log::Error(XorString("Error with hack `%s' in keyEvent!"), hack->name());
						//throw;
						_REPORT_ERROR(hack, keyEvent());
					}
				}
			}

			return men.keyEvent(keynum);
		}
	}
	_CATCHMODULE
	{
		//Log::Fatal(XorString("Error with key event!"));
		Log::Error("%s", e.what());
	}

	return ret;
}
//===================================================================================
