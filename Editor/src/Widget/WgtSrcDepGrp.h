/**************************************************************************
 *	WgtSrcDepGrp.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2021 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WgtSrcDepGrp_H
#define WgtSrcDepGrp_H

#include <QWidget>

namespace Ui {
class WgtSrcDepGrp;
}

class WgtSrcDependency;

class WgtSrcDepGrp : public QWidget
{
	Q_OBJECT

	Ui::WgtSrcDepGrp *			ui;

	QString						m_sAbsProjDirPath;
	
	
public:
	WgtSrcDepGrp(QWidget *pParent = nullptr);
	virtual ~WgtSrcDepGrp();

	void SetProjDir(QString sNewProjDirPath);



	

	QString GetError();

Q_SIGNALS:
	void OnDirty();
};

#endif // WgtSrcDepGrp_H
