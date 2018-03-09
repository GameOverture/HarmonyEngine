/**************************************************************************
 *	DlgNewItem.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Designer Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
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

	Project *       m_pItemProject;
	HyGuiItemType       m_eItemType;
	QStringList     m_PrefixStringList;

public:
	explicit DlgNewItem(Project *pItemProject, HyGuiItemType eItem, QString sDefaultPrefix, QWidget *parent = 0);
	~DlgNewItem();
	
	QString GetName();
	
	QString GetPrefix();

private Q_SLOTS:
	void on_chkNewPrefix_stateChanged(int arg1);

	void on_txtPrefix_textChanged(const QString &arg1);

	void on_cmbPrefixList_currentIndexChanged(const QString &arg1);

	void on_txtName_textChanged(const QString &arg1);

	void on_buttonBox_accepted();
	
private:
	Ui::DlgNewItem *ui;

	void ErrorCheck();
};

#endif // DLGNEWSPRITE_H
