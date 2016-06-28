/**************************************************************************
 *	WidgetSprite.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetSprite.h"
#include "ui_WidgetSprite.h"

#include "ItemSprite.h"
#include "ItemSpriteCmds.h"
#include "DlgInputName.h"
#include "WidgetAtlasManager.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QAction>

WidgetSprite::WidgetSprite(ItemSprite *pItemSprite, WidgetAtlasManager *pAtlasMan, QWidget *parent) :   QWidget(parent),
                                                                                                        m_pItemSprite(pItemSprite),
                                                                                                        m_pAtlasManager(pAtlasMan),
                                                                                                        ui(new Ui::WidgetSprite),
                                                                                                        m_pCurSpriteState(NULL)
{
    ui->setupUi(this);

    ui->txtPrefixAndName->setText(m_pItemSprite->GetName(true));
    
    QMenu *pEditMenu = m_pItemSprite->GetEditMenu();

    m_pUndoStack = new QUndoStack(this);
    QAction *pActionUndo = m_pUndoStack->createUndoAction(pEditMenu, "&Undo");
    pActionUndo->setIcon(QIcon(":/icons16x16/generic-undo.png"));
    pActionUndo->setShortcuts(QKeySequence::Undo);
    pActionUndo->setShortcutContext(Qt::ApplicationShortcut);

    QAction *pActionRedo = m_pUndoStack->createRedoAction(pEditMenu, "&Redo");
    pActionRedo->setIcon(QIcon(":/icons16x16/generic-redo.png"));
    pActionRedo->setShortcuts(QKeySequence::Redo);
    pActionRedo->setShortcutContext(Qt::ApplicationShortcut);

    pEditMenu->addAction(pActionUndo);
    pEditMenu->addAction(pActionRedo);
    pEditMenu->addSeparator();
    pEditMenu->addAction(ui->actionAddState);
    pEditMenu->addAction(ui->actionRemoveState);
    pEditMenu->addAction(ui->actionRenameState);
    pEditMenu->addAction(ui->actionOrderStateBackwards);
    pEditMenu->addAction(ui->actionOrderStateForwards);
    pEditMenu->addSeparator();
    pEditMenu->addAction(ui->actionImportFrames);
    pEditMenu->addAction(ui->actionRemoveFrames);
    pEditMenu->addAction(ui->actionOrderFrameUpwards);
    pEditMenu->addAction(ui->actionOrderFrameDownwards);

    ui->btnAddState->setDefaultAction(ui->actionAddState);
    ui->btnRemoveState->setDefaultAction(ui->actionRemoveState);
    ui->btnRenameState->setDefaultAction(ui->actionRenameState);
    ui->btnOrderStateBack->setDefaultAction(ui->actionOrderStateBackwards);
    ui->btnOrderStateForward->setDefaultAction(ui->actionOrderStateForwards);

    m_StateActionsList.push_back(ui->actionImportFrames);
    m_StateActionsList.push_back(ui->actionRemoveFrames);
    m_StateActionsList.push_back(ui->actionOrderFrameUpwards);
    m_StateActionsList.push_back(ui->actionOrderFrameDownwards);
    
    ui->cmbStates->clear();

    // If a .hyspr file exists, parse and initalize with it, otherwise make default empty sprite
    QFile spriteFile(m_pItemSprite->GetAbsPath());
    if(spriteFile.exists())
    {
        if(!spriteFile.open(QIODevice::ReadOnly))
            HyGuiLog(QString("WidgetSprite::WidgetSprite() could not open ") % m_pItemSprite->GetAbsPath(), LOGTYPE_Error);

        QJsonDocument spriteJsonDoc = QJsonDocument::fromJson(spriteFile.readAll());
        spriteFile.close();

        QJsonObject spriteObj = spriteJsonDoc.object();

//        QJsonArray frameArray = settingsObj["frames"].toArray();
//        for(int i = 0; i < frameArray.size(); ++i)
//        {
//            QJsonObject frameObj = frameArray[i].toObject();

//            QRect rAlphaCrop(QPoint(frameObj["cropLeft"].toInt(), frameObj["cropTop"].toInt()), QPoint(frameObj["cropRight"].toInt(), frameObj["cropBottom"].toInt()));
//            HyGuiFrame *pNewFrame = new HyGuiFrame(frameObj["hash"].toInt(),
//                                                   frameObj["name"].toString(),
//                                                   rAlphaCrop,
//                                                   GetId(),
//                                                   frameObj["width"].toInt(),
//                                                   frameObj["height"].toInt(),
//                                                   frameObj["textureIndex"].toInt(),
//                                                   frameObj["rotate"].toBool(),
//                                                   frameObj["x"].toInt(),
//                                                   frameObj["y"].toInt());

//            QJsonArray frameLinksArray = frameObj["links"].toArray();
//            for(int k = 0; k < frameLinksArray.size(); ++k)
//                pNewFrame->SetLink(frameLinksArray[k].toString());

//            eAtlasNodeType eIconType = ATLAS_Frame_Warning;
//            int iTexIndex = frameObj["textureIndex"].toInt();
//            if(iTexIndex >= 0)
//            {
//                //while(m_TextureList.empty() || m_TextureList.size() <= frameObj["textureIndex"].toInt())
//                //    m_TextureList.append(CreateTreeItem(NULL, "Texture: " % QString::number(m_TextureList.size()), ATLAS_Texture));

//                //pTextureTreeItem = m_TextureList[];
//                eIconType = ATLAS_Frame;
//            }

//            pNewFrame->SetTreeWidgetItem(CreateTreeItem(NULL, frameObj["name"].toString(), iTexIndex, eIconType));
            
//            m_FrameList.append(pNewFrame);
//        }
    }
    else
    {
        on_actionAddState_triggered();
    }

    // Clear the UndoStack because we don't want any of the above initialization to be able to be undone.
    // I don't believe any on_actionAddState_triggered() calls will leak their dynamically allocated 'm_pSpriteState', since they should become children of 'ui->grpStateLayout'
    m_pUndoStack->clear();

    UpdateActions();
}

WidgetSprite::~WidgetSprite()
{
    delete ui;
}

void WidgetSprite::UpdateActions()
{
    ui->actionRemoveState->setEnabled(ui->cmbStates->count() > 1);
    ui->actionOrderStateBackwards->setEnabled(ui->cmbStates->currentIndex() != 0);
    ui->actionOrderStateForwards->setEnabled(ui->cmbStates->currentIndex() != (ui->cmbStates->count() - 1));
}

void WidgetSprite::InsertFrame(HyGuiFrame *pFrame, QVariant param)
{
    ui->cmbStates->currentData().value<WidgetSpriteState *>()->InsertFrame(pFrame, param);
}

void WidgetSprite::RemoveFrame(HyGuiFrame *pFrame)
{
    ui->cmbStates->currentData().value<WidgetSpriteState *>()->RemoveFrame(pFrame);
}

void WidgetSprite::on_actionAddState_triggered()
{
    QUndoCommand *pCmd = new ItemSpriteCmd_AddState(m_StateActionsList, ui->cmbStates);
    m_pUndoStack->push(pCmd);

    UpdateActions();
}

void WidgetSprite::on_actionRemoveState_triggered()
{
    QUndoCommand *pCmd = new ItemSpriteCmd_RemoveState(ui->cmbStates);
    m_pUndoStack->push(pCmd);

    UpdateActions();
}

void WidgetSprite::on_actionRenameState_triggered()
{
    DlgInputName *pDlg = new DlgInputName("Rename Sprite State", ui->cmbStates->currentData().value<WidgetSpriteState *>()->GetName());
    if(pDlg->exec() == QDialog::Accepted)
    {
        QUndoCommand *pCmd = new ItemSpriteCmd_RenameState(ui->cmbStates, pDlg->GetName());
        m_pUndoStack->push(pCmd);
    }
}

void WidgetSprite::on_actionOrderStateBackwards_triggered()
{
    QUndoCommand *pCmd = new ItemSpriteCmd_MoveStateBack(ui->cmbStates);
    m_pUndoStack->push(pCmd);

    UpdateActions();
}

void WidgetSprite::on_actionOrderStateForwards_triggered()
{
    QUndoCommand *pCmd = new ItemSpriteCmd_MoveStateForward(ui->cmbStates);
    m_pUndoStack->push(pCmd);

    UpdateActions();
}

void WidgetSprite::on_actionImportFrames_triggered()
{
    QUndoCommand *pCmd = new ItemSpriteCmd_AddFrames(m_pItemSprite, m_pAtlasManager);
    m_pUndoStack->push(pCmd);

    UpdateActions();
}

void WidgetSprite::on_actionRemoveFrames_triggered()
{
    WidgetSpriteState *pSpriteState = ui->cmbStates->itemData(ui->cmbStates->currentIndex()).value<WidgetSpriteState *>();
    HyGuiFrame *pSelectredFrame = pSpriteState->SelectedFrame();

    QUndoCommand *pCmd = new ItemSpriteCmd_DeleteFrame(m_pItemSprite, m_pAtlasManager, pSelectredFrame, pSpriteState->SelectedIndex());
    m_pUndoStack->push(pCmd);

    UpdateActions();
}

void WidgetSprite::on_actionOrderFrameUpwards_triggered()
{
    
}

void WidgetSprite::on_actionOrderFrameDownwards_triggered()
{
    
}

void WidgetSprite::on_cmbStates_currentIndexChanged(int index)
{
    WidgetSpriteState *pSpriteState = ui->cmbStates->itemData(index).value<WidgetSpriteState *>();
    if(m_pCurSpriteState == pSpriteState)
        return;

    if(m_pCurSpriteState)
        m_pCurSpriteState->hide();

    ui->grpStateLayout->addWidget(pSpriteState);

#if _DEBUG
    int iDebugTest = ui->grpStateLayout->count();
#endif

    m_pCurSpriteState = pSpriteState;
    m_pCurSpriteState->show();

    UpdateActions();
}

