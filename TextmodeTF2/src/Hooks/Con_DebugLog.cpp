#include "../SDK/SDK.h"

MAKE_HOOK(Con_DebugLog, G::Con_DebugLogAddr, void,
		  const char *fmt, ...)
{
	va_list argptr; 
	char data[4096];
    
    va_start(argptr, fmt);
    _vsnprintf_s(data, sizeof(data), fmt, argptr);
    va_end(argptr);

	// Write to our own file
	SDK::OutputFile(CONSOLE_LOG_FILE, data);
}