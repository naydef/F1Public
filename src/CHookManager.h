#pragma once

#include "VMTHooks.h"

#include <unordered_map>
#include <memory>

#include "windows.h"

#include "Log.h"

using namespace toolkit;

// manager for vmt hooks
class CHookManager
{


	std::unordered_map<void *, std::vector<int>> hookedMethods;

public:

	~CHookManager()
	{
		// clear up hooks
		for(auto &kv : hookedMethods)
		{
			delete &VMTBaseManager::GetHook(kv.first);
		}
	}

	bool isHooked(void *address) try
	{
		return VMTManager::HookPresent(address);
	}
	catch(...)
	{
		return false;
	}

	// performs no initialisation on the hook created
	VMTBaseManager *findOrCreateHook(void *address, unsigned offset = 0)
	{
		if(isHooked(address))
		{
			return &VMTBaseManager::GetHook((void *)address, offset);
		}
		else
		{
			hookedMethods[address] = {0};
			auto hook = new VMTBaseManager();

			hook->Init(address, offset);

			return hook;
		}
	}

	bool methodHooked(void *address, int index)
	{
		for(auto &hookedMethod : hookedMethods[address])
		{
			if(hookedMethod == index) return true;
		} 

		return false;
	}

	void hookMethod(void *address, int index, void *newFunc, unsigned offset = 0)
	{
		if(methodHooked(address, index))
		{
			Log::Error("Possible erronious double hook on method index %d from instance 0x%X", index, address);
		}

		hookedMethods[address].push_back(index);

		VMTBaseManager *hook = findOrCreateHook(address, offset);

		hook->HookMethod(newFunc, index);
		hook->Rehook();
	}

	template<typename T>
	T getMethod(void *address, int index)
	{
		if(!isHooked(address))
			Log::Error("trying to find function index '%d' from address 0x%X with no hook!!", index, address);

		return findOrCreateHook(address)->GetMethod<T>(index);
	}
};

extern CHookManager gHookManager;