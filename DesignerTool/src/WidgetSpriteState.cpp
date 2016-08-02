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
#include "ItemSprite.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

WidgetSpriteState::WidgetSpriteState(ItemSprite *pItemSprite, QList<QAction *> stateActionList, QWidget *parent) :  QWidget(parent),
                                                                                                                    m_pSpriteOwner(pItemSprite),
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
    while(m_pFrameList.empty() == false)
    {
        delete m_pFrameList[m_pFrameList.count() - 1];
        m_pFrameList.removeLast();
    }

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
    Frame *pFrameToInsert = NULL;
    
    // See if this frame has been recently removed, and re-add if possible. Otherwise, create a new Frame
    QMap<quint32, Frame *>::iterator iter = m_RemovedFrameMap.find(pFrame->GetHash());
    if(iter == m_RemovedFrameMap.end())
        pFrameToInsert = new Frame(pFrame, ui->frames->rowCount());
    else
    {
        pFrameToInsert = iter.value();
        m_RemovedFrameMap.remove(pFrame->GetHash());
    }

    ui->frames->insertRow(pFrameToInsert->m_iRowIndex);
    ui->frames->setItem(pFrameToInsert->m_iRowIndex, COLUMN_Frame, pFrameToInsert->m_pTableItems[COLUMN_Frame]);
    ui->frames->setItem(pFrameToInsert->m_iRowIndex, COLUMN_Offset, pFrameToInsert->m_pTableItems[COLUMN_Offset]);
    ui->frames->setItem(pFrameToInsert->m_iRowIndex, COLUMN_Rotation, pFrameToInsert->m_pTableItems[COLUMN_Rotation]);
    ui->frames->setItem(pFrameToInsert->m_iRowIndex, COLUMN_Scale, pFrameToInsert->m_pTableItems[COLUMN_Scale]);
    ui->frames->setItem(pFrameToInsert->m_iRowIndex, COLUMN_Duration, pFrameToInsert->m_pTableItems[COLUMN_Duration]);

    m_pFrameList.insert(pFrameToInsert->m_iRowIndex, pFrameToInsert);

    ui->frames->selectRow(0);
}

void WidgetSpriteState::RemoveFrame(HyGuiFrame *pFrame)
{
    for(int i = 0; i < ui->frames->rowCount(); ++i)
    {
        if(ui->frames->item(i, COLUMN_Frame)->data(Qt::UserRole).value<HyGuiFrame *>() == pFrame)
        {
            ui->frames->removeRow(i);

            m_RemovedFrameMap[pFrame->GetHash()] = m_pFrameList[i];
            m_pFrameList.removeAt(i);
            break;
        }
    }
}

HyGuiFrame *WidgetSpriteState::SelectedFrame()
{
    return ui->frames->item(ui->frames->currentRow(), COLUMN_Frame)->data(Qt::UserRole).value<HyGuiFrame *>();
}

int WidgetSpriteState::SelectedIndex()
{
    return ui->frames->currentRow();
}

void WidgetSpriteState::GetStateFrameInfo(QJsonArray &stateArrayOut)
{
    for(int i = 0; i < m_pFrameList.count(); ++i)
    {
        QJsonObject frameObj;

        frameObj.insert("duration", QJsonValue(m_pFrameList[i]->m_fDuration));
        frameObj.insert("rotation", QJsonValue(m_pFrameList[i]->m_fRotation));
        frameObj.insert("offsetX", QJsonValue(m_pFrameList[i]->m_ptOffset.x()));
        frameObj.insert("offsetY", QJsonValue(m_pFrameList[i]->m_ptOffset.y()));
        frameObj.insert("scaleX", QJsonValue(m_pFrameList[i]->m_ptScale.x()));
        frameObj.insert("scaleY", QJsonValue(m_pFrameList[i]->m_ptScale.y()));
        
        frameObj.insert("hash", QJsonValue(static_cast<qint64>(m_pFrameList[i]->m_pFrame->GetHash())));
        frameObj.insert("atlasGroupId", QJsonValue(m_pFrameList[i]->m_pFrame->GetAtlasGroupdId()));

        stateArrayOut.append(frameObj);
    }
}
