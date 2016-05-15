#include "backTrace.h"

#include "CHack.h"

void sehTranslator(unsigned int code, EXCEPTION_POINTERS *e)
{
	// create a new exception base off of these and then throw it.
	//Log::Error("preparing to throw seh exception!");
	throw sehException(code, e);
}

void destructHack()
{
	extern CHack gHack;

	gHack.~CHack();
}

void terminateHandler()
{
	Log::Error("Termination called!");

	destructHack();

	return;
}

void unexpectedHandler()
{
	Log::Error("Unhandled exception!!");

	destructHack();
	return;
}

LONG WINAPI unhandledSehExceptionHandler(EXCEPTION_POINTERS *e)
{
	Log::Error("An SEH exception\n(name: %s | id=0x%X)\nwas left UNCAUGHT!", "UNKNOWN", e->ExceptionRecord->ExceptionCode);
	return 0;
}
