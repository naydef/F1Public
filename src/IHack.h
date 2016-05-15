#pragma once

#include <string>
#include <vector>

#include "bf_buffer.h"

//#define F1_ENABLE_CONSOLE_VARS

enum class ButtonCode_t;

class CUserCmd;

enum class type_t
{
	Bool = 0,
	Int,
	Float,
	Switch, // uses bool to decide whether it is open or closed but also has a vector of vars inside of it
	Enum, // uses BETTER_ENUM
};

class var;

struct vecVars
{
	std::vector<var> vars;
	int index;
	bool open;
	bool e;
	vecVars *parent;

	vecVars()
	{
		index  = 0;
		parent = nullptr;
	}

	vecVars(const vecVars &) = default;

	vecVars(vecVars *p)
	{
		index  = 0;
		parent = p;
	}

	void push_back(const var &v)
	{
		return vars.push_back(v);
	}

	bool empty()
	{
		return e = (vars.size() == 0);
	}

	size_t size()
	{
		return vars.size();
	}

	var &operator[](size_t idx)
	{
		return vars[idx];
	}

	var &at(size_t idx)
	{
		return vars[idx];
	}
	var &active()
	{
		return vars[index];
	}

	std::vector<var>::iterator begin()
	{
		return vars.begin();
	}

	std::vector<var>::iterator end()
	{
		return vars.end();
	}

	void closeSwitch();
};

// stupid, increadibly hacky var class for bool, int and float
// allows us to store all of these types in one type (for vectors)
//class var
//{
//	type_t type_;
//
//public:
//	std::string name_;
//	vecVars vars;
//
//	// bool is stored in the int
//	union
//	{
//		float fVal;
//		int iVal;
//		bool bVal;
//	};
//
//	// max
//	union Max_t
//	{
//		float fMax;
//		int iMax;
//	};
//	Max_t max_;
//
//	// min
//	union Min_t
//	{
//		float fMin;
//		int iMin;
//	};
//	Min_t min_;
//
//	// step
//	union Step_t
//	{
//		float fStep;
//		int iStep;
//	};
//	Step_t step_;
//
//	union Default_t
//	{
//		float fDef;
//		int iDef;
//		bool bDef;
//	};
//
//	void increment();
//
//	void decrement();
//
//	const char *print() const;
//
//	var(std::string name, type_t type)
//	{
//		name_ = name;
//		type_ = type;
//
//		fVal = 0.0f;
//		iVal = 0;
//		bVal = false;
//		//vars.parent = this;
//	}
//
//	// all vars (except bools) must init!
//
//	void init(float min, float max, float step, float def)
//	{
//		min_.fMin   = min;
//		max_.fMax   = max;
//		step_.fStep = step;
//
//		fVal = def;
//	}
//
//	void init(int min, int max, int step, int def)
//	{
//		min_.iMin   = min;
//		max_.iMax   = max;
//		step_.iStep = step;
//
//		iVal = def;
//	}
//
//	template <typename T>
//	__declspec(deprecated("this function is replaced by the iGet bGet and fGet functions!")) T get()
//	{
//		if(std::is_same<T, double>() || std::is_same<T, float>())
//			return fVal;
//		if(std::is_same<T, int>())
//			return iVal;
//		if(std::is_same<T, bool>())
//			return bVal;
//
//		return T();
//	}
//
//	float fGet()
//	{
//		return fVal;
//	}
//	int iGet()
//	{
//		return iVal;
//	}
//	bool bGet()
//	{
//		return bVal;
//	}
//
//	type_t getType()
//	{
//		return type_;
//	}
//};

// TODO replace all instances of the old var with this new var
class var
{

	using defaultType = int;

	std::string name;
	type_t type;

	template<typename T>
	T ret()
	{
		if(val)
			return *(T *)val;
		else
			if(def)
				return *(T *)def.get();
			else
				return T();
	}

public:

	// we only own this memory if the type is a type_t::switch
	void *val;

	// we own this memory
	std::unique_ptr<int> lower{nullptr};
	std::unique_ptr<int> upper{nullptr};
	std::unique_ptr<int> def{nullptr};
	std::unique_ptr<int> step{nullptr};

	var(const var &v)
	{
		var::operator=(v);
	};
	//var(var &&) = default;
	var() = default;

	var(std::string n, vecVars *v, type_t t = type_t::Switch) : name(n), type(t), val(v)
	{
		((vecVars *)val)->open = false;
	}

	var(std::string n, void *v, type_t t) : name(n), type(t), val(v)
	{}

	var(std::string n, bool *v, type_t t = type_t::Bool) : name(n), type(t), val(v)
	{
		*(bool *)val = 0;
	}

	~var()
	{
		//Log::Error("~var() was called!!");
		// all of our memory should free itself here (unique pointers)
	}

	var(std::string n, int *Val, int Min, int Max, int Def, int Step, type_t t = type_t::Int) : var(n, Val, t)
	{
		*(std::unique_ptr<int> *)&lower = std::make_unique<int>(Min);
		*(std::unique_ptr<int> *)&upper = std::make_unique<int>(Max);
		*(std::unique_ptr<int> *)&def = std::make_unique<int>(Def);
		*(std::unique_ptr<int> *)&step = std::make_unique<int>(Step);

		*(int *)val = Def;
	}

	var(std::string n, float *Val, float Min, float Max, float Def, float Step, type_t t = type_t::Float) : var(n, Val, t)
	{
		*(std::unique_ptr<float> *)&lower = std::make_unique<float>(Min);
		*(std::unique_ptr<float> *)&upper = std::make_unique<float>(Max);
		*(std::unique_ptr<float> *)&def = std::make_unique<float>(Def);
		*(std::unique_ptr<float> *)&step = std::make_unique<float>(Step);

		*(float *)val = Def;
	}

	var &operator=(const var &o)
	{

		auto &other = const_cast<var&>(o);

		if(this != &other) // protect against invalid self-assignment
		{
			this->val = other.val;
			this->upper.swap(other.upper);
			this->lower.swap(other.lower);
			this->def.swap(other.def);
			this->step.swap(other.step);
			this->type = other.getType();
			this->name = other.name;
		}
		return *this;
	}

	float fGet()
	{
		return ret<float>();
	}
	int iGet()
	{
		return ret<int>();
	}
	bool bGet()
	{
		return ret<bool>();
	}

	type_t getType() const
	{
		return type;
	}

	void increment();

	void decrement();

	std::string print();

	const char *Name();
};


template<typename T>
class EnumVar : var
{
	
};

// base hack interface
class IHack
{
public:
	// vars for this hack
	vecVars variables;

	bool isOpen = false;

	virtual ~IHack()
	{
		// destroy the vecVars
		variables.~vecVars();
	}

	// each hack must impl the name function
	virtual const char *name() const = 0;

	// called on init
	virtual bool init()
	{
		return false;
	}

	// called during paint traverse
	virtual bool paint()
	{
		return false;
	}

	// called during clientmode createmove - takes a CUserCMD
	virtual bool move(CUserCmd *pUserCmd)
	{
		return false;
	}

	// THIS IS DISABLED
	// RUN EVERYTHING IN move(CUserCmd *pUserCmd)
	//virtual bool chlmove(CUserCmd *pUserCmd)
	//{
	//	return false;
	//}

	// called during keyevent
	virtual bool keyEvent(ButtonCode_t keynum)
	{
		return false;
	}

	virtual bool inEntityLoop(int index)
	{
		return false;
	}

	virtual bool userMessage(int type, bf_read &data)
	{
		return false;
	}

	// called by the menu when a class needs to draw its menu
	virtual void menuDraw(DWORD menuColor, int menux, int y, int xx, int menuw, int h);
};
