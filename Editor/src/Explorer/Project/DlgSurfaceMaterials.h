/**************************************************************************
 *	DlgSurfaceMaterials.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DlgSurfaceMaterials_H
#define DlgSurfaceMaterials_H

#include <QDialog>
#include <QJsonObject>

class SurfaceMaterialsModel;

namespace Ui {
class DlgSurfaceMaterials;
}

class DlgSurfaceMaterials : public QDialog
{
	Q_OBJECT

	SurfaceMaterialsModel &		m_SurfaceMaterialsModelRef;

public:
	explicit DlgSurfaceMaterials(SurfaceMaterialsModel &surfaceModelRef, QWidget *pParent = 0);
	virtual ~DlgSurfaceMaterials();

	QUuid GetSelectedMaterialUuid() const;

private Q_SLOTS:
	void on_actionNewMat_triggered();
	void on_actionRemoveMat_triggered();
	void on_actionRenameMat_triggered();
	void on_actionSortMatUp_triggered();
	void on_actionSortMatDown_triggered();

private:
	Ui::DlgSurfaceMaterials *ui;

	void ErrorCheck();
};

#endif // DlgSurfaceMaterials_H
