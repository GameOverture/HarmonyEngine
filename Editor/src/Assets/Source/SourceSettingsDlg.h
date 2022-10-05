/**************************************************************************
 *	SourceSettingsDlg.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2021 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef SourceSettingsDlg_H
#define SourceSettingsDlg_H

#include "Global.h"
#include "Project.h"

#include <QDialog>

namespace Ui {
class SourceSettingsDlg;
}

class WgtSrcDependency;

class SourceSettingsDlg : public QDialog
{
	Q_OBJECT

	Ui::SourceSettingsDlg *		ui;

	const Project &				m_ProjectRef;
	QList<WgtSrcDependency *>	m_SrcDependencyList;
	bool						m_bIsError;

public:
	SourceSettingsDlg(const Project &projectRef, QJsonObject settingsObj, QWidget *pParent = nullptr);
	virtual ~SourceSettingsDlg();

	QString GetProjectDir() const;
	
	void RemoveSrcDep(WgtSrcDependency *pRemoved);

	void Refresh();

	void UpdateMetaObj(QJsonObject &metaObjRef) const;

	bool IsError() const;
	QString GetError() const;

private Q_SLOTS:
	void on_txtOutputName_textChanged(const QString &arg1);

	void on_btnAddDependency_clicked();

private:
	void ErrorCheck();
};

#endif // SourceSettingsDlg_H
