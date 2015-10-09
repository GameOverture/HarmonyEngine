/**************************************************************************
 *	HySprite2dData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HySprite2dData_h__
#define __HySprite2dData_h__

#include "Afx/HyStdAfx.h"

#include "FileIO/Data/IHyData.h"
#include "FileIO/HyFactory.h"
#include "FileIO/HyAtlasManager.h"

#include "Utilities/jsonxx.h"

class HySprite2dData : public IHyData
{
	friend class HyFactory<HySprite2dData>;

	struct AnimState
	{
		const std::string	sNAME;
		const bool			bLOOP;
		const bool			bREVERSE;
		const bool			bBOUNCE;

		struct Frame
		{
			HyAtlasGroup &	m_AtlasGrpRef;
			int				m_iRectIndex;

			vec2			m_vOffset;
			float			m_fRot;
			vec2			m_vScale;	// negative values will flip image
			float			m_fDur;
		};
		Frame *			m_pFrames;
		int				m_iNumFrames;

		AnimState(std::string sName, bool bLoop, bool bReverse, bool bBounce, jsonxx::Array &frameArray, HyAtlasManager &atlasManagerRef);
	};
	AnimState *			m_pAnimStates;
	uint32				m_uiNumStates;

	// Only allow HyFactory instantiate
	HySprite2dData(const std::string &sPath);

public:
	virtual ~HySprite2dData(void);

	virtual void DoFileLoad(HyAtlasManager &atlasManagerRef);
	virtual void OnGfxLoad(IHyRenderer &gfxApi);
	virtual void OnGfxRemove(IHyRenderer &gfxApi);
};

#endif /* __HySprite2dData_h__ */
