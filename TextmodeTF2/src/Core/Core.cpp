#include "Core.h"

#include "../SDK/SDK.h"
#include "../BytePatches/BytePatches.h"
#include <filesystem>

#define LOAD_WAIT 0 - m_bTimeout
#define LOAD_FAIL -1
#define CHECK(x, sFailMessage) if (x == LOAD_FAIL) {m_bUnload = true; SDK::Output("TextmodeTF2", sFailMessage); return;}

void CCore::AppendFailText(const char* sMessage, bool bCritical)
{
	if (!m_bTimeout && !bCritical)
		return;

	ssFailStream << std::format("{}\n", sMessage);
	SDK::Output(sMessage);
}

void CCore::AppendSuccessText(const char* sFunction, const char* sMessage)
{
	SDK::Output(sFunction, sMessage);
}

int CCore::LoadFilesystem()
{
	G::IFileSystemAddr = reinterpret_cast<uintptr_t>(U::Memory.FindInterface("filesystem_stdio.dll", "VFileSystem022"));
	if (!G::IFileSystemAddr)
		return LOAD_WAIT;

	G::IBaseFileSystemAddr = G::IFileSystemAddr + 0x8;

	static std::vector<const char*> vFilesystemHooks
	{
		"IFileSystem_FindFirst", "IFileSystem_FindNext",
		"IFileSystem_AsyncReadMultiple", "IFileSystem_OpenEx",
		"IFileSystem_ReadFileEx", "IFileSystem_AddFilesToFileCache",
		"IBaseFileSystem_Open", "IBaseFileSystem_Precache",
		"IBaseFileSystem_ReadFile"
	};

	for (auto cHook : vFilesystemHooks)
		if (!U::Hooks.Initialize(cHook))
			return LOAD_FAIL;

	return m_bFilesystemLoaded = true;
}

int CCore::LoadEngine()
{
	static bool bTextmodeInit{ false };
	if(!G::g_bTextModeAddr)
		G::g_bTextModeAddr = U::Memory.FindSignature("engine.dll", "88 15 ? ? ? ? 48 8B 4E");
	if (!bTextmodeInit && G::g_bTextModeAddr)
		*reinterpret_cast<bool*>(U::Memory.RelToAbs(G::g_bTextModeAddr, 0x2)) = bTextmodeInit = true;

	static bool bStartupGraphicHookInit{ false };
	if (!G::CVideoModeCommon_SetupStartupGraphicAddr)
		G::CVideoModeCommon_SetupStartupGraphicAddr = U::Memory.FindSignature("engine.dll", "48 8B C4 48 89 58 ? 48 89 70 ? 48 89 78 ? 55 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 48 8B F9");
	if (!bStartupGraphicHookInit && G::CVideoModeCommon_SetupStartupGraphicAddr)
	{
		if (!U::Hooks.Initialize("CVideoModeCommon_SetupStartupGraphic"))
			return LOAD_FAIL;
		bStartupGraphicHookInit = true;
	}

	static bool bInsecureBypassInit{ false };
	if (!bInsecureBypassInit)
	{
		if (!G::g_bAllowSecureServersAddr || !G::Host_IsSecureServerAllowedAddr)
		{
			G::g_bAllowSecureServersAddr = U::Memory.FindSignature("engine.dll", "40 88 35 ? ? ? ? 40 84 FF");
			G::Host_IsSecureServerAllowedAddr = U::Memory.FindSignature("engine.dll", "48 83 EC ? FF 15 ? ? ? ? 48 8D 15 ? ? ? ? 48 8B C8 4C 8B 00 41 FF 50 ? 85 C0 75");
		}
		if (G::g_bAllowSecureServersAddr && G::Host_IsSecureServerAllowedAddr)
		{
			if (!U::Hooks.Initialize("Host_IsSecureServerAllowed"))
				return LOAD_FAIL;
			bInsecureBypassInit = true;
		}
	}

	static bool bCon_DebugLogInit{ false };
	if (!bCon_DebugLogInit)
	{
		if (!G::Con_DebugLogAddr)
			G::Con_DebugLogAddr = U::Memory.FindSignature("engine.dll", "48 89 4C 24 ? 48 89 54 24 ? 4C 89 44 24 ? 4C 89 4C 24 ? 57");
		if (G::Con_DebugLogAddr)
		{
			if (!U::Hooks.Initialize("Con_DebugLog"))
				return LOAD_FAIL;
			bCon_DebugLogInit = true;
		}
	}

	static bool bBytePatchesInit{ false };
	if (!bBytePatchesInit && U::BytePatches.Initialize("engine"))
		bBytePatchesInit = true;


	if(!bStartupGraphicHookInit || !bInsecureBypassInit || !bTextmodeInit || !bCon_DebugLogInit || !bBytePatchesInit)
		return LOAD_WAIT;

	return m_bEngineLoaded = true;
}

int CCore::LoadMatSys()
{
	if (!U::Interfaces.Initialize())
		return LOAD_WAIT;

	I::MaterialSystem->SetInStubMode(true);

	static std::vector<const char*> vMatSystemHooks
	{
		"IMaterialSystem_CreateRenderTargetTexture", "IMaterialSystem_CreateNamedRenderTargetTextureEx",
		"IMaterialSystem_CreateNamedRenderTargetTexture", "IMaterialSystem_CreateNamedRenderTargetTextureEx2",
		"IMaterialSystem_SwapBuffers"
	};

	for (auto cHook : vMatSystemHooks)
		if (!U::Hooks.Initialize(cHook))
			return LOAD_FAIL;

	return m_bMatSysLoaded = true;
}

int CCore::LoadClient()
{
	if (!U::BytePatches.Initialize("client"))
		return LOAD_WAIT;

	return m_bClientLoaded = true;
}

void CCore::Load()
{
	G::CurrentPath = std::filesystem::current_path().string() + "\\TextmodeTF2";
	char* cBotID = nullptr;
	if (_dupenv_s(&cBotID, nullptr, "BOTID") == 0 && cBotID)
	{
		char* cAppdataPath = nullptr;
		if (_dupenv_s(&cAppdataPath, nullptr, "LOCALAPPDATA") == 0 && cAppdataPath)
		{
			G::AppdataPath = std::format("{}\\{}_{}\\", cAppdataPath, "Amalgam\\Textmode", cBotID);
			free(cAppdataPath);
		}
		free(cBotID);
	}
	if (G::AppdataPath.size())
	{
		try
		{
			if (!std::filesystem::exists(G::AppdataPath))
				std::filesystem::create_directories(G::AppdataPath);

			std::ofstream resetFile(G::AppdataPath + TEXTMODE_LOG_FILE);
			resetFile.close();
			resetFile.open(G::AppdataPath + CONSOLE_LOG_FILE);
			resetFile.close();
		}
		catch (...) {}
	}

	do
	{
		// if all required modules are loaded and we still fail stop trying to load
		m_bTimeout = GetModuleHandleA("filesystem_stdio.dll") &&
			GetModuleHandleA("engine.dll") &&
			GetModuleHandleA("materialsystem.dll") &&
			GetModuleHandleA("client.dll");

		int iFilesystem = m_bFilesystemLoaded ? 1 : LoadFilesystem();
		CHECK(iFilesystem, "Failed to load file system")
		int iEngine = m_bEngineLoaded ? 1 : LoadEngine();
		CHECK(iEngine, "Failed to load engine")
		int iMatSys = m_bMatSysLoaded ? 1 : LoadMatSys();
		CHECK(iMatSys, "Failed to load material system")
		int iClient = m_bClientLoaded ? 1 : LoadClient();
		CHECK(iClient, "Failed to load client")
	}
	while (!m_bFilesystemLoaded || !m_bEngineLoaded || !m_bMatSysLoaded || !m_bClientLoaded);

	SDK::Output("TextmodeTF2", std::format("Loaded in {} seconds", SDK::PlatFloatTime()).c_str());
}

void CCore::Loop()
{
	while (true)
	{
		if (m_bUnload)
			break;

		Sleep(3600000);
	}
}

void CCore::Unload()
{	
	U::Hooks.Unload();
	U::BytePatches.Unload();

	SDK::Output("TextmodeTF2", "Failed to load");

	ssFailStream << "\nCtrl + C to copy. Logged to TextmodeTF2\\fail_log.txt. \n";
	ssFailStream << "Built @ " __DATE__ ", " __TIME__;
	ssFailStream << "\n\n\n\n";
	std::ofstream file;
	file.open(G::CurrentPath + "\\fail_log.txt", std::ios_base::app);
	file << ssFailStream.str();
	file.close();
	return;
}