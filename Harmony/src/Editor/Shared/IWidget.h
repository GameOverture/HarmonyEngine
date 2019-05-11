/**************************************************************************
*	IWidget.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IWIDGET_H
#define IWIDGET_H

#include <QWidget.h>

class ProjectItem;

class IWidget : public QWidget
{
protected:
	ProjectItem &			m_ItemRef;

public:
	IWidget(ProjectItem &itemRef, QWidget *pParent = nullptr);
	virtual ~IWidget();

	ProjectItem &GetItem();

	virtual void OnGiveMenuActions(QMenu *pMenu) = 0;
	virtual void FocusState(int iStateIndex, QVariant subState) = 0;
};

#endif // IWIDGET_H
