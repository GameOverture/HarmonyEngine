/**************************************************************************
 *	DlgInputNumber.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DlgInputNumber_H
#define DlgInputNumber_H

#include <QDialog>
#include "ExplorerItemData.h"

namespace Ui {
class DlgInputNumber;
}

class DlgInputNumber : public QDialog
{
	Q_OBJECT

	int									m_iStartValue;
	std::function<QString(int)>			m_fpErrorCheckFunc;

public:
	explicit DlgInputNumber(const QString &sDlgTitle, const QIcon &icon, int iStartValue, int iMin, int iMax, std::function<QString(int)> fpErrorCheckFunc, QWidget *pParent = nullptr);
	virtual ~DlgInputNumber();
	
	int GetValue();
	
private Q_SLOTS:
	void on_txtName_textChanged(const QString &arg1);
	
private:
	Ui::DlgInputNumber *ui;
	
	void ErrorCheck();
};

#endif // DlgInputNumber_H
