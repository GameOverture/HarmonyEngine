/**************************************************************************
 *	FontWidget.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef FONTWIDGET_H
#define FONTWIDGET_H

#include "FontModels.h"

#include <QWidget>
#include <QDir>
#include <QJsonArray>
#include <QTableView>

#include "freetype-gl/freetype-gl.h"

namespace Ui {
class FontWidget;
}

class FontItem;
class FontStateLayersModel;

struct FontTypeface;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FontTableView : public QTableView
{
	Q_OBJECT

public:
	FontTableView(QWidget *pParent = 0);

protected:
	virtual void resizeEvent(QResizeEvent *pResizeEvent) override;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FontDelegate : public QStyledItemDelegate
{
	Q_OBJECT

	ProjectItem *			m_pItem;

public:
	FontDelegate(ProjectItem *pItem, QObject *pParent = nullptr);

	virtual QWidget* createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	virtual void setEditorData(QWidget *pEditor, const QModelIndex &index) const override;
	virtual void setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const override;
	virtual void updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FontWidget : public IWidget
{
	Q_OBJECT
	
public:
	explicit FontWidget(ProjectItem &itemRef, QWidget *pParent = nullptr);
	~FontWidget();

	virtual void OnGiveMenuActions(QMenu *pMenu) override;
	virtual void OnUpdateActions() override;
	virtual void OnFocusState(int iStateIndex, QVariant subState) override;

private Q_SLOTS:
	
	void on_actionAddLayer_triggered();
	
	void on_actionRemoveLayer_triggered();

	void on_actionOrderLayerDownwards_triggered();

	void on_actionOrderLayerUpwards_triggered();

	void on_cmbRenderMode_currentIndexChanged(int index);

	void on_sbSize_editingFinished();

	void on_cmbFontList_currentIndexChanged(int index);
	
private:
	Ui::FontWidget *ui;

	int GetSelectedStageId();
};

#endif // FONTWIDGET_H
