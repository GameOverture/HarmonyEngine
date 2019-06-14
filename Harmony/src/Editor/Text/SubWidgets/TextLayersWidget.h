/**************************************************************************
*	TextLayersWidget.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef TEXTLAYERSWIDGET_H
#define TEXTLAYERSWIDGET_H

#include <QTableView>

class TextLayersWidget : public QTableView
{
	Q_OBJECT

public:
	TextLayersWidget(QWidget *pParent = nullptr);

protected:
	virtual void resizeEvent(QResizeEvent *pResizeEvent) override;
};

#endif // TEXTLAYERSWIDGET_H
