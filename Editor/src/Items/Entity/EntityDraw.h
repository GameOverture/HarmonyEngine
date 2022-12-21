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
	class TransformWidget : public HyEntity2d
	{
		HyPrimitive2d				m_Box;
		HyPrimitive2d				m_GrabOutline[8];
		HyPrimitive2d				m_GrabFill[8];

	public:
		TransformWidget(HyEntity2d *pParent) :
			HyEntity2d(pParent)
		{
			ChildAppend(m_Box);

			for(uint i = 0; i < 8; ++i)
			{
				ChildAppend(m_GrabOutline[i]);
				ChildAppend(m_GrabFill[i]);
			}
		}

		void Set(IHyDrawable2d &nodeRef, float fLineThickness)
		{
			//m_Box.shape = nodeRef.GetLocalBoundingVolume();
			m_Box.SetLineThickness(fLineThickness);
			//HyAssert(m_Box.GetShapeType() == HYSHAPE_Polygon && static_cast<const b2PolygonShape *>(m_Box.shape.GetB2Shape())->m_count == 4, "TransformWidget got a shape that wasn't a box polygon");

			for(uint i = 0; i < 4; ++i)
			{
				//const b2PolygonShape *pB2Shape = static_cast<const b2PolygonShape *>(m_Box.GetB2Shape());
				//m_GrabOutline[i].pos.Set(pB2Shape->m_vertices[i].x, pB2Shape->m_vertices[i].y);
				//m_GrabFill[i].pos.Set(pB2Shape->m_vertices[i].x, pB2Shape->m_vertices[i].y);
			}
		}
	};
	struct ChildNode
	{
		IHyLoadable2d *				m_Child;
		HyPrimitive2d				m_Shape;
	};

public:
	EntityDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef);
	virtual ~EntityDraw();

	//void Sync();

protected:
	virtual void OnApplyJsonData(HyJsonDoc &itemDataDocRef) override;
	virtual void OnShow() override;
	virtual void OnHide() override;
	virtual void OnResizeRenderer() override;
	virtual void OnZoom(HyZoomLevel eZoomLevel) override;
};

#endif // ENTITYDRAW_H
