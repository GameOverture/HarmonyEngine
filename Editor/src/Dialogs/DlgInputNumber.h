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
	bool								m_bAllowStartValue;

public:
	explicit DlgInputNumber(const QString &sDlgTitle, const QString &sLabel, const QIcon &icon, int iStartValue, int iMin, int iMax, std::function<QString(int)> fpErrorCheckFunc, bool bAllowStartValue = false, QWidget *pParent = nullptr);
	virtual ~DlgInputNumber();
	
	int GetValue();
	
private Q_SLOTS:
	void on_sbValue_valueChanged(int iArg);
	
private:
	Ui::DlgInputNumber *ui;
	
	void ErrorCheck();
};

#endif // DlgInputNumber_H
