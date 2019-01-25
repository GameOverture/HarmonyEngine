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

class ProjectItem;

class IDraw : public HyEntity2d
{
protected:
	ProjectItem *       m_pProjItem;
	HyCamera2d *        m_pCamera;

	HyPrimitive2d       m_primOriginHorz;
	HyPrimitive2d       m_primOriginVert;

	bool                m_bPanCameraKeyDown;
	bool                m_bIsCameraPanning;
	QPointF             m_ptOldMousePos;

public:
	IDraw(ProjectItem *pProjItem);
	virtual ~IDraw();
	
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

protected:
	virtual void OnApplyJsonData(jsonxx::Value &valueRef) { }
	virtual void OnShow() = 0;
	virtual void OnHide() = 0;
	virtual void OnResizeRenderer() = 0;
};

#endif // IDRAW_H
