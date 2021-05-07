/**************************************************************************
 *	AtlasDraw.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ATLASDRAW_H
#define ATLASDRAW_H

#include "IManagerDraw.h"
#include "AtlasModel.h"

class AtlasDraw : public IManagerDraw
{
	bool									m_bIsMouseOver;
	
	// TODO: Test whether splitting HyTexturedQuad's into multiple maps has any lookup/insert benefit, rather than one massive QMap
	struct TextureEnt : public HyEntity2d
	{
		QMap<QUuid, HyTexturedQuad2d *>		m_FrameUuidMap;
		
		TextureEnt()
		{ }
	};
	QList<TextureEnt *>						m_MasterList;
	
	QList<HyTexturedQuad2d *>				m_SelectedTexQuadList;

	HyPrimitive2d							m_HoverBackground;
	HyPrimitive2d							m_HoverStrokeInner;
	HyPrimitive2d							m_HoverStrokeOutter;
	HyTexturedQuad2d *						m_pHoverTexQuad;

public:
	AtlasDraw(AtlasModel &atlasManagerModelRef);
	virtual ~AtlasDraw();

	virtual void SetHover(TreeModelItemData *pHoverItem) override;
	virtual void SetSelected(QList<AssetItemData *> selectedList) override;

	virtual void OnShow() override;
	virtual void OnHide() override;
	virtual void OnResizeRenderer() override;

	virtual void OnDrawUpdate() override;
};

#endif // ATLASDRAW_H
