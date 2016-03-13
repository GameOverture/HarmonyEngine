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


    m_pEditMenu = new QMenu("Edit");
    m_pUndoStack = new QUndoStack();

    QAction *pActionUndo = m_pUndoStack->createUndoAction(m_pEditMenu, "&Undo");
    pActionUndo->setIcon(QIcon(":/icons16x16/generic-undo.png"));
    pActionUndo->setShortcuts(QKeySequence::Undo);

    QAction *pActionRedo = m_pUndoStack->createRedoAction(m_pEditMenu, "&Redo");
    pActionRedo->setIcon(QIcon(":/icons16x16/generic-redo.png"));
    pActionRedo->setShortcuts(QKeySequence::Redo);

    m_pEditMenu->addAction(pActionUndo);
    m_pEditMenu->addAction(pActionRedo);
    m_pEditMenu->addSeparator();
}

WidgetSprite::~WidgetSprite()
{
    delete ui;
}

void WidgetSprite::on_btnAddState_clicked()
{
    QUndoCommand *addCommand = new ItemSpriteCmd_NewState();
    m_pUndoStack->push(addCommand);
}
