/**************************************************************************
 *	DlgNewBuild.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DlgNewBuild_H
#define DlgNewBuild_H

#include <QDialog>

namespace Ui {
class DlgNewBuild;
}

class DlgNewBuild : public QDialog
{
	Q_OBJECT

	Project &				m_ProjectRef;

public:
	explicit DlgNewBuild(Project &projectRef, QWidget *parent = 0);
	~DlgNewBuild();

private Q_SLOTS:

	void on_buttonBox_accepted();

private:
	Ui::DlgNewBuild *ui;

	void ErrorCheck();
};


#endif // DlgNewBuild_H
