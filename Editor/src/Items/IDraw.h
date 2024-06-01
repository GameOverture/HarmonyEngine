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
#include "ShapeCtrl.h"

#include <QWidget>
#include <QString>

class ProjectItemData;

class IDraw : public HyEntity2d
{
protected:
	ProjectItemData *	m_pProjItem;
	HyCamera2d *		m_pCamera;
	glm::vec2			m_ptCamPos;
	float				m_fCamZoom;

	enum PanFlags
	{
		PAN_UP			= 1 << 0,
		PAN_DOWN		= 1 << 1,
		PAN_LEFT		= 1 << 2,
		PAN_RIGHT		= 1 << 3
	};
	uint32				m_uiPanFlags;

	bool				m_bIsMiddleMouseDown;
	QPointF				m_ptOldMousePos;

private:
	QString				m_sSizeStatus;	// Derived classes should set this using UpdateDrawStatus()
	QString				m_sZoomStatus;

public:
	IDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef);
	virtual ~IDraw();

	void GetCameraInfo(glm::vec2 &ptPosOut, float &fZoomOut);
	void SetCamera(glm::vec2 ptCamPos, float fZoom);
	
	void ApplyJsonData();

	void Show();
	void Hide();
	void ResizeRenderer();
	void UpdateDrawStatus(QString sSizeDescription);

	virtual void OnUndoStackIndexChanged(int iIndex) { }

	// Derived classes should pass their input events to this class, if they want their main camera to accept user control
	virtual void OnKeyPressEvent(QKeyEvent *pEvent);
	virtual void OnKeyReleaseEvent(QKeyEvent *pEvent);

	virtual void OnMousePressEvent(QMouseEvent *pEvent);
	virtual void OnMouseReleaseEvent(QMouseEvent *pEvent);
	virtual void OnMouseWheelEvent(QWheelEvent *pEvent);
	virtual void OnMouseMoveEvent(QMouseEvent *pEvent);

protected:
	virtual void OnUpdate() override;

	bool IsCameraPanning() const;
	
	virtual void OnApplyJsonData(HyJsonDoc &itemDataDocRef) { }
	virtual void OnApplyJsonMeta(QJsonObject &itemMetaObj) { }
	virtual void OnShow() = 0;
	virtual void OnHide() = 0;
	virtual void OnResizeRenderer() = 0;
	virtual void OnZoom(HyZoomLevel eZoomLevel) { }

	float GetLineThickness(HyZoomLevel eZoomLevel);
};

#endif // IDRAW_H
