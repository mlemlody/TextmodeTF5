#include "../SDK/SDK.h"

MAKE_HOOK(IMDLCache_ProcessDataIntoCache, U::Memory.GetVFunc(reinterpret_cast<void*>(G::IMDLCache), 33), bool,
		  void* rcx, MDLHandle_t handle, MDLCacheDataType_t type, void* pData, int nDataSize, bool bAsync)
{
	if (type == MDLCACHE_VERTEXES)
	{
		const char* pszModelName = G::IMDLCache->GetModelName(handle);
		if (pszModelName && SDK::BlacklistFile(pszModelName))
			return true;
	}

	return CALL_ORIGINAL(rcx, handle, type, pData, nDataSize, bAsync);
}
