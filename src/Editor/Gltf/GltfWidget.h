/**************************************************************************
*	GltfWidget.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef GLTFWIDGET_H
#define GLTFWIDGET_H

#include "GltfModel.h"

#include <QWidget>

namespace Ui {
	class GltfWidget;
}

class GltfWidget : public QWidget
{
	Q_OBJECT

	GltfModel *					m_pModel;

public:
	GltfWidget(QWidget *pParent);
	GltfWidget(GltfModel *pModel, QWidget *pParent = nullptr);
	~GltfWidget();

private:
	Ui::GltfWidget *ui;
};

#endif // GLTFWIDGET_H
