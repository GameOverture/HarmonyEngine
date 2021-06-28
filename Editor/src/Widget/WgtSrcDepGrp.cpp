/**************************************************************************
 *	WgtSrcDepGrp.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2021 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "WgtSrcDepGrp.h"
#include "WgtSrcDependency.h"
#include "ui_WgtSrcDepGrp.h"
#include "DlgNewProject.h"

#include <QFileDialog>

WgtSrcDepGrp::WgtSrcDepGrp(QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::WgtSrcDepGrp)
{
	ui->setupUi(this);
}

WgtSrcDepGrp::~WgtSrcDepGrp()
{


	delete ui;
}

void WgtSrcDepGrp::SetProjDir(QString sNewProjDirPath)
{
	//m_sAbsProjDirPath = sNewProjDirPath;
	//for(auto srcDep : m_SrcDependencyList)
	//	srcDep->ResetProjDir(m_sAbsProjDirPath);
}

//QString WgtSrcDepGrp::GetError()
//{
//	QString sError;
//	for(auto srcDep : m_SrcDependencyList)
//	{
//		sError = srcDep->GetError();
//		if(sError.isEmpty() == false)
//			break;
//	}
//	return sError;
//}
