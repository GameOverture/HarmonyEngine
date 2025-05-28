/**************************************************************************
 *	DlgSetSpineAnims.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DLGSETSPINEANIMS_H
#define DLGSETSPINEANIMS_H

#include <QDialog>

class QButton;

namespace Ui {
class DlgSetSpineAnims;
}

class DlgSetSpineAnims : public QDialog
{
	Q_OBJECT
	
	Project	&			m_ProjectRef;
	

public:
	explicit DlgSetSpineAnims(Project &projectRef, QString sTitle, QWidget *parent = 0);
	~DlgSetSpineAnims();

private Q_SLOTS:
	
private:
	Ui::DlgSetSpineAnims *ui;
};

#endif // DLGSETSPINEANIMS_H
