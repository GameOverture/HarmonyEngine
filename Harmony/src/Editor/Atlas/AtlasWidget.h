/**************************************************************************
 *	AtlasWidget.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ATLASWIDGET_H
#define ATLASWIDGET_H

#include "AtlasModel.h"
#include "AtlasDraw.h"

#include <QWidget>
#include <QDir>
#include <QMouseEvent>
#include <QStringListModel>
#include <QStackedWidget>


namespace Ui {
class AtlasWidget;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class AtlasTreeWidget : public QTreeWidget
{
	AtlasWidget *      m_pOwner;

public:
	AtlasTreeWidget(QWidget *parent = Q_NULLPTR);
	void SetAtlasOwner(AtlasWidget *pOwner);

protected:
	virtual void dropEvent(QDropEvent *e) override;
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class AtlasTreeItem : public QTreeWidgetItem
{
public:
	AtlasTreeItem(AtlasTreeWidget *pView, int type = Type) : QTreeWidgetItem(pView, type)
	{ }
	AtlasTreeItem(QTreeWidgetItem *parent, int type = Type) : QTreeWidgetItem(parent, type)
	{ }

	bool operator<(const QTreeWidgetItem& other) const;
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class AtlasWidget : public QWidget
{
	Q_OBJECT

	friend class WidgetAtlasGroup;

	AtlasModel *                    m_pModel;
	AtlasDraw                       m_Draw;

	QTreeWidgetItem *               m_pMouseHoverItem;

public:
	explicit AtlasWidget(QWidget *parent = 0);
	explicit AtlasWidget(AtlasModel *pModel, QWidget *parent = 0);
	~AtlasWidget();

	quint32 GetSelectedAtlasGrpId();

	AtlasModel &GetData();
	AtlasTreeWidget *GetFramesTreeWidget();

	void DrawUpdate();

	void StashTreeWidgets();
	
	void RefreshInfo();
	
protected:
	virtual void enterEvent(QEvent *pEvent) override;
	virtual void leaveEvent(QEvent *pEvent) override;
	virtual void resizeEvent(QResizeEvent *event) override;

	bool DoAtlasGroupSettingsDlg();

private Q_SLOTS:
	void on_actionDeleteImages_triggered();

	void on_actionReplaceImages_triggered();

	void on_atlasList_itemSelectionChanged();

	void OnContextMenu(const QPoint &pos);

	void on_actionRename_triggered();

	void on_cmbAtlasGroups_currentIndexChanged(int index);
	
	void on_actionAddGroup_triggered();

	void on_actionGroupSettings_triggered();

	void on_actionRemoveGroup_triggered();
	
	void on_actionAtlasGrpTransfer_triggered(QAction *pAction);

	void on_actionImportImages_triggered();

	void on_actionImportDirectory_triggered();

	void on_actionAddFilter_triggered();

private:
	Ui::AtlasWidget *ui;

	void GetSelectedItemsRecursively(QList<QTreeWidgetItem *> selectedTreeItems, QList<QTreeWidgetItem *> &frameListRef, QList<QTreeWidgetItem *> &filterListRef);
};

#endif // ATLASWIDGET_H
