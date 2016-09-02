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

#include "Assets/Data/IHyData2d.h"
#include "Assets/HyFactory.h"
#include "Assets/HyTextures.h"

#include "Utilities/jsonxx.h"

class HySprite2dData : public IHyData2d
{
	friend class HyFactory<HySprite2dData>;

	class AnimState
	{
		const std::string	m_sNAME;
		const bool			m_bLOOP;
		const bool			m_bREVERSE;
		const bool			m_bBOUNCE;

		class Frame
		{
			HyAtlasGroup *				m_pAtlasGroup;
			const HyRectangle<float>	m_rSRC_RECT;
			const uint32				m_uiTEXTUREINDEX;

			const glm::vec2				m_vOFFSET;
			const float					m_fDURATION;

			Frame(HyAtlasGroup *pAtlasGrp,
				  uint32 uiTextureIndex,
				  float fSrcLeft,
				  float fSrcTop,
				  float fSrcRight,
				  float fSrcBot,
				  glm::vec2 vOffset,
				  float fRotation,
				  glm::vec2 vScale,
				  float fDuration) :	m_pAtlasGroup(pAtlasGrp),
										m_uiTEXTUREINDEX(uiTextureIndex),
										m_rSRC_RECT(fSrcLeft, fSrcTop, fSrcRight, fSrcBot),
										m_vOFFSET(vOffset),
										m_fDURATION(fDuration)
			{ }
		};
		Frame *			m_pFrames;
		const uint32	m_uiNUMFRAMES;

	public:
		AnimState(std::string sName, bool bLoop, bool bReverse, bool bBounce, jsonxx::Array &frameArray, HySprite2dData &dataRef);
		~AnimState();
	};
	AnimState *			m_pAnimStates;
	uint32				m_uiNumStates;

	// Only allow HyFactory instantiate
	HySprite2dData(const std::string &sPath);

public:
	virtual ~HySprite2dData(void);

	virtual void DoFileLoad();
};

#endif /* __HySprite2dData_h__ */
