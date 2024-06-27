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

class IDraw : public HyEntity2d
{
	friend class HarmonyWidget;
	friend class UndoCmd_AddGuide;
	friend class UndoCmd_RemoveGuide;
	friend class UndoCmd_MoveGuide;

protected:
	ProjectItemData *									m_pProjItem;
	HyCamera2d *										m_pCamera;
	glm::vec2											m_ptCamPos;
	float												m_fCamZoom;

	uint32												m_uiPanFlags;

	bool												m_bIsMiddleMouseDown;
	QPointF												m_ptOldMousePos;

	HyPrimitive2d										m_PendingGuide;
	QMap<QPair<HyOrientation, int>, HyPrimitive2d *>	m_GuideMap;

private:
	QString												m_sSizeStatus;	// Derived classes should set this using UpdateDrawStatus()
	QString												m_sZoomStatus;

public:
	IDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef);
	virtual ~IDraw();

	ProjectItemData *GetProjItemData();

	void GetCameraInfo(glm::vec2 &ptPosOut, float &fZoomOut);
	void SetCamera(glm::vec2 ptCamPos, float fZoom);

	QJsonArray GetGuideArray(HyOrientation eOrientation);

	void StopCameraPanning();
	
	void ApplyJsonData();

	void Show();
	void Hide();
	void ResizeRenderer();
	void UpdateDrawStatus(QString sSizeDescription);

	virtual void OnUndoStackIndexChanged(int iIndex) { }

	// NOTE: Qt Input Events are passed in from WgtHarmony
	// Derived classes should invoke IDraw's version and pass their input events to this class, if they want their main camera to accept user control
	virtual void OnKeyPressEvent(QKeyEvent *pEvent);
	virtual void OnKeyReleaseEvent(QKeyEvent *pEvent);

	virtual void OnMouseWheelEvent(QWheelEvent *pEvent);
	virtual void OnMouseMoveEvent(QMouseEvent *pEvent);
	virtual void OnMousePressEvent(QMouseEvent *pEvent);
	virtual void OnMouseReleaseEvent(QMouseEvent *pEvent);

	const QMap<QPair<HyOrientation, int>, HyPrimitive2d *> &GetGuideMap() const;
	void SetPendingGuide(HyOrientation eOrientation);
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
