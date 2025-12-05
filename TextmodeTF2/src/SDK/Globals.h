#pragma once
#include "Definitions/Definitions.h"
#include "../Utils/Signatures/Signatures.h"
#include "../Utils/Memory/Memory.h"

#define TEXTMODE_LOG_FILE "TextmodeLog.log"
#define CONSOLE_LOG_FILE "Console.log"

namespace G
{
	inline std::string CurrentPath{};
	inline std::string AppdataPath{};
	inline uintptr_t CVideoModeCommon_SetupStartupGraphicAddr{};
	inline uintptr_t IFileSystemAddr{};
	inline uintptr_t IBaseFileSystemAddr{};
	inline uintptr_t g_bTextModeAddr{};
	inline uintptr_t g_bAllowSecureServersAddr{};
	inline uintptr_t Host_IsSecureServerAllowedAddr{};
	inline uintptr_t Client_CreateEntityByNameAddr{};
	inline uintptr_t Con_DebugLogAddr{};
};