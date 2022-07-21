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

	bool				m_bPanCameraKeyDown;
	bool				m_bIsCameraPanning;
	QPointF				m_ptOldMousePos;

private:
	QString				m_sSizeStatus;	// Derived classes should set this using UpdateDrawStatus()

public:
	IDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef);
	virtual ~IDraw();

	void GetCameraInfo(glm::vec2 &ptPosOut, float &fZoomOut);
	
	void ApplyJsonData();

	void Show();
	void Hide();
	void ResizeRenderer();
	void UpdateDrawStatus(QString sSizeDescription);

	// Derived classes should pass their input events to this class, if they want their main camera to accept user control
	virtual void OnKeyPressEvent(QKeyEvent *pEvent);
	virtual void OnKeyReleaseEvent(QKeyEvent *pEvent);
	virtual void OnMousePressEvent(QMouseEvent *pEvent);
	virtual void OnMouseReleaseEvent(QMouseEvent *pEvent);
	virtual void OnMouseWheelEvent(QWheelEvent *pEvent);
	virtual void OnMouseMoveEvent(QMouseEvent *pEvent);

	virtual void OnDrawUpdate() { }

protected:
	virtual void OnApplyJsonData(HyJsonDoc &itemDataDocRef) { }
	virtual void OnShow() = 0;
	virtual void OnHide() = 0;
	virtual void OnResizeRenderer() = 0;
};

#endif // IDRAW_H
