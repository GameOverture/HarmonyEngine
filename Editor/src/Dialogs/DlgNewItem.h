/**************************************************************************
 *	DlgNewItem.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DLGNEWSPRITE_H
#define DLGNEWSPRITE_H

#include "Global.h"
#include "Project.h"

#include <QDialog>

namespace Ui {
class DlgNewItem;
}

class DlgNewItem : public QDialog
{
	Q_OBJECT

	Project *			m_pItemProject;
	ItemType			m_eItemType;
	QStringList			m_PrefixStringList;

public:
	explicit DlgNewItem(Project *pItemProject, ItemType eItem, QString sDefaultPrefix, QWidget *parent);
	~DlgNewItem();
	
	QString GetName();
	QString GetPrefix();
	QString GetImportFile();

private Q_SLOTS:
	void on_chkNewPrefix_stateChanged(int arg1);

	void on_txtPrefix_textChanged(const QString &arg1);

	void on_cmbPrefixList_currentIndexChanged(const QString &arg1);

	void on_txtName_textChanged(const QString &arg1);

	void on_buttonBox_accepted();

	void on_btnImportBrowse_clicked();

	void on_txtImport_textChanged(const QString &arg1);

private:
	Ui::DlgNewItem *ui;

	void ErrorCheck();
};

#endif // DLGNEWSPRITE_H
