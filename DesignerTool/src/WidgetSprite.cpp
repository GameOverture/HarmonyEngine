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

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

SpriteState::SpriteState() : m_sName("Unnamed")
{
}

SpriteState::~SpriteState()
{
}

QString SpriteState::GetName()
{
    return m_sName;
}

void SpriteState::SetName(QString sNewName)
{
    m_sName = sNewName;
}

WidgetSprite::WidgetSprite(ItemSprite *pItemSprite, QWidget *parent) :  QWidget(parent),
                                                                        m_pItemSprite(pItemSprite),
                                                                        ui(new Ui::WidgetSprite)
{
    ui->setupUi(this);

    ui->txtPrefixAndName->setText(m_pItemSprite->GetName(true));
    
    QMenu *pEditMenu = m_pItemSprite->GetEditMenu();

    m_pUndoStack = new QUndoStack(this);
    QAction *pActionUndo = m_pUndoStack->createUndoAction(pEditMenu, "&Undo");
    pActionUndo->setIcon(QIcon(":/icons16x16/generic-undo.png"));
    pActionUndo->setShortcuts(QKeySequence::Undo);

    QAction *pActionRedo = m_pUndoStack->createRedoAction(pEditMenu, "&Redo");
    pActionRedo->setIcon(QIcon(":/icons16x16/generic-redo.png"));
    pActionRedo->setShortcuts(QKeySequence::Redo);

    pEditMenu->addAction(pActionUndo);
    pEditMenu->addAction(pActionRedo);
    pEditMenu->addSeparator();
    pEditMenu->addAction(ui->actionAddState);
    pEditMenu->addAction(ui->actionRemoveState);
    pEditMenu->addAction(ui->actionRenameState);
    pEditMenu->addAction(ui->actionOrderStateBackwards);
    pEditMenu->addAction(ui->actionOrderStateForwards);
    pEditMenu->addSeparator();
    pEditMenu->addAction(ui->actionAddFrames);
    pEditMenu->addAction(ui->actionRemoveFrame);
    pEditMenu->addAction(ui->actionOrderFrameUpwards);
    pEditMenu->addAction(ui->actionOrderFrameDownwards);

    ui->btnAddState->setDefaultAction(ui->actionAddState);
    ui->btnRemoveState->setDefaultAction(ui->actionRemoveState);
    ui->btnRenameState->setDefaultAction(ui->actionRenameState);
    ui->btnOrderStateBack->setDefaultAction(ui->actionOrderStateBackwards);
    ui->btnOrderStateForward->setDefaultAction(ui->actionOrderStateForwards);
    
    ui->btnAddFrames->setDefaultAction(ui->actionAddFrames);
    ui->btnRemoveFrame->setDefaultAction(ui->actionRemoveFrame);
    ui->btnOrderFrameUp->setDefaultAction(ui->actionOrderFrameUpwards);
    ui->btnOrderFrameDown->setDefaultAction(ui->actionOrderFrameDownwards);
    
    ui->cmbStates->clear();

    // If a .hyspr file exists, parse and initalize with it, otherwise make default empty sprite
    QFile spriteFile(m_pItemSprite->GetPath());
    if(spriteFile.exists())
    {
        if(!spriteFile.open(QIODevice::ReadOnly))
            HyGuiLog(QString("WidgetSprite::WidgetSprite() could not open ") % m_pItemSprite->GetPath(), LOGTYPE_Error);

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
        // Use this QUndoCommand to perform adding a anim state, but don't add to UndoStack because we don't want it to be undone.
        QUndoCommand *addCommand = new ItemSpriteCmd_AddState(ui->cmbStates);
        addCommand->redo();
        m_CmdsNotInUndoStack.push_back(addCommand);
    }
}

WidgetSprite::~WidgetSprite()
{
    delete ui;
    
    for(int i = 0; i < m_CmdsNotInUndoStack.count(); ++i)
        delete m_CmdsNotInUndoStack[i];
}


void WidgetSprite::on_actionAddState_triggered()
{
    QUndoCommand *addCommand = new ItemSpriteCmd_AddState(ui->cmbStates);
    m_pUndoStack->push(addCommand);
}

void WidgetSprite::on_actionRemoveState_triggered()
{
    
}

void WidgetSprite::on_actionRenameState_triggered()
{
    
}

void WidgetSprite::on_actionOrderStateBackwards_triggered()
{
    
}

void WidgetSprite::on_actionOrderStateForwards_triggered()
{
    
}

void WidgetSprite::on_actionAddFrames_triggered()
{
    
}

void WidgetSprite::on_actionRemoveFrame_triggered()
{
    
}

void WidgetSprite::on_actionOrderFrameUpwards_triggered()
{
    
}

void WidgetSprite::on_actionOrderFrameDownwards_triggered()
{
    
}
