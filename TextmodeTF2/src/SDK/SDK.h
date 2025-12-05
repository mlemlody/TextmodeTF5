#pragma once

#include "Globals.h"
#include "../Utils/Feature/Feature.h"
#include "Definitions/Interfaces.h"
#include "../Utils/Signatures/Signatures.h"
#include "../Utils/Interfaces/Interfaces.h"
#include "../Utils/Hooks/Hooks.h"
#include "../Utils/Memory/Memory.h"
#include <intrin.h>
#include <format>
#include <exception>
#include <fstream>

namespace SDK
{
	void Output(const char* cFunction, const char* cLog = nullptr, bool bLogFile = true, int iMessageBox = -1);
	void OutputFile(const char* cOutputFileName, const char* cMsg);
	bool BlacklistFile(const char* cFileName);
	double PlatFloatTime();
}