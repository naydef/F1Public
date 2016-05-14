#include "SDK.h"
#include "CDrawManager.h"

int CMenu::keyEvent(ButtonCode_t keynum)
{
	if(keynum == ButtonCode_t::KEY_INSERT) //insert
	{
		flipMenu();
		if(activeVars == nullptr)
			if(hacks.size() != 0)
			{
				activeVars = &hacks[0]->variables;
				iIndex = 0;
			}

		gInts.Surface->PlaySound("UI/buttonclick.wav");
	}

	if(bMenuActive)
	{
		switch(keynum)
		{
		case ButtonCode_t::KEY_UP:
		case ButtonCode_t::MOUSE_WHEEL_UP:
			if(activeVars->index > 0)
				activeVars->index--;
			else
				activeVars->index = activeVars->size() - 1;

			gInts.Surface->PlaySound("UI/item_info_mouseover.wav");

			return 0;
			break;

		case ButtonCode_t::KEY_DOWN:
		case ButtonCode_t::MOUSE_WHEEL_DOWN:
			if(activeVars->index < activeVars->size() - 1)
				activeVars->index++;
			else
				activeVars->index = 0;

			gInts.Surface->PlaySound("UI/item_info_mouseover.wav");

			return 0;
			break;

		case ButtonCode_t::KEY_LEFT:
		case ButtonCode_t::MOUSE_LEFT:
			activeVars->active().decrement();

			gInts.Surface->PlaySound("UI/buttonclick.wav");

			return 0;
			break;

		case ButtonCode_t::KEY_RIGHT:
		case ButtonCode_t::MOUSE_RIGHT:
			activeVars->active().increment();

			gInts.Surface->PlaySound("UI/buttonclick.wav");

			return 0;
			break;

		case ButtonCode_t::KEY_TAB:
			// try to find the first empty hack
			int newIndex;
			newIndex = iIndex;
			do
			{
				if(newIndex < hacks.size() - 1)
					newIndex += 1;
				else if(newIndex == hacks.size() - 1)
					newIndex = 0;
			} while(hacks[newIndex]->variables.empty());

			iIndex = newIndex;
			// close all active switches
			activeVars->closeSwitch();

			// set the active vars to the next
			activeVars = &hacks[iIndex]->variables;

			gInts.Surface->PlaySound("UI/item_info_mouseover.wav");

			return 0;
			break;

		case ButtonCode_t::KEY_ENTER:
			Log::Console("Enter pressed!!");
			// switches can only be opened using the enter key
			if(activeVars->active().getType() == type_t::Switch)
			{
				activeVars->active().increment();
				//Log::Console("var %s is now %s", activeVars->active().name_.c_str(), activeVars->active().bVal ? "true" : "false");
				activeVars = (vecVars *)activeVars->active().val;

				gInts.Surface->PlaySound("UI/buttonclick.wav");

			}
			else
			{
				// deactivate this switch via its parent
				if(activeVars->parent)
				{
					activeVars = activeVars->parent;
					activeVars->active().decrement();
					//Log::Console("var %s is now %s", activeVars->active().name_.c_str(), activeVars->active().bVal ? "true" : "false");

					gInts.Surface->PlaySound("UI/buttonclickrelease.wav");

				}
			}
			return 0;
			break;

		default:
			return 1;
		}
	}
	// get the engine to handle all other cases
	return 1;
}

void CMenu::menu()
{
	if(!bMenuActive)
		return;
	if(hacks.empty()) // if cheats list is empty
		return;
	try
	{

		//Log::Console("iIndex == %d", iIndex);

		int x	 = 0,
			menux = 300,
			y	 = 300,
			yy	= 0,
			//h	 = gDrawManager.GetPixelTextSize("hud", "A").height,
			menuw = 230;

		//int hudHeight = gDrawManager.GetPixelTextSize("hud", "A").height;
		const int menuMainHeight = gDrawManager.GetPixelTextSize("menuMain", "A").height;
		const int menuListHeight = gDrawManager.GetPixelTextSize("menuList", "A").height;

		yy = GetMenuHeight(menuMainHeight);

		// menu color is team color
		DWORD menuColor = gDrawManager.dwGetTeamColor(gLocalPlayerVars.team);

		// draw the background
		gDrawManager.DrawRect(x, y, (gScreenSize.iScreenWidth / 3), yy, COLORCODE(30, 30, 30, 170));

		int curr = yy = 0;

		for(auto &hack : hacks)
		{
			//size.length += 2;
			//size.height = h + 2;
			if(!hack->variables.empty())
			{
				if(curr == iIndex)
				{
					//gDrawManager.DrawRect(x, y + yy, size.length, size.height, COLOR_BLACK);
					//gDrawManager.OutlineRect(x, y + yy, size.length, size.height, COLOR_MENU_OFF);
					gDrawManager.DrawString("menuMain", x + 2, y + yy + 1, COLOR_MENU_OFF, hack->name());
				}
				else
				{
					//gDrawManager.DrawRect(x, y + yy, size.length, size.height, COLOR_BLACK);
					//gDrawManager.OutlineRect(x, y + yy, size.length, size.height, menuColor);
					gDrawManager.DrawString("menuMain", x + 2, y + yy + 2, menuColor, hack->name());
				}
				yy += menuMainHeight;
				curr++;

				CDrawManager::font_size_t size = gDrawManager.GetPixelTextSize("hud", hack->name());
				menux = max(menux, size.length);
			}
		}

		int xx = menux + 170;

		gDrawManager.DrawString(XorString("hud"), x + 12, y - (menuMainHeight + 2), menuColor, XorString("F1ssi0N 2016 Hack \"Gitlab.com/josh33901/F1Rebase/\""));

		y += 32;

		// drawList(hacks[iIndex]->variables, menuColor, menux, y, xx, menuw, h);

		// call the current hack's menu draw function
		hacks[iIndex]->menuDraw(menuColor, menux, y, xx, menuw, menuListHeight);

		return;
	}
	catch(...)
	{
		Log::Debug(XorString("Error with cheat menu!"));
		throw;
	}
}

void CMenu::drawList(vecVars &list, DWORD color, int x, int y, int xx, int w, int h)
{

	// TODO maybe check first if there is an open switch to resolve highlighting issues

	// posibility for this is that every var vector has its own iiIndex that it can set to and arbitary value
	// outside its range in order to have no index, therefore eliminating the problem

	int iMenuItems = list.size();

	// draw the background box
	//gDrawManager.DrawRect(x, y, w, iMenuItems * h, COLOR_BACK);
	//gDrawManager.OutlineRect(x, y, w, iMenuItems * h, color);

	for(int i = 0; i < iMenuItems; i++)
	{
		var &curr = list[i];

		if(curr.getType() == type_t::Switch)
		{
			if(((vecVars *)curr.val)->open)
			{
				int newx  = x + w + 4;
				int newxx = w + 4 + xx;

				drawList(*(vecVars *)curr.val, color, newx, y, newxx, w, h);
			}
		}

		if(i != list.index)
		{
			gDrawManager.DrawString(XorString("menuList"), x + 2, y + (h * i), COLOR_MENU_OFF, curr.Name());
			gDrawManager.DrawString(XorString("menuList"), xx, y + (h * i), COLOR_MENU_OFF, curr.print().c_str()); // call print for types
		}
		else
		{
			gDrawManager.DrawRect(x + 1, y + (h * i), w - 2, h, COLORCODE(255, 255, 255, 80));
			gDrawManager.DrawString(XorString("menuList"), x + 2, y + (h * i), color, curr.Name());
			gDrawManager.DrawString(XorString("menuList"), xx, y + (h * i), color, curr.print().c_str()); // call print for types
		}
	}
}

void CMenu::addHack(IHack *hack)
{
	hacks.emplace_back(hack);
}

void CMenu::get()
{
}
