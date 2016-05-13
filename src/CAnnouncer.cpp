#include "CAnnouncer.h"

#include "SDK.h"

// girs stuff - huge creds to him for this

bool CAnnouncer::init(void)
{
	// Log::Console("adding listeners");
	gInts.EventManager->AddListener(this, "player_death", false);
	gInts.EventManager->AddListener(this, "player_spawn", false);

	return true;
}

void CAnnouncer::FireGameEvent(IGameEvent *event)
{
	//Log::Console("%s Attacker: %i CustomKill: %i Me: %i Userid: %i Inflictor: %i Time: %f", event->GetName(), event->GetInt("attacker", 0),
	//			 event->GetInt("customkill", 0), me, event->GetInt("userid", 0), event->GetInt("inflictor_entindex", 0), gInts.Globals->curtime);
	if(gLocalPlayerVars.info.userID == 0) // CBasePlayer::GetUserId() You can get this from the player_info_t struct as well.
		return;

	if(!strcmp(event->GetName(), "player_death"))
	{
		int attacker = event->GetInt("attacker", 0);
		int userId   = event->GetInt("userid", 0);
		// Log::Console("Player death attacker: %d, userID: %d, localID: %d", attacker, userId, gLocalPlayerVars.info.userID);
		if(attacker == gLocalPlayerVars.info.userID)
		{
			if(attacker == userId) // Suicide
				return;

			killCounter++;
			killStreakCounter++;

			if((gInts.Globals->curtime - lastKillTime) < killStreakTimeout)
			{
				// This is done to prevent getting spammed with killstreak sounds when killing like 4 people at once with a crit sticky, so it will play the
				// most recent sound when Think is called.
				switch(killStreakCounter)
				{
				case 2:
					strcpy(chKillstreakSound, "UT2k4/Double_Kill.wav");
					break;
				case 3:
					strcpy(chKillstreakSound, "Quake3/quake_tripplekill.wav");
					break;
				case 4:
					strcpy(chKillstreakSound, "UT99/multikill.wav");
					break;
				case 5:
					strcpy(chKillstreakSound, "UT2k4/MegaKill.wav");
					break;
				case 6:
					strcpy(chKillstreakSound, "UT99/ultrakill.wav");
					break;
				case 7:
					strcpy(chKillstreakSound, "UT99/monsterkill.wav");
					break;
				case 8:
					strcpy(chKillstreakSound, "UT2k4/LudicrousKill.wav");
					break;
				default: // 9 or more kills in a row, play 'Holy Shit!' every time.
					strcpy(chKillstreakSound, "UT2k4/HolyShit.wav");
				}
			}
			else
			{
				// This scenario is when you kill someone outside the killStreakTimeout variable.
				killStreakCounter = 1;
			}

			switch(static_cast<tf_customkills>(event->GetInt("customkill", 0)))
			{
			case tf_customkills::HEADSHOT:
			case tf_customkills::HEADSHOT_DECAPITATION:
			case tf_customkills::PENETRATE_HEADSHOT:
				PlaySound("UT99/headshot.wav");
				break;
			}

			// if(gPlayerVars.bHasMeleeWeapon)
			//{
			//	PlaySound("Quake3/quake_humiliation.wav"); //A little quake never hurt nobody.
			//}

			switch(killCounter)
			{
#ifdef USE_TF2_KILLSTREAKS
			case 5:
				PlaySound("UT99/killingspree.wav");
				break;
			case 10:
				PlaySound("UT99/unstoppable.wav");
				break;
			case 15:
				PlaySound("UT99/rampage.wav");
				break;
			default: // If the player gets a 20 or more killstreak, just play Godlike every 5 kills.
				if(killCounter % 5 == 0)
					PlaySound("UT99/godlike.wav");
#else
			case 5:
				PlaySound("UT99/killingspree.wav");
				break;
			case 10:
				PlaySound("UT99/rampage.wav");
				break;
			case 15:
				PlaySound("UT2k4/Dominating.wav");
				break;
			case 20:
				PlaySound("UT99/unstoppable.wav");
				break;
			default: // If the player gets a 25 or more killstreak, just play Godlike every 5 kills.
				if(killCounter % 5 == 0)
					PlaySound("UT99/godlike.wav");
#endif
			}
			lastKillTime = gInts.Globals->curtime;
		}
		return;
	}
	if(!strcmp(event->GetName(), "player_spawn")) // This will also get called when the player changes class.
	{
		if(event->GetInt("userid", 0) == gLocalPlayerVars.info.userID)
		{
			killCounter  = 0;
			lastKillTime = 0.0f;
		}
		return;
	}
}

bool CAnnouncer::move(CUserCmd *)
{
	if(chKillstreakSound)
	{
		if(strcmp(chKillstreakSound, "")) // Check if the killstreak has something queued.
		{
			PlaySound(chKillstreakSound);
			strcpy(chKillstreakSound, ""); // Once done, zero out the chKillstreakSound member.
		}
	}
	return true;
}

void CAnnouncer::PlaySound(const char *soundName)
{
	// other method
	// auto filt = CSingleUserRecipientFilter{gInts.EntList->GetClientEntity(me)};
	// gInts.SoundEngine->EmitSound(filt, -1, 0, soundName, 100, 100);

	// works fine when running sv_pure 0 or -1
	// meaning that it works fine with the sv_pure bypass
	gInts.Surface->PlaySound(soundName);
}
