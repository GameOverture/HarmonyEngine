/**************************************************************************
 *	HySprite2dData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HySprite2dData_h__
#define HySprite2dData_h__

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

	HyTextureHandle GetGfxApiHandle() const;
	bool IsValid() const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HySprite2dData : public IHyNodeData
{
public:
	HySprite2dData(const std::string &sPath, const jsonxx::Object &itemDataObjRef, HyAssets &assetsRef);
	virtual ~HySprite2dData();

	class AnimState
	{
	public:
		const bool			m_bLOOP;
		const bool			m_bREVERSE;
		const bool			m_bBOUNCE;
		const float			m_fDURATION;

		HySprite2dFrame *	m_pFrames;
		const uint32		m_uiNUMFRAMES;

		AnimState(bool bLoop, bool bReverse, bool bBounce, float fDuration, HyFilesManifest &requiredAtlasIndicesRef, jsonxx::Array &frameArray, HyAssets &assetsRef);
		~AnimState();

		const HySprite2dFrame &GetFrame(uint32 uiFrameIndex) const;
	};
	AnimState *			m_pAnimStates;
	uint32				m_uiNumStates;

	uint32 GetNumStates() const;
	const AnimState &GetState(uint32 uiAnimStateIndex) const;
	const HySprite2dFrame &GetFrame(uint32 uiAnimStateIndex, uint32 uiFrameIndex) const;
};

#endif /* HySprite2dData_h__ */
