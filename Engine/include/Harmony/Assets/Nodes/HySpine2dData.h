/**************************************************************************
 *	HySpine2dData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HySpine2dData_h__
#define HySpine2dData_h__

#include "Afx/HyInteropAfx.h"
#include "Assets/Nodes/IHyNodeData.h"

class HySpine2dData : public IHyNodeData
{
	//class HySpineTextureLoader : public spine::TextureLoader
	//{
	//public:
	//	HySpineTextureLoader() { }
	//	virtual ~HySpineTextureLoader() { }

	//	// Called when the atlas loads the texture of a page.
	//	virtual void load(spine::AtlasPage &page, const spine::String &path) override {
	//		HyFileAtlas Texture *texture = engine_loadTexture(path);

	//		// if texture loading failed, we simply return.
	//		if(!texture) return;

	//		// store the Texture on the rendererObject so we can
	//		// retrieve it later for rendering.
	//		page.setRendererObject(texture);

	//		// store the texture width and height on the spAtlasPage
	//		// so spine-c can calculate texture coordinates for
	//		// rendering.
	//		page. .setWidth(texture->width);
	//		page.setHeight(texture->height);
	//	}

	//	// Called when the atlas is disposed and itself disposes its atlas pages.
	//	virtual void unload(void *texture) {
	//		// the texture parameter is the texture we stored in the page via page->setRendererObject()
	//		engine_disposeTexture(texture);
	//	}
	//};
	spine::Atlas *					m_pAtlasData;
	spine::SkeletonData *			m_pSkeletonData;
	spine::AnimationStateData *		m_pAnimStateData;

public:
	HySpine2dData(const std::string &sPath, HyJsonObj itemDataObj, HyAssets &assetsRef);
	virtual ~HySpine2dData();

	//spSkeletonData *GetSkeletonData()								{ return m_SpineSkeletonData; }

	// Sets every combination of animation blend to this amount
	void AnimInitBlend(float fInterpDur);

	void AnimInitBlend(const char *szAnimFrom, const char *szAnimTo, float fInterpDur);
	void AnimInitBlend(uint32 uiAnimIdFrom, uint32 uiAnimIdTo, float fInterpDur);
};

#endif /* HySpine2dData_h__ */
