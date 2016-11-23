/**************************************************************************
 *	ItemAudioCmds.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ITEMAUDIOCMDS_H
#define ITEMAUDIOCMDS_H

#include <QUndoCommand>
#include <QComboBox>

class WidgetAudio;
class WidgetAudioState;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemAudioCmd_AddState : public QUndoCommand
{
    QComboBox *         m_pComboBox;
    WidgetAudioState *  m_pAudioState;

public:
    ItemAudioCmd_AddState(WidgetAudio *pOwner, QList<QAction *> stateActionList, QComboBox *pCmb, QUndoCommand *pParent = 0);
    virtual ~ItemAudioCmd_AddState();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemAudioCmd_RemoveState : public QUndoCommand
{
    QComboBox *         m_pComboBox;
    WidgetAudioState *  m_pAudioState;
    int                 m_iIndex;

public:
    ItemAudioCmd_RemoveState(QComboBox *pCmb, QUndoCommand *pParent = 0);
    virtual ~ItemAudioCmd_RemoveState();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemAudioCmd_RenameState : public QUndoCommand
{
    QComboBox *         m_pComboBox;
    WidgetAudioState *  m_pAudioState;
    QString             m_sNewName;
    QString             m_sOldName;

public:
    ItemAudioCmd_RenameState(QComboBox *pCmb, QString sNewName, QUndoCommand *pParent = 0);
    virtual ~ItemAudioCmd_RenameState();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemAudioCmd_MoveStateBack : public QUndoCommand
{
    QComboBox *         m_pComboBox;
    WidgetAudioState *  m_pAudioState;

public:
    ItemAudioCmd_MoveStateBack(QComboBox *pCmb, QUndoCommand *pParent = 0);
    virtual ~ItemAudioCmd_MoveStateBack();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemAudioCmd_MoveStateForward : public QUndoCommand
{
    QComboBox *         m_pComboBox;
    WidgetAudioState *  m_pAudioState;

public:
    ItemAudioCmd_MoveStateForward(QComboBox *pCmb, QUndoCommand *pParent = 0);
    virtual ~ItemAudioCmd_MoveStateForward();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

#endif // ITEMAUDIOCMDS_H
