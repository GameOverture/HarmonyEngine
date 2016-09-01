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

	struct AnimState
	{
		const std::string	sNAME;
		const bool			bLOOP;
		const bool			bREVERSE;
		const bool			bBOUNCE;

		struct Frame
		{
			HyAtlasGroup *				pAtlasGroup;
			const HyRectangle<float>	rSRC_RECT;
			const uint32				uiTEXTUREINDEX;

			const glm::vec2		vOFFSET;
			const float			fDURATION;

			Frame(HyAtlasGroup *pAtlasGrp,
				  uint32 uiTextureIndex,
				  float fSrcLeft,
				  float fSrcTop,
				  float fSrcRight,
				  float fSrcBot,
				  glm::vec2 vOffset,
				  float fRotation,
				  glm::vec2 vScale,
				  float fDuration) :	pAtlasGroup(pAtlasGrp),
										uiTEXTUREINDEX(uiTextureIndex),
										rSRC_RECT(fSrcLeft, fSrcTop, fSrcRight, fSrcBot),
										vOFFSET(vOffset),
										fDURATION(fDuration)
			{ }
		};
		Frame *			pFrames;
		const uint32	uiNUMFRAMES;

		AnimState(std::string sName, bool bLoop, bool bReverse, bool bBounce, jsonxx::Array &frameArray, HySprite2dData &dataRef);
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
