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

#include "Creator/Data/IHyData.h"

class HySprite2dData : public IHyData
{
	friend class HyFactory<HySprite2dData>;

	bool				m_bIsSimpleSprite;

	struct AnimState
	{
		std::string		sName;
		bool			bLoop;
		bool			bReverse;
		bool			bBounce;

		struct Frame
		{
			int			m_iTextureIndex;
			int			m_iRectIndex;

			vec2		m_vOffset;
			float		m_fRot;
			vec2		m_vScale;	// negative values will flip image
			float		m_fDur;
		};

		Frame *			m_pFrames;
		int				m_iNumFrames;
	};

	int					m_iNumStates;
	AnimState *			m_pAnimStates;

	// Only allow HyFactory instantiate
	HySprite2dData(const std::string &sPath);

public:
	virtual ~HySprite2dData(void);

	bool IsSimpleSprite()			{ return m_bIsSimpleSprite; }

	virtual void DoFileLoad();
	virtual void OnGfxLoad(IHyRenderer &gfxApi);
	virtual void OnGfxRemove(IHyRenderer &gfxApi);
};

#endif /* __HySprite2dData_h__ */
