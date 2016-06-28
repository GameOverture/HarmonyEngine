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

    ui->btnAddFrames->setDefaultAction(FindAction(stateActionList, "actionImportFrames"));
    ui->btnRemoveFrame->setDefaultAction(FindAction(stateActionList, "actionRemoveFrames"));
    ui->btnOrderFrameUp->setDefaultAction(FindAction(stateActionList, "actionOrderFrameUpwards"));
    ui->btnOrderFrameDown->setDefaultAction(FindAction(stateActionList, "actionOrderFrameDownwards"));
    
    ui->frames->setColumnWidth(COLUMN_Frame, 100);
    ui->frames->setColumnWidth(COLUMN_Offset, 64);
    ui->frames->setColumnWidth(COLUMN_Rotation, 32);
    ui->frames->setColumnWidth(COLUMN_Scale, 64);
    ui->frames->setColumnWidth(COLUMN_Duration, 32);
    ui->frames->setMinimumWidth(100+64+32+64+32);
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

void WidgetSpriteState::InsertFrame(HyGuiFrame *pFrame)
{
    Frame *pNewItem = new Frame(pFrame);
    int iRow = ui->frames->rowCount();
    
    ui->frames->insertRow(iRow);
    ui->frames->setItem(iRow, COLUMN_Frame, pNewItem->m_pTableItems[COLUMN_Frame]);
    ui->frames->setItem(iRow, COLUMN_Offset, pNewItem->m_pTableItems[COLUMN_Offset]);
    ui->frames->setItem(iRow, COLUMN_Rotation, pNewItem->m_pTableItems[COLUMN_Rotation]);
    ui->frames->setItem(iRow, COLUMN_Scale, pNewItem->m_pTableItems[COLUMN_Scale]);
    ui->frames->setItem(iRow, COLUMN_Duration, pNewItem->m_pTableItems[COLUMN_Duration]);
}

void WidgetSpriteState::RemoveFrame(HyGuiFrame *pFrame)
{
    for(int i = 0; i < ui->frames->rowCount(); ++i)
    {
        if(ui->frames->item(i, COLUMN_Frame)->data(Qt::UserRole).value<HyGuiFrame *>() == pFrame)
        {
            ui->frames->removeRow(i);
            break;
        }
    }
}

HyGuiFrame *WidgetSpriteState::SelectedFrame()
{
    return ui->frames->item(ui->frames->currentRow(), COLUMN_Frame)->data(Qt::UserRole).value<HyGuiFrame *>();
}
