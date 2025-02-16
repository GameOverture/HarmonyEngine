/**************************************************************************
 *	HySpriteData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HySpriteData_h__
#define HySpriteData_h__

#include "Afx/HyStdAfx.h"

#include "Assets/Nodes/IHyNodeData.h"
#include "Assets/HyAssets.h"

struct HySpriteFrame
{
	HyFileAtlas *				pAtlas;
	const HyUvCoord				rSRC_RECT;
	const HyMargins<uint16>		rCROP_MARGINS;
	const glm::ivec2			vOFFSET;
	const float					fDURATION;

	// NOTE: Masks are serialized in LEFT, TOP, RIGHT, BOTTOM order. Each value is uint16 bits. In 0xLLLLTTTTRRRRBBBB order
	HySpriteFrame(HyFileAtlas *pAtlas,
					float fSrcLeft,
					float fSrcTop,
					float fSrcRight,
					float fSrcBot,
					uint64 uiCropMask,
					glm::ivec2 vOffset,
					float fDuration) :	pAtlas(pAtlas),
										rSRC_RECT(fSrcLeft, fSrcBot, fSrcRight, fSrcTop),
										rCROP_MARGINS((uiCropMask >> 48) & 0xFFFF, uiCropMask & 0xFFFF, (uiCropMask >> 16) & 0xFFFF, (uiCropMask >> 32) & 0xFFFF),
										vOFFSET(vOffset),
										fDURATION(fDuration)
	{ }

	HyTextureHandle GetGfxApiHandle() const;
	bool IsAtlasValid() const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HySpriteData : public IHyNodeData
{
public:
	HySpriteData(const HyNodePath &nodePath, HyJsonObj itemDataObj, HyAssets &assetsRef);
	virtual ~HySpriteData();

	class AnimState
	{
	public:
		const bool			m_bLOOP;
		const bool			m_bREVERSE;
		const bool			m_bBOUNCE;
		const float			m_fDURATION;

		HySpriteFrame *		m_pFrames;
		const uint32		m_uiNUMFRAMES;

		AnimState(bool bLoop, bool bReverse, bool bBounce, float fDuration, HyFilesManifest &requiredAtlasIndicesRef, HyJsonArray frameArray, HyAssets &assetsRef);
		~AnimState();

		const HySpriteFrame *GetFrame(uint32 uiFrameIndex) const;
	};
	AnimState *				m_pAnimStates;

	const AnimState &GetState(uint32 uiAnimStateIndex) const;
	const HySpriteFrame *GetFrame(uint32 uiAnimStateIndex, uint32 uiFrameIndex) const;
};

#endif /* HySpriteData_h__ */
