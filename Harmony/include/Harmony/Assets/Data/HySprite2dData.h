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

#include "Assets/Data/IHy2dData.h"
#include "Assets/HyFactory.h"
#include "Assets/HyTextures.h"

struct HySprite2dFrame
{
	HyAtlasGroup *				pAtlasGroup;
	const uint32				uiATLAS_GROUP_TEXTURE_INDEX;
	const HyRectangle<float>	rSRC_RECT;
	const glm::ivec2			vOFFSET;
	const float					fDURATION;

	HySprite2dFrame(HyAtlasGroup *pAtlasGrp,
					uint32 uiAtlasGroupTextureIndex,
					float fSrcLeft,
					float fSrcTop,
					float fSrcRight,
					float fSrcBot,
					glm::ivec2 vOffset,
					float fDuration) :	pAtlasGroup(pAtlasGrp),
										uiATLAS_GROUP_TEXTURE_INDEX(uiAtlasGroupTextureIndex),
										rSRC_RECT(fSrcLeft, fSrcTop, fSrcRight, fSrcBot),
										vOFFSET(vOffset),
										fDURATION(fDuration)
	{ }

	uint32 GetGfxApiHandle() const;
	uint32 GetActualTextureIndex() const;
};

class HySprite2dData : public IHyData
{
	friend class HyFactory<HySprite2dData>;

	// Only allow HyFactory instantiate
	HySprite2dData(const std::string &sPath, const jsonxx::Value &dataValueRef);

public:
	class AnimState
	{
	public:
		const std::string	m_sNAME;
		const bool			m_bLOOP;
		const bool			m_bREVERSE;
		const bool			m_bBOUNCE;

		HySprite2dFrame *	m_pFrames;
		const uint32		m_uiNUMFRAMES;

		AnimState(std::string sName, bool bLoop, bool bReverse, bool bBounce, jsonxx::Array &frameArray, HySprite2dData &dataRef);
		~AnimState();

		const HySprite2dFrame &GetFrame(uint32 uiFrameIndex);
	};
	AnimState *			m_pAnimStates;
	uint32				m_uiNumStates;

	virtual ~HySprite2dData();

	uint32 GetNumStates() const;
	const AnimState &GetState(uint32 uiAnimStateIndex) const;
	const HySprite2dFrame &GetFrame(uint32 uiAnimStateIndex, uint32 uiFrameIndex) const;
};

#endif /* __HySprite2dData_h__ */
