/**************************************************************************
 *	WidgetAudioBank.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetAudioBank.h"
#include "ui_WidgetAudioBank.h"

#include "MainWindow.h"
#include "WidgetAudioManager.h"

WidgetAudioBank::WidgetAudioBank(QWidget *parent) : QWidget(parent),
                                                    ui(new Ui::WidgetAudioBank)
{
    ui->setupUi(this);
    
    // Invalid constructor. This exists so Q_OBJECT can work.
    HyGuiLog("WidgetAudioBank::WidgetAudioBank() invalid constructor used", LOGTYPE_Error);
}

WidgetAudioBank::WidgetAudioBank(QDir metaDir, QDir dataDir, WidgetAudioManager *pManager, QWidget *pParent /*= 0*/) :  QWidget(pParent),
                                                                                                                        ui(new Ui::WidgetAudioBank),
                                                                                                                        m_pManager(pManager),
                                                                                                                        m_MetaDir(metaDir),
                                                                                                                        m_DataDir(dataDir),
                                                                                                                        m_sName("Unnamed")
{
    ui->setupUi(this);
}

WidgetAudioBank::~WidgetAudioBank()
{
    delete ui;
}

QString WidgetAudioBank::GetName()
{
    return m_sName;
}
