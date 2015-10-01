/**************************************************************************
*	HyTexturedQuad2dData.h
*
*	Harmony Engine
*	Copyright (c) 2013 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __HyTexturedQuad2dData_h__
#define __HyTexturedQuad2dData_h__

#include "Creator\Data\IHyData.h"

class HyTexturedQuad2dData : public IHyData
{
	friend class HyFactory<HyTexturedQuad2dData>;

	HyTexture *				m_pTexture;

	// Only allow HyFactory instantiate
	HyTexturedQuad2dData(const std::string &sPath);

public:
	virtual ~HyTexturedQuad2dData();

	const HyTexture *GetTexture() const;

	virtual void DoFileLoad();
	virtual void OnGfxLoad(IHyRenderer &gfxApi);
	virtual void OnGfxRemove(IHyRenderer &gfxApi);
};

#endif /* __HyTexturedQuad2dData_h__ */
