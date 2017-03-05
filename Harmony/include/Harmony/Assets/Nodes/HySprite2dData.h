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

#include "Assets/Nodes/IHyNodeData.h"
#include "Assets/HyAssets.h"

struct HySprite2dFrame
{
	HyAtlas *					pAtlas;
	const HyRectangle<float>	rSRC_RECT;
	const glm::ivec2			vOFFSET;
	const float					fDURATION;

	HySprite2dFrame(HyAtlas *pAtlas,
					float fSrcLeft,
					float fSrcTop,
					float fSrcRight,
					float fSrcBot,
					glm::ivec2 vOffset,
					float fDuration) :	pAtlas(pAtlas),
										rSRC_RECT(fSrcLeft, fSrcTop, fSrcRight, fSrcBot),
										vOFFSET(vOffset),
										fDURATION(fDuration)
	{ }

	uint32 GetGfxApiHandle() const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HySprite2dData : public IHyNodeData
{
public:
	HySprite2dData(const std::string &sPath, const jsonxx::Value &dataValueRef, HyAssets &assetsRef);
	virtual ~HySprite2dData();

	class AnimState
	{
	public:
		const std::string	m_sNAME;
		const bool			m_bLOOP;
		const bool			m_bREVERSE;
		const bool			m_bBOUNCE;
		const float			m_fDURATION;

		HySprite2dFrame *	m_pFrames;
		const uint32		m_uiNUMFRAMES;

		std::set<uint32>	m_UsedAtlasIds;

		AnimState(std::string sName, bool bLoop, bool bReverse, bool bBounce, float fDuration, jsonxx::Array &frameArray, HyAssets &assetsRef);
		~AnimState();

		const HySprite2dFrame &GetFrame(uint32 uiFrameIndex);
	};
	AnimState *			m_pAnimStates;
	uint32				m_uiNumStates;

	uint32 GetNumStates() const;
	const AnimState &GetState(uint32 uiAnimStateIndex) const;
	const HySprite2dFrame &GetFrame(uint32 uiAnimStateIndex, uint32 uiFrameIndex) const;

	virtual void AppendRequiredAtlasIds(std::set<uint32> &requiredAtlasIdsOut) override;
};

#endif /* __HySprite2dData_h__ */
