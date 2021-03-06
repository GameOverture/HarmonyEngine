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

#include "Project/ProjectItemData.h"

#include <QTableView>
#include <QStyledItemDelegate>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TextLayersWidget : public QTableView
{
	Q_OBJECT

public:
	TextLayersWidget(QWidget *pParent = nullptr);

protected:
	virtual void resizeEvent(QResizeEvent *pResizeEvent) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TextLayersDelegate : public QStyledItemDelegate
{
	Q_OBJECT

	ProjectItemData *			m_pItem;

public:
	TextLayersDelegate(ProjectItemData *pItem, QObject *pParent = nullptr);

	virtual QWidget *createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	virtual void setEditorData(QWidget *pEditor, const QModelIndex &index) const override;
	virtual void setModelData(QWidget *pEditor, QAbstractItemModel *pItemModel, const QModelIndex &index) const override;
	virtual void updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // TEXTLAYERSWIDGET_H
