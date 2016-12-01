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
#include <QCheckBox>
#include <QSpinBox>
#include <QRadioButton>

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemAudioCmd_CategoryChanged : public QUndoCommand
{
    WidgetAudioState &  m_WidgetAudioStateRef;

    QComboBox *         m_pCmbCategory;
    int                 m_iPrevIndex;
    int                 m_iNewIndex;
    
    QString             m_sPrev;
    QString             m_sNew;
    
public:
    ItemAudioCmd_CategoryChanged(WidgetAudioState &widgetAudioState, QComboBox *pCmb, int iPrevIndex, int iNewIndex, QUndoCommand *pParent = 0);
    virtual ~ItemAudioCmd_CategoryChanged();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemAudioCmd_CheckBox : public QUndoCommand
{
    WidgetAudioState &  m_WidgetAudioStateRef;
    QCheckBox *         m_pCheckBox;
    bool                m_bInitialValue;

public:
    ItemAudioCmd_CheckBox(WidgetAudioState &widgetFont, QCheckBox *pCheckBox, QUndoCommand *pParent = 0);
    virtual ~ItemAudioCmd_CheckBox();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemAudioCmd_PlayTypeChanged : public QUndoCommand
{
    WidgetAudioState &  m_WidgetAudioStateRef;

    QComboBox *         m_pCmbCategory;
    int                 m_iPrevIndex;
    int                 m_iNewIndex;
    
public:
    ItemAudioCmd_PlayTypeChanged(WidgetAudioState &widgetAudioState, QComboBox *pCmb, int iPrevIndex, int iNewIndex, QUndoCommand *pParent = 0);
    virtual ~ItemAudioCmd_PlayTypeChanged();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemAudioCmd_NumInstChanged : public QUndoCommand
{
    QComboBox *         m_pCmbStates;
    QSpinBox *          m_pSbNumInst;
    WidgetAudioState *  m_pAudioState;
    int                 m_iPrevSize;
    int                 m_iNewSize;

public:
    ItemAudioCmd_NumInstChanged(QComboBox *pCmbStates, QSpinBox *pSbNumInst, int iPrevSize, int iNewSize, QUndoCommand *pParent = 0);
    virtual ~ItemAudioCmd_NumInstChanged();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemAudioCmd_RadioToggle : public QUndoCommand
{
    QComboBox *         m_pCmbStates;
    WidgetAudioState *  m_pAudioState;
    QRadioButton *      m_pNewRadBtnToggle;
    QRadioButton *      m_pPrevRadBtnToggle;
    
public:
    ItemAudioCmd_RadioToggle(QComboBox *pCmbStates, QRadioButton *pNewRadBtnToggle, QRadioButton *pPrevRadBtnToggle, QUndoCommand *pParent = 0);
    virtual ~ItemAudioCmd_RadioToggle();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};


#endif // ITEMAUDIOCMDS_H
