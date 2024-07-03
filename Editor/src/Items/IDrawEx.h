/**************************************************************************
 *	IDrawEx.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IDRAWEX_H
#define IDRAWEX_H

#include "Global.h"
#include "IDraw.h"
#include "IDrawItem.h"

struct SnapCandidates
{
	float 						m_fSnapTolerance;
	QSet<float>					m_HorzSet;
	QSet<float>					m_VertSet;

	SnapCandidates() { Clear(); }
	void Clear()
	{
		m_fSnapTolerance = 0.0f;
		m_HorzSet.clear();
		m_VertSet.clear();
	}

	bool IsEmpty() const
	{
		return m_HorzSet.empty() && m_VertSet.empty();
	}
};

// LOGIC FLOW NOTES:
// 1) Input is handled with the standard OnKey*Event and OnMouse*Event() functions, then are delegated out to respective DoMouse*_*() functions
// 2) ShapeEditShape takes precedence over DragState
// 3) DragState/Transform also handles nudging with arrow keys
class IDrawEx : public IDraw
{
protected:
	QList<IDrawExItem *>					m_ItemList;			// Items MUST be dynamically allocated
	QList<IDrawExItem *>					m_SelectedItemList;

	ShapeCtrl								m_DragShape;		// Used for marquee selects and initially adding new shapes

	// Multi transform used when 'm_SelectedItemList' contains more than 1 item. It wraps around all selected items.
	TransformCtrl							m_MultiTransform;
	float									m_fMultiTransformStartRot;

	// Mouse hovering
	IDrawExItem *							m_pCurHoverItem;
	TransformCtrl::GrabPointType			m_eCurHoverGrabPoint;

	// Mouse press/dragging (and arrow key nudging)
	HyTimer									m_PressTimer;
	bool									m_bSelectionHandled; // During mouse release, this bool can signify if selection was already handled by previous logic (or if selection is not necessary)
	glm::vec2								m_ptDragStart;
	glm::vec2								m_ptDragCenter;
	glm::vec2								m_vDragStartSize;
	QPoint									m_vNudgeTranslate;	// Used for nudging selected items with arrow keys

	// Used to help transform (translate, rotate, scale) selected items
	HyEntity2d								m_ActiveTransform;
	QList<glm::mat4>						m_PrevTransformList;

	HyPrimitive2d							m_SnapGuideHorz;
	HyPrimitive2d							m_SnapGuideVert;

public:
	IDrawEx(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef);
	virtual ~IDrawEx();

	QList<IDrawExItem *> GetDrawItemList();

	virtual void ResizeRenderer();

	virtual void OnKeyPressEvent(QKeyEvent *pEvent) override;
	virtual void OnKeyReleaseEvent(QKeyEvent *pEvent) override;

	virtual void OnMouseWheelEvent(QWheelEvent *pEvent) override;

	virtual void OnMouseMoveEvent(QMouseEvent *pEvent) override;
	virtual void OnMousePressEvent(QMouseEvent *pEvent) override;
	virtual void OnMouseReleaseEvent(QMouseEvent *pEvent) override;

	void RefreshTransforms();

protected:
	virtual void OnResizeRenderer() = 0;
	virtual void OnCameraUpdated() override;

	virtual void OnRequestSelection(QList<IDrawExItem *> selectionList) = 0;

	void DoMouseMove(bool bCtrlMod, bool bShiftMod);

	bool SetTransformHoverActionViaGrabPoint(TransformCtrl::GrabPointType eGrabPoint, float fRotation);
	void BeginTransform();

	void DoMouseMove_Transform(bool bCtrlMod, bool bShiftMod);
	void DoMouseRelease_Transform();

	void GetSnapCandidateList(SnapCandidates &snapCandidatesOut); // Gather snapping candidates using the snapping settings and items from m_ItemList
	glm::vec2 SnapTransform(const SnapCandidates &snapCandidatesRef, TransformCtrl *pCurTransform);

	virtual void OnPerformTransform() = 0;
};

#endif // IDRAWEX_H
