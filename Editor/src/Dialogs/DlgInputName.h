/**************************************************************************
 *	DlgInputName.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DLGINPUTNAME_H
#define DLGINPUTNAME_H

#include <QDialog>
#include "ExplorerItemData.h"

namespace Ui {
class DlgInputName;
}

class DlgInputName : public QDialog
{
	Q_OBJECT

	QString									m_sOriginalName;
	std::function<QString(QString)>			m_fpErrorCheckFunc;
	
	void CtorInit(QString sDlgTitle, QString sCurName, const QValidator *pValidator);

public:
	explicit DlgInputName(const QString sDlgTitle, QString sCurName, const QValidator *pValidator, std::function<QString(QString)> fpErrorCheckFunc, QWidget *pParent = nullptr);
	explicit DlgInputName(const QString sDlgTitle, ExplorerItemData *pItem, const QValidator *pValidator, std::function<QString(QString)> fpErrorCheckFunc, QWidget *pParent = nullptr);
	~DlgInputName();
	
	QString GetName();
	QString GetOldName();
	
private Q_SLOTS:
	void on_txtName_textChanged(const QString &arg1);
	
private:
	Ui::DlgInputName *ui;

	
	void ErrorCheck();
};

#endif // DLGINPUTNAME_H
