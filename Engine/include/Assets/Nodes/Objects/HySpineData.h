/**************************************************************************
 *	HySpineData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HySpineData_h__
#define HySpineData_h__

#include "Afx/HyInteropAfx.h"
#include "Assets/Nodes/IHyNodeData.h"
#include "Assets/Files/HyFileAtlas.h"

struct HySpineAtlas
{
	std::string					m_sName;
	HyFileAtlas *				m_pAtlas;
	HyTextureHandle				m_hGfxApiHandle;
	const HyUvCoord				m_rSRC_RECT;

	// GUI temp data constructor
	HySpineAtlas(std::string sName, HyTextureHandle hGfxApiHandle) :
		m_sName(sName),
		m_pAtlas(nullptr),
		m_hGfxApiHandle(hGfxApiHandle),
		m_rSRC_RECT(0.0f, 1.0f, 1.0f, 0.0f)
	{ }

	// Normal runtime files constructor
	HySpineAtlas(std::string sName, HyFileAtlas *pAtlas, float fSrcLeft, float fSrcTop, float fSrcRight, float fSrcBot) :
		m_sName(sName),
		m_pAtlas(pAtlas),
		m_hGfxApiHandle(HY_UNUSED_HANDLE),
		m_rSRC_RECT(fSrcLeft, fSrcBot, fSrcRight, fSrcTop)
	{ }

	HyTextureHandle GetTexHandle() const
	{
		if(m_pAtlas)
			return m_pAtlas->GetTextureHandle();
		else
			return m_hGfxApiHandle;
	}

	struct UVRect {
		float u_min; // left
		float v_min; // bottom
		float u_max; // right
		float v_max; // top
	};

	glm::vec2 ConvertUVs(const glm::vec2 &uv) const
	{
		glm::vec2 uvOut;
		uvOut.x = uv.x * (m_rSRC_RECT.right - m_rSRC_RECT.left) + m_rSRC_RECT.left;
		uvOut.y = uv.y * (m_rSRC_RECT.bottom - m_rSRC_RECT.top) + m_rSRC_RECT.top;
		return uvOut;
	}
};

#ifdef HY_USE_SPINE // Custom engine class extensions to assist spine-cpp with loading
	class HySpineTextureLoader : public spine::TextureLoader
	{
	public:
		std::vector<HySpineAtlas> m_SubAtlasList;

		HySpineTextureLoader();
		virtual void load(spine::AtlasPage &page, const spine::String &path) override;
		virtual void unload(void *pTexture) override;
	};
#endif

class HySpineData : public IHyNodeData
{
#ifdef HY_USE_SPINE
	HySpineTextureLoader			m_SpineTextureLoader;
	spine::Atlas *					m_pAtlasData;
	spine::SkeletonData *			m_pSkeletonData;
	spine::AnimationStateData *		m_pAnimStateData; // Crossfade times set in the editor
#endif

	struct StateData
	{
		// Animation Setup


		// Skin Setup
	};

public:
	HySpineData(const HyNodePath &nodePath, HyJsonObj itemDataObj, HyAssets &assetsRef);
	virtual ~HySpineData();

	HyTextureHandle GetTexHandle(uint32 uiAtlasIndex) const;

#ifdef HY_USE_SPINE
	spine::SkeletonData *GetSkeletonData() const;
	spine::AnimationStateData *GetAnimationStateData() const;
#endif

	// Sets every combination of animation blend to this amount
	void AnimInitBlend(float fInterpDur);

	void AnimInitBlend(const char *szAnimFrom, const char *szAnimTo, float fInterpDur);
	void AnimInitBlend(uint32 uiAnimIdFrom, uint32 uiAnimIdTo, float fInterpDur);
};

#endif /* HySpineData_h__ */
