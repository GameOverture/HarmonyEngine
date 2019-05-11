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
	QComboBox *				m_pCmbStates;

public:
	FontDelegate(ProjectItem *pItem, QComboBox *pCmbStates, QObject *pParent = 0);

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

	void OnUpdate();
	
	QString GetFullItemName();

	QComboBox *GetCmbStates();
	
	virtual void FocusState(int iStateIndex, QVariant subState) override;

	void UpdateActions();

	FontStateData *GetCurStateData();
	int GetSelectedStageId();

private Q_SLOTS:
	void on_cmbStates_currentIndexChanged(int index);

	void on_actionAddState_triggered();

	void on_actionRemoveState_triggered();
	
	void on_actionRenameState_triggered();
	
	void on_actionOrderStateBackwards_triggered();
	
	void on_actionOrderStateForwards_triggered();
	
	void on_actionAddLayer_triggered();
	
	void on_actionRemoveLayer_triggered();

	void on_actionOrderLayerDownwards_triggered();

	void on_actionOrderLayerUpwards_triggered();

	void on_cmbRenderMode_currentIndexChanged(int index);

	void on_sbSize_editingFinished();

	void on_cmbFontList_currentIndexChanged(int index);
	
private:
	Ui::FontWidget *ui;
};

#endif // FONTWIDGET_H
