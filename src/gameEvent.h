#pragma once

class IGameEvent
{
public:
	virtual ~IGameEvent(){};
	virtual const char *GetName() const = 0; // get event name

	virtual bool IsReliable() const = 0;				  // if event handled reliable
	virtual bool IsLocal() const = 0;					  // if event is never networked
	virtual bool IsEmpty(const char *keyName = NULL) = 0; // check if data field exists

	// Data access
	virtual bool GetBool(const char *keyName = NULL, bool defaultValue = false) = 0;
	virtual int GetInt(const char *keyName = NULL, int defaultValue = 0) = 0;
	virtual float GetFloat(const char *keyName = NULL, float defaultValue = 0.0f) = 0;
	virtual const char *GetString(const char *keyName = NULL, const char *defaultValue = "") = 0;

	virtual void SetBool(const char *keyName, bool value) = 0;
	virtual void SetInt(const char *keyName, int value) = 0;
	virtual void SetFloat(const char *keyName, float value) = 0;
	virtual void SetString(const char *keyName, const char *value) = 0;
};

class IGameEventListener2
{
public:
	virtual ~IGameEventListener2(void){};

	// FireEvent is called by EventManager if event just occured
	// KeyValue memory will be freed by manager if not needed anymore
	virtual void FireGameEvent(IGameEvent *event) = 0;
};

class IGameEventManager2
{
public:
	bool AddListener(IGameEventListener2 *listener, const char *name, bool bServerSide)
	{
		typedef bool(__thiscall * OriginalFn)(PVOID, IGameEventListener2 *, const char *, bool);
		return getvfunc<OriginalFn>(this, 3)(this, listener, name, bServerSide);
	}
};
