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

#include "IDraw.h"
#include "IManagerModel.h"

class AtlasDraw : public IDraw
{
	bool									m_bIsMouseOver;
	
	// TODO: Test whether splitting HyTexturedQuad's into multiple maps has any lookup/insert benefit, rather than one massive QMap
	struct TextureEnt : public HyEntity2d
	{
		QMap<QUuid, HyTexturedQuad2d *>		m_FrameUuidMap;
		
		TextureEnt(HyEntity2d *pParent) : HyEntity2d(pParent)
		{ }
	};
	QList<TextureEnt *>						m_MasterList;
	
	QList<HyTexturedQuad2d *>				m_SelectedTexQuadList;

	HyPrimitive2d							m_HoverBackground;
	HyPrimitive2d							m_HoverStrokeInner;
	HyPrimitive2d							m_HoverStrokeOutter;
	HyTexturedQuad2d *						m_pHoverTexQuad;

public:
	AtlasDraw(IManagerModel &atlasManagerModel);
	virtual ~AtlasDraw();

	void SetHover(TreeModelItemData *pHoverItem);
	void SetSelected(QList<AssetItemData *> selectedList);

	void DrawUpdate();

	virtual void OnShow() override;
	virtual void OnHide() override;
	virtual void OnResizeRenderer() override;
};

#endif // ATLASDRAW_H
