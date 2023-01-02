/**************************************************************************
*	EntityDraw.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef ENTITYDRAW_H
#define ENTITYDRAW_H

#include "IDraw.h"
#include "EntityModel.h"

class EntityDraw : public IDraw
{
	class ChildWidget : public HyEntity2d
	{
		HyGuiItemType							m_eGuiType;
		QUuid									m_ItemUuid;

		TransformCtrl							m_Transform;
		IHyLoadable2d *							m_pChild;

		bool									m_bStale;

	public:
		ChildWidget(HyGuiItemType eGuiType, QUuid uuid, HyEntity2d *pParent);
		virtual ~ChildWidget();
		HyGuiItemType GetGuiType() const {
			return m_eGuiType;
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

	class ShapeWidget : public HyEntity2d
	{
		TransformCtrl							m_Transform;
		HyShape2d								m_Shape;

	public:
		ShapeWidget(HyEntity2d *pParent) :
			HyEntity2d(pParent),
			m_Transform(this)
		{
		}
	};
	
	QList<ChildWidget *>						m_ChildWidgetList;
	QList<ShapeWidget *>						m_ShapeWidgetList;

public:
	EntityDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef);
	virtual ~EntityDraw();

	virtual void OnKeyPressEvent(QKeyEvent *pEvent) override;
	virtual void OnKeyReleaseEvent(QKeyEvent *pEvent) override;
	virtual void OnMousePressEvent(QMouseEvent *pEvent) override;
	virtual void OnMouseReleaseEvent(QMouseEvent *pEvent) override;
	virtual void OnMouseWheelEvent(QWheelEvent *pEvent) override;
	virtual void OnMouseMoveEvent(QMouseEvent *pEvent) override;

protected:
	virtual void OnApplyJsonMeta(QJsonObject &itemMetaObj) override;
	virtual void OnShow() override;
	virtual void OnHide() override;
	virtual void OnResizeRenderer() override;
	virtual void OnZoom(HyZoomLevel eZoomLevel) override;

	ChildWidget *FindStaleChild(HyGuiItemType eType, QUuid uuid);
	void DeleteStaleChildren();
};

#endif // ENTITYDRAW_H
