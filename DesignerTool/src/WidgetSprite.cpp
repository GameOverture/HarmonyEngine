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

WidgetSprite::WidgetSprite(ItemSprite *pItemSprite, QWidget *parent) :  QWidget(parent),
                                                                        m_pItemSprite(pItemSprite),
                                                                        ui(new Ui::WidgetSprite)
{
    ui->setupUi(this);

    QMenu *pEditMenu = m_pItemSprite->GetEditMenu();

    m_pUndoGroup = new QUndoGroup(this);
    QAction *pActionUndo = m_pUndoGroup->createUndoAction(pEditMenu, "&Undo");
    pActionUndo->setIcon(QIcon(":/icons16x16/generic-undo.png"));
    pActionUndo->setShortcuts(QKeySequence::Undo);

    QAction *pActionRedo = m_pUndoGroup->createRedoAction(pEditMenu, "&Redo");
    pActionRedo->setIcon(QIcon(":/icons16x16/generic-redo.png"));
    pActionRedo->setShortcuts(QKeySequence::Redo);

    pEditMenu->addAction(pActionUndo);
    pEditMenu->addAction(pActionRedo);
    pEditMenu->addSeparator();

    //ui->cmbStates->addItem(

    ui->btnAddState->setDefaultAction(ui->actionAddState);
    ui->btnRemoveState->setDefaultAction(ui->actionRemoveState);
    ui->btnRenameState->setDefaultAction(ui->actionRenameState);
    ui->btnOrderStateBack->setDefaultAction(ui->actionOrderStateBackwards);
    ui->btnOrderStateForward->setDefaultAction(ui->actionOrderStateForwards);

    ui->actionAddState->setEnabled(false);
    
    ui->txtPrefixAndName->setText(m_pItemSprite->GetName(true));
}

WidgetSprite::~WidgetSprite()
{
    delete ui;
}


void WidgetSprite::on_actionAddState_triggered()
{
    QUndoCommand *addCommand = new ItemSpriteCmd_AddState();
    m_pUndoGroup->activeStack()->push(addCommand);
}
