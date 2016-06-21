/**************************************************************************
 *	WidgetSpriteState.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetSpriteState.h"
#include "ui_WidgetSpriteState.h"

#include "HyGlobal.h"

WidgetSpriteState::WidgetSpriteState(QList<QAction *> stateActionList, QWidget *parent) :   QWidget(parent),
                                                                                            ui(new Ui::WidgetSpriteState),
                                                                                            m_sName("Unnamed")
{
    ui->setupUi(this);

    ui->btnAddFrames->setDefaultAction(FindAction(stateActionList, "actionRequestFrames"));
    ui->btnRemoveFrame->setDefaultAction(FindAction(stateActionList, "actionRelinquishFrames"));
    ui->btnOrderFrameUp->setDefaultAction(FindAction(stateActionList, "actionOrderFrameUpwards"));
    ui->btnOrderFrameDown->setDefaultAction(FindAction(stateActionList, "actionOrderFrameDownwards"));
}

WidgetSpriteState::~WidgetSpriteState()
{
    delete ui;
}

QString WidgetSpriteState::GetName()
{
    return m_sName;
}

void WidgetSpriteState::SetName(QString sNewName)
{
    m_sName = sNewName;
}
