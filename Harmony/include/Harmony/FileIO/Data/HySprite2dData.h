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

class HySprite2dData : public IHyData
{
	friend class HyFactory<HySprite2dData>;

	struct AnimState
	{
		std::string		sName;
		bool			bLoop;
		bool			bReverse;
		bool			bBounce;

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
	};
	AnimState *			m_pAnimStates;
	int					m_iNumStates;

	// Only allow HyFactory instantiate
	HySprite2dData(const std::string &sPath);

public:
	virtual ~HySprite2dData(void);

	virtual void DoFileLoad(HyAtlasManager &atlasManagerRef);
	virtual void OnGfxLoad(IHyRenderer &gfxApi);
	virtual void OnGfxRemove(IHyRenderer &gfxApi);
};

#endif /* __HySprite2dData_h__ */
