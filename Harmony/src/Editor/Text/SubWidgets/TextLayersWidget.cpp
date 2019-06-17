/**************************************************************************
*	TextLayersWidget.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "global.h"
#include "TextLayersWidget.h"
#include "TextLayersModel.h"

#include <QResizeEvent>

TextLayersWidget::TextLayersWidget(QWidget *pParent /*= nullptr*/) :
	QTableView(pParent)
{
}

/*virtual*/ void TextLayersWidget::resizeEvent(QResizeEvent *pResizeEvent)
{
	int iWidth = pResizeEvent->size().width();

	iWidth -= 144;
	setColumnWidth(TextLayersModel::COLUMN_Type, iWidth);
	setColumnWidth(TextLayersModel::COLUMN_Thickness, 64);
	setColumnWidth(TextLayersModel::COLUMN_Color, 80);

	QTableView::resizeEvent(pResizeEvent);
}
