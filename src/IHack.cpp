#include "IHack.h"

#include "SDK.h"
#include "CDrawManager.h"

#include "CMenu.h"

//void var::increment()
//{
//	if(type_ == type_t::Bool)
//		bVal = true;
//	if(type_ == type_t::Float)
//	{
//		if(fVal <= max_.fMax)
//		{
//			if((fVal + step_.fStep) <= max_.fMax)
//			{
//				fVal += step_.fStep;
//			}
//		}
//	}
//	if(type_ == type_t::Int)
//	{
//		if(iVal <= max_.iMax)
//		{
//			if((iVal + step_.iStep) <= max_.iMax)
//			{
//				iVal += step_.iStep;
//			}
//		}
//	}
//}
//
//void var::decrement()
//{
//	if(type_ == type_t::Bool)
//		bVal = false;
//	else if(type_ == type_t::Float)
//	{
//		if(fVal >= min_.fMin)
//		{
//			if((fVal - step_.fStep) >= min_.fMin)
//			{
//				fVal -= step_.fStep;
//			}
//		}
//	}
//	if(type_ == type_t::Int)
//	{
//		if(iVal >= min_.iMin)
//		{
//			if((iVal - step_.iStep) >= min_.iMin)
//			{
//				iVal -= step_.iStep;
//			}
//		}
//	}
//}
//
//const char *var::print() const
//{
//	char *buf;
//	// bool
//	if(type_ == type_t::Bool)
//	{
//		return bVal ? "true" : "false";
//	}
//
//	if(type_ == type_t::Switch)
//	{
//		return bVal ? "open" : "close";
//	}
//
//	// float
//	// TODO maybe allow manual precision setting
//	if(type_ == type_t::Float)
//	{
//		buf = new char[127];
//		sprintf_s(buf, 127, "%2.2f", fVal);
//		return buf;
//	}
//
//	// int
//	buf = new char[127];
//	sprintf_s(buf, 127, "%i", iVal);
//	return buf;
//}

// passed some useful stuff for lining up the menu
void IHack::menuDraw(DWORD menuColor, int menux, int y, int xx, int menuw, int h)
{
	gHack.getMenu()->drawList(variables, menuColor, menux, y, xx, menuw, h);
	return;
}

void vecVars::closeSwitch()
{
	for(auto &v : vars)
	{
		// if there is an open switch
		if(v.getType() == type_t::Switch && ((vecVars *)v.val)->open)
		{
			// see if there are any open switches in that
			((vecVars *)v.val)->closeSwitch();

			// close this current switch
			v.decrement();
		}
	}
}

void var::increment()
{
	if(type == type_t::Bool || type == type_t::Switch)
		*(bool *)val = true;
	if(type == type_t::Float)
	{
		float &Val = *(float *)val;
		float &Max = *(float *)upper.get();
		float &Step = *(float *)step.get();

		if(Val <= Max)
		{
			if((Val + Step) <= Max)
			{
				Val += Step;
			}
		}
	}
	else if(type == type_t::Int)
	{
		int &Val = *(int *)val;
		int &Max = *(int *)upper.get();
		int &Step = *(int *)step.get();

		if(Val <= Max)
		{
			if((Val + Step) <= Max)
			{
				Val += Step;
			}
		}
	}
}

void var::decrement()
{
	if(type == type_t::Bool || type == type_t::Switch)
		*(bool *)val = false;
	else if(type == type_t::Float)
	{
		float &Val = *(float *)val;
		float &Min = *(float *)lower.get();
		float &Step = *(float *)step.get();
		if(Val >= Min)
		{
			if((Val - Step) >= Min)
			{
				Val -= Step;
			}
		}
	}
	else if(type == type_t::Int)
	{
		int &Val = *(int *)val;
		int &Min = *(int *)lower.get();
		int &Step = *(int *)step.get();

		if(Val >= Min)
		{
			if((Val - Step) >= Min)
			{
				Val -= Step;
			}
		}
	}
}

std::string var::print()
{
	char buf[127];
	// bool
	if(type == type_t::Bool)
	{
		return *(bool *)val ? "true" : "false";
	}

	if(type == type_t::Switch)
	{
		return ((vecVars *)val)->open ? "open" : "close";
	}

	// float
	// TODO maybe allow manual precision setting
	if(type == type_t::Float)
	{
		//char *buf = new char[127];
		sprintf_s(buf, 127, "%2.2f", *(float *)val);
		return buf;
	}

	if(type == type_t::Int)
	{
		// int
		//char *buf = new char[127];
		sprintf_s(buf, 127, "%i", *(int *)val);
		return buf;
	}

	return "";
}

const char *var::Name()
{
	return name.c_str();
}
