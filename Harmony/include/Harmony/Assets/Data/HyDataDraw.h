/**************************************************************************
*	HyDataDraw.h
*
*	Harmony Engine
*	Copyright (c) 2013 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __HyDataDraw_h__
#define __HyDataDraw_h__

#include "Afx/HyStdAfx.h"

#include "Assets/Data/IHyData.h"
#include "Assets/Containers/HyAtlasContainer.h"
#include "Renderer/Components/IHyShader.h"

class HyDataDraw
{
	friend class HyAssets;

	HyLoadState				m_eLoadState;

	std::set<uint32>		m_AssociatedAtlasIds;
	std::set<IHyShader *>	m_AssociatedShaders;

public:
	HyDataDraw();
	virtual ~HyDataDraw();

	HyLoadState GetLoadState();
	virtual void SetLoadState(HyLoadState eState);

	int32 GetShaderId();
	bool IsIncrementRenderRefs();

	virtual void SetLoadState(HyLoadState eState);

	HyAtlasGroup *RequestTexture(uint32 uiAtlasGroupId);
	const std::set<HyAtlasGroup *> &GetAssociatedAtlases();
	const std::set<IHyShader *> &GetAssociatedShaders();
};

#endif /* __HyDataDraw_h__ */
