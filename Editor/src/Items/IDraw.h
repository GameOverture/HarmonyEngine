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

class ProjectItemData;

class IDraw : public HyEntity2d
{
protected:
	ProjectItemData *	m_pProjItem;
	HyCamera2d *		m_pCamera;

	bool				m_bPanCameraKeyDown;
	bool				m_bIsCameraPanning;
	QPointF				m_ptOldMousePos;

public:
	IDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef);
	virtual ~IDraw();

	HyCamera2d *GetCamera();
	
	void ApplyJsonData();

	void Show();
	void Hide();
	void ResizeRenderer();

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
