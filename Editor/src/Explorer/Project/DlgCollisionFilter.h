/**************************************************************************
 *	DlgCollisionFilter.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DlgCollisionFilter_H
#define DlgCollisionFilter_H

#include "Global.h"

#include <QDialog>

class Project;

namespace Ui {
class DlgCollisionFilter;
}

class DlgCollisionFilter : public QDialog
{
	Q_OBJECT

	Project &				m_ProjectRef;
	int						m_iRenameSelectedIndex;

public:
	explicit DlgCollisionFilter(Project &projectRef, b2Filter initFilter, QWidget *pParent = 0);
	virtual ~DlgCollisionFilter();

	b2Filter GetB2Filter() const;

protected Q_SLOTS:
	void on_btnAddCategory_clicked();
	void on_btnAddMask_clicked();
	void on_btnRemoveCategory_clicked();
	void on_btnRemoveAllCategories_clicked();
	void on_btnRemoveMask_clicked();
	void on_btnRemoveAllMasks_clicked();

	void on_actionRenameCategory_triggered();
	void OnCategoryContextMenu(const QPoint &ptLocal);
	void OnMaskContextMenu(const QPoint &ptLocal);

private:
	Ui::DlgCollisionFilter *ui;

	void Refresh();
	void ErrorCheck();
};

#endif // DlgCollisionFilter_H
