#include "../SDK/SDK.h"

MAKE_HOOK(CParticleSystemMgr_ReadParticleConfigFile, G::CParticleSystemMgr_ReadParticleConfigFileAddr, bool,
		  void* rcx, const char* pFileName, bool bPrecache, bool bDecommitTempMemory)
{
	if (SDK::BlacklistFile(pFileName))
		return true;

	return CALL_ORIGINAL(rcx, pFileName, bPrecache, bDecommitTempMemory);
}
