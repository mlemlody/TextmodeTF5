#include "../SDK/SDK.h"

MAKE_HOOK(IBaseFileSystem_Open, U::Memory.GetVFunc(reinterpret_cast<void*>(G::IBaseFileSystemAddr), 2), FileHandle_t,
		  void* rcx, const char* pFileName, const char* pOptions, const char* pathID)
{
	if (SDK::BlacklistFile(pFileName))
		return nullptr;

	return CALL_ORIGINAL(rcx, pFileName, pOptions, pathID);
}

MAKE_HOOK(IBaseFileSystem_Precache, U::Memory.GetVFunc(reinterpret_cast<void*>(G::IBaseFileSystemAddr), 9), bool,
		  void* rcx, const char* pFileName, const char* pPathID)
{
	return true;
}

class CUtlBuffer;

MAKE_HOOK(IBaseFileSystem_ReadFile, U::Memory.GetVFunc(reinterpret_cast<void*>(G::IBaseFileSystemAddr), 14), bool,
		  void* rcx, const char* pFileName, const char* pPath, CUtlBuffer& buf, int nMaxBytes, int nStartingByte, FSAllocFunc_t pfnAlloc)
{
	if (SDK::BlacklistFile(pFileName))
		return false;

	return CALL_ORIGINAL(rcx, pFileName, pPath, buf, nMaxBytes, nStartingByte, pfnAlloc);
}

MAKE_HOOK(IBaseFileSystem_FileExists, U::Memory.GetVFunc(reinterpret_cast<void*>(G::IBaseFileSystemAddr), 10), bool,
		  void* rcx, const char* pFileName, const char* pPathID)
{
	if (SDK::BlacklistFile(pFileName))
		return false;

	return CALL_ORIGINAL(rcx, pFileName, pPathID);
}