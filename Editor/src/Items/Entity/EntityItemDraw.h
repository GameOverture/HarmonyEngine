/**************************************************************************
*	EntityItemDraw.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2023 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef ENTITYITEMDRAW_H
#define ENTITYITEMDRAW_H

#include "IDraw.h"

class EntityItemDraw
{
	HyGuiItemType							m_eGuiType;
	QUuid									m_ItemUuid;

	IHyLoadable2d *							m_pChild;
	HyShape2d *								m_pShape;

	bool									m_bStale;

public:
	EntityItemDraw(HyGuiItemType eGuiType, QUuid uuid, HyEntity2d *pParent);
	virtual ~EntityItemDraw();
	HyGuiItemType GetGuiType() const {
		return m_eGuiType;
	}
	IHyLoadable2d *GetNodeChild() const {
		return m_pChild;
	}
	HyShape2d *GetShape() const {
		return m_pShape;
	}
	const QUuid &GetUuid() const {
		return m_ItemUuid;
	}
	bool IsStale() const {
		return m_bStale;
	}
	void SetStale() {
		m_bStale = true;
	}
	void RefreshJson(HyCamera2d *pCamera, QJsonObject childObj); // Clears stale flag

protected:
	void RefreshOverrideData();
};

#endif // ENTITYITEMDRAW_H
