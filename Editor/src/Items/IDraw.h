/**************************************************************************
 *	IDraw.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IDRAW_H
#define IDRAW_H

#include "Global.h"

class ProjectItemData;

enum DrawAction
{
	// NOTE: Actions are ordered by priority [low->high]. Only allow setting a higher priority action
	HYACTION_None = 0,
	HYACTION_Streaming,

	HYACTION_MarqueeStart,					// When clicking outside any items' bounds
	
	HYACTION_HoverItem,
	HYACTION_HoverGuideHorz,
	HYACTION_HoverGuideVert,
	HYACTION_HoverScale,
	HYACTION_HoverRotate,
	
	HYACTION_Pending,						// Using 'm_PressTimer' (or mouse movement) to determine if new item selection, or drag existing selection will occur

	HYACTION_Previewing,					// Previewing wants to prevent any new actions once set, but not start until any existing action in progress

	HYACTION_ManipGuideHorz,
	HYACTION_ManipGuideVert,

	HYACTION_TransformingScale,
	HYACTION_TransformingRotation,
	HYACTION_TransformingTranslate,
	HYACTION_TransformingNudging,			// Using arrow keys to nudge selected items

	// Entity Specific:
	HYACTION_EntityAddShape,				// A new primitive or bounding volume tool icon is selected
	HYACTION_EntityAddShapePending,
	HYACTION_EntityAddShapeDrag,			// User is mouse dragging to create the new shape

	//HYACTION_EntitySem,						// Default/Blank state of Shape Edit Mode (SEM) - This action is set using SetBackgroundAction() until the mode is toggled off
	//HYACTION_EntitySemHoverItem,
	//HYACTION_EntitySemHoverTranslate,		// Translates all vertices of the shape
	//HYACTION_EntitySemHoverAddVertex,		// When holding CTRL, shows cross-hair to add a new vertex
	//HYACTION_EntitySemHoverGrabVertex,		// Hovering over a vertex to grab and move it
	//HYACTION_EntitySemHoverRadiusHorizontal,// Circle Shape's horizontal radius grab vertex
	//HYACTION_EntitySemHoverRadiusVertical,	// Circle Shape's vertical radius grab vertex

	//HYACTION_EntitySemMarquee,

	//HYACTION_EntitySemTranslating,
	//HYACTION_EntitySemAddingVertex,
	//HYACTION_EntitySemTranslateVertex,
	//HYACTION_EntitySemRadiusHorizontal,
	//HYACTION_EntitySemRadiusVertical,
	//
	//HYACTION_EntitySemInvalid,				// Indicates an invalid SEM action taking place (trying to add too many vertices to polygon, invalid vertex placement, etc)

	// High Priority Basic Actions:
	HYACTION_Forbidden,						// Disable and prevents any other actions from occurring, but still allows camera panning
	HYACTION_Pan,
	HYACTION_MarqueeDrag,					// Marquee in progress

	// Editor Loading (locked input):
	HYACTION_Wait
};

class IDraw : public HyEntity2d
{
	friend class HarmonyWidget;
	friend class UndoCmd_AddGuide;
	friend class UndoCmd_RemoveGuide;
	friend class UndoCmd_MoveGuide;

	static bool											sm_bIsPaintAllowed;

protected:
	ProjectItemData *									m_pProjItem;
	HyCamera2d *										m_pCamera;
	glm::vec2											m_ptCamPos;
	float												m_fCamZoom;

	DrawAction											m_eDrawAction;
	DrawAction											m_eBackgroundDrawAction;

	uint32												m_uiPanFlags;
	QPointF												m_ptOldMousePos;

	HyPrimitive2d										m_PendingGuide;
	QMap<QPair<HyOrientation, int>, HyPrimitive2d *>	m_GuideMap;
	int													m_iGuideOldMovePos; // When dragging an existing guide, this was its old position

private:
	QString												m_sZoomStatus;

public:
	IDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef);
	virtual ~IDraw();

	static bool IsPaintAllowed();

	ProjectItemData *GetProjItemData();

	void GetCameraInfo(glm::vec2 &ptPosOut, float &fZoomOut);
	void SetCamera(glm::vec2 ptCamPos, float fZoom);

	// Action management
	DrawAction GetCurAction() const;
	bool IsActionTransforming() const;
	bool SetAction(DrawAction eHyAction);
	DrawAction GetBackgroundAction() const;
	bool SetBackgroundAction(DrawAction eHyAction);
	void ClearAction();
	void ClearBackgroundAction();

	QJsonArray GetGuideArray(HyOrientation eOrientation);

	void StopCameraPanning();
	
	void ApplyJsonData();

	void Show();
	void Hide();
	virtual void ResizeRenderer();
	void UpdateDrawStatus();

	virtual void OnUndoStackIndexChanged(int iIndex) { }

	// NOTE: Qt Input Events are passed in from WgtHarmony
	// Derived classes should invoke IDraw's version and pass their input events to this class, if they want their main camera to accept user control
	virtual void OnKeyPressEvent(QKeyEvent *pEvent);
	virtual void OnKeyReleaseEvent(QKeyEvent *pEvent);

	virtual void OnMouseWheelEvent(QWheelEvent *pEvent);
	virtual void OnMouseMoveEvent(QMouseEvent *pEvent);
	virtual void OnMousePressEvent(QMouseEvent *pEvent);
	virtual void OnMouseReleaseEvent(QMouseEvent *pEvent);

	bool TryAllocateGuide(HyOrientation eOrientation, int iWorldPos);

protected:
	virtual void OnUpdate() override;

	bool IsCameraPanning() const;
	
	virtual void OnApplyJsonData(HyJsonDoc &itemDataDocRef) { }
	virtual void OnApplyJsonMeta(QJsonObject &itemMetaObj) { }
	virtual void OnResizeRenderer() = 0;
	void CameraUpdated();
	virtual void OnCameraUpdated() { }

	float GetLineThickness(HyZoomLevel eZoomLevel);

private:
	bool AllocateGuide(HyOrientation eOrientation, int iWorldPos);
	bool DeleteGuide(HyOrientation eOrientation, int iWorldPos);
};

#endif // IDRAW_H
