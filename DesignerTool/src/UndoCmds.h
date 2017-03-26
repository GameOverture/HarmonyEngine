/**************************************************************************
 *	UndoCmds.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef UNDOCMDS
#define UNDOCMDS

#include "HyGuiGlobal.h"
#include "ProjectItem.h"
#include "Project.h"
#include "IModel.h"

#include <QUndoCommand>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QRadioButton>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename STATEDATA>
class UndoCmd_AddState : public QUndoCommand
{
    ProjectItem &       m_ItemRef;
    int                 m_iIndex;

public:
    UndoCmd_AddState(QString sText, ProjectItem &itemRef, QUndoCommand *pParent = 0) :  QUndoCommand(pParent),
                                                                                        m_ItemRef(itemRef)
    {
        setText(sText);
    }
    
    virtual ~UndoCmd_AddState()
    { }

    void redo() override
    {
        m_iIndex = static_cast<IModel *>(m_ItemRef.GetModel())->AppendState<STATEDATA>(QJsonObject());
        m_ItemRef.RefreshWidget(m_iIndex);
    }
    
    void undo() override
    {
        static_cast<IModel *>(m_ItemRef.GetModel())->PopStateAt(m_iIndex);
        m_ItemRef.RefreshWidget(m_iIndex);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename STATEDATA>
class UndoCmd_RemoveState : public QUndoCommand
{
    ProjectItem &       m_ItemRef;
    int                 m_iIndex;
    QJsonObject         m_PoppedStateObj;

public:
    UndoCmd_RemoveState(QString sText, ProjectItem &itemRef, int iIndex, QUndoCommand *pParent = 0) :   QUndoCommand(pParent),
                                                                                                        m_ItemRef(itemRef),
                                                                                                        m_iIndex(iIndex)
    {
        setText(sText);
    }

    virtual ~UndoCmd_RemoveState()
    { }

    void redo() override
    {
        m_PoppedStateObj = static_cast<IModel *>(m_ItemRef.GetModel())->PopStateAt(m_iIndex);
        m_ItemRef.RefreshWidget(m_iIndex);
    }
    
    void undo() override
    {
        m_iIndex = static_cast<IModel *>(m_ItemRef.GetModel())->AppendState<STATEDATA>(m_PoppedStateObj);
        m_ItemRef.RefreshWidget(m_iIndex);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class UndoCmd_RenameState : public QUndoCommand
{
    ProjectItem &       m_ItemRef;
    int                 m_iIndex;

    QString             m_sNewName;
    QString             m_sOldName;

public:
    UndoCmd_RenameState(QString sText, ProjectItem &itemRef, QString sNewName, int iIndex, QUndoCommand *pParent = 0) : QUndoCommand(pParent),
                                                                                                                        m_ItemRef(itemRef),
                                                                                                                        m_iIndex(iIndex),
                                                                                                                        m_sNewName(sNewName)
    {
        setText(sText);
    }
    
    virtual ~UndoCmd_RenameState()
    { }

    void redo() override
    {
        m_sOldName = static_cast<IModel *>(m_ItemRef.GetModel())->SetStateName(m_iIndex, m_sNewName);
        m_ItemRef.RefreshWidget(m_iIndex);
    }
    
    void undo() override
    {
        static_cast<IModel *>(m_ItemRef.GetModel())->SetStateName(m_iIndex, m_sOldName);
        m_ItemRef.RefreshWidget(m_iIndex);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class UndoCmd_MoveStateBack : public QUndoCommand
{
    ProjectItem &       m_ItemRef;
    int                 m_iStateIndex;

public:
    UndoCmd_MoveStateBack(QString sText, ProjectItem &itemRef, int iStateIndex, QUndoCommand *pParent = 0) :    QUndoCommand(pParent),
                                                                                                                m_ItemRef(itemRef),
                                                                                                                m_iStateIndex(iStateIndex)
    {
        setText(sText);
    }
    
    virtual ~UndoCmd_MoveStateBack()
    { }

    void redo() override
    {
        static_cast<IModel *>(m_ItemRef.GetModel())->MoveStateBack(m_iStateIndex);
        m_iStateIndex -= 1;

        m_ItemRef.RefreshWidget(m_iStateIndex);
    }
    
    void undo() override
    {
        static_cast<IModel *>(m_ItemRef.GetModel())->MoveStateForward(m_iStateIndex);
        m_iStateIndex += 1;

        m_ItemRef.RefreshWidget(m_iStateIndex);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class UndoCmd_MoveStateForward : public QUndoCommand
{
    ProjectItem &       m_ItemRef;
    int                 m_iStateIndex;

public:
    UndoCmd_MoveStateForward(QString sText, ProjectItem &itemRef, int iStateIndex, QUndoCommand *pParent = 0) : QUndoCommand(pParent),
                                                                                                                m_ItemRef(itemRef),
                                                                                                                m_iStateIndex(iStateIndex)
    {
        setText(sText);
    }
    
    virtual ~UndoCmd_MoveStateForward()
    { }

    void redo() override
    {
        static_cast<IModel *>(m_ItemRef.GetModel())->MoveStateForward(m_iStateIndex);
        m_iStateIndex += 1;

        m_ItemRef.RefreshWidget(m_iStateIndex);
    }
    
    void undo() override
    {
        static_cast<IModel *>(m_ItemRef.GetModel())->MoveStateBack(m_iStateIndex);
        m_iStateIndex -= 1;

        m_ItemRef.RefreshWidget(m_iStateIndex);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename MAPPER>
class UndoCmd_ComboBox : public QUndoCommand
{
    ProjectItem &       m_ItemRef;
    MAPPER *            m_pMapper;
    int                 m_iStateIndex;

    int                 m_iPrevValue;
    int                 m_iNewValue;
    
public:
    UndoCmd_ComboBox(QString sText, ProjectItem &itemRef, MAPPER *pMapper, int iStateIndex, int iPrevValue, int iNewValue, QUndoCommand *pParent = 0) : QUndoCommand(pParent),
                                                                                                                                                        m_ItemRef(itemRef),
                                                                                                                                                        m_pMapper(pMapper),
                                                                                                                                                        m_iStateIndex(iStateIndex),
                                                                                                                                                        m_iPrevValue(iPrevValue),
                                                                                                                                                        m_iNewValue(iNewValue)
    {
        setText(sText);
    }
            
    virtual ~UndoCmd_ComboBox()
    { }

    void redo() override
    {
        m_pMapper->SetIndex(m_iNewValue);
        m_ItemRef.RefreshWidget(m_iStateIndex);
    }
    
    void undo() override
    {
        m_pMapper->SetIndex(m_iPrevValue);
        m_ItemRef.RefreshWidget(m_iStateIndex);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class UndoCmd_CheckBox : public QUndoCommand
{
    ProjectItem &       m_ItemRef;
    CheckBoxMapper *    m_pMapper;
    int                 m_iStateIndex;

    bool                m_bNewValue;

public:
    UndoCmd_CheckBox(QString sText, ProjectItem &itemRef, CheckBoxMapper *pMapper, int iStateIndex, QUndoCommand *pParent = 0) :    QUndoCommand(pParent),
                                                                                                                                    m_ItemRef(itemRef),
                                                                                                                                    m_pMapper(pMapper),
                                                                                                                                    m_iStateIndex(iStateIndex),
                                                                                                                                    m_bNewValue(!pMapper->IsChecked())
    {
        setText((m_bNewValue ? "Checked " : "Unchecked ") % sText);
    }
    
    virtual ~UndoCmd_CheckBox()
    { }

    void redo() override
    {
        m_pMapper->SetChecked(m_bNewValue);
        m_ItemRef.RefreshWidget(m_iStateIndex);
    }
    
    void undo() override
    {
        m_pMapper->SetChecked(!m_bNewValue);
        m_ItemRef.RefreshWidget(m_iStateIndex);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename OWNER>
class UndoCmd_SpinBox : public QUndoCommand
{
    OWNER *             m_pOwner;
    QSpinBox *          m_pSpinBox;
    int                 m_iPrevSize;
    int                 m_iNewSize;

public:
    UndoCmd_SpinBox(QString sText, OWNER *pOwner, QSpinBox *pSpinBox, int iPrevSize, int iNewSize, QUndoCommand *pParent = 0) :   QUndoCommand(pParent),
                                                                                                                                        m_pOwner(pOwner),
                                                                                                                                        m_pSpinBox(pSpinBox),
                                                                                                                                        m_iPrevSize(iPrevSize),
                                                                                                                                        m_iNewSize(iNewSize)
    {
        setText(sText);
    }
    
    virtual ~UndoCmd_SpinBox()
    { }

    void redo() override
    {
        m_pSpinBox->blockSignals(true);
        m_pSpinBox->setValue(m_iNewSize);
        m_pSpinBox->blockSignals(false);
        
        m_pOwner->UpdateActions();
    }
    
    void undo() override
    {
        m_pSpinBox->blockSignals(true);
        m_pSpinBox->setValue(m_iPrevSize);
        m_pSpinBox->blockSignals(false);
        
        m_pOwner->UpdateActions();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class UndoCmd_DoubleSpinBox : public QUndoCommand
{
    ProjectItem &           m_ItemRef;
    DoubleSpinBoxMapper *   m_pMapper;
    int                     m_iStateIndex;

    double                  m_dNew;
    double                  m_dOld;

public:
    UndoCmd_DoubleSpinBox(QString sText, ProjectItem &itemRef, DoubleSpinBoxMapper *pMapper, int iStateIndex, double dNew, double dOld, QUndoCommand *pParent = 0) :    QUndoCommand(pParent),
                                                                                                                                                                        m_ItemRef(itemRef),
                                                                                                                                                                        m_pMapper(pMapper),
                                                                                                                                                                        m_iStateIndex(iStateIndex),
                                                                                                                                                                        m_dNew(dNew),
                                                                                                                                                                        m_dOld(dOld)
    {
        setText(sText);
    }
    
    virtual ~UndoCmd_DoubleSpinBox()
    { }

    void redo() override
    {
        m_pMapper->SetValue(m_dNew);
        m_ItemRef.RefreshWidget(m_iStateIndex);
    }
    
    void undo() override
    {
        m_pMapper->SetValue(m_dOld);
        m_ItemRef.RefreshWidget(m_iStateIndex);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class UndoCmd_LineEdit : public QUndoCommand
{
    ProjectItem &       m_ItemRef;
    LineEditMapper *    m_pMapper;
    int                 m_iStateIndex;

    QString             m_sNew;
    QString             m_sOld;

public:
    UndoCmd_LineEdit(QString sText, ProjectItem &itemRef, LineEditMapper *pMapper, int iStateIndex, QString sNew, QString sOld, QUndoCommand *pParent = 0) :    QUndoCommand(pParent),
                                                                                                                                                                m_ItemRef(itemRef),
                                                                                                                                                                m_pMapper(pMapper),
                                                                                                                                                                m_iStateIndex(iStateIndex),
                                                                                                                                                                m_sNew(sNew),
                                                                                                                                                                m_sOld(sOld)
    {
    }
    
    virtual ~UndoCmd_LineEdit()
    { }

    void redo() override
    {
        m_pMapper->SetString(m_sNew);
        m_ItemRef.RefreshWidget(m_iStateIndex);
    }
    
    void undo() override
    {
        m_pMapper->SetString(m_sOld);
        m_ItemRef.RefreshWidget(m_iStateIndex);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename OWNER>
class UndoCmd_RadioToggle : public QUndoCommand
{
    QComboBox *         m_pCmbStates;
    OWNER *             m_pOwner;
    QRadioButton *      m_pNewRadBtnToggle;
    QRadioButton *      m_pPrevRadBtnToggle;
    
public:
    UndoCmd_RadioToggle(QString sText, OWNER *pOwner, QRadioButton *pNewRadBtnToggle, QRadioButton *pPrevRadBtnToggle, QUndoCommand *pParent = 0) :   QUndoCommand(pParent),
                                                                                                                                                            m_pOwner(pOwner),
                                                                                                                                                            m_pNewRadBtnToggle(pNewRadBtnToggle),
                                                                                                                                                            m_pPrevRadBtnToggle(pPrevRadBtnToggle)
    {
        setText(sText);
    }
    
    virtual ~UndoCmd_RadioToggle()
    { }
    
    void redo() override
    {
        m_pNewRadBtnToggle->blockSignals(true);
        m_pPrevRadBtnToggle->blockSignals(true);
        
        m_pNewRadBtnToggle->setChecked(true);
        
        m_pNewRadBtnToggle->blockSignals(false);
        m_pPrevRadBtnToggle->blockSignals(false);
        
        m_pOwner->UpdateActions();
    }
    
    void undo() override
    {
        m_pNewRadBtnToggle->blockSignals(true);
        m_pPrevRadBtnToggle->blockSignals(true);
        
        m_pPrevRadBtnToggle->setChecked(true);
        
        m_pNewRadBtnToggle->blockSignals(false);
        m_pPrevRadBtnToggle->blockSignals(false);
        
        m_pOwner->UpdateActions();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class UndoCmd_AddFrames : public QUndoCommand
{
    ProjectItem &           m_ItemRef;
    int                     m_iStateIndex;

    QList<AtlasFrame *>     m_Frames;
    
public:
    UndoCmd_AddFrames(QString sText, ProjectItem &itemRef, int iStateIndex, QUndoCommand *pParent = 0) :    QUndoCommand(pParent),
                                                                                                            m_ItemRef(itemRef),
                                                                                                            m_iStateIndex(iStateIndex)
    {
        setText(sText);
        m_Frames.clear();
    }
    
    virtual ~UndoCmd_AddFrames()
    { }

    void redo() override
    {
        m_Frames = static_cast<IModel *>(m_ItemRef.GetModel())->RequestFrames(m_iStateIndex, m_Frames);
        m_ItemRef.RefreshWidget(m_iStateIndex);
    }
    
    void undo() override
    {
        static_cast<IModel *>(m_ItemRef.GetModel())->RelinquishFrames(m_iStateIndex, m_Frames);
        m_ItemRef.RefreshWidget(m_iStateIndex);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class UndoCmd_DeleteFrame : public QUndoCommand
{
    ProjectItem &           m_ItemRef;
    int                     m_iStateIndex;

    QList<AtlasFrame *>     m_Frames;

public:
    UndoCmd_DeleteFrame(QString sText, ProjectItem &itemRef, int iStateIndex, AtlasFrame *pFrame, QUndoCommand *pParent = 0) :  QUndoCommand(pParent),
                                                                                                                                m_ItemRef(itemRef),
                                                                                                                                m_iStateIndex(iStateIndex)
    {
        setText(sText);
        m_Frames.append(pFrame);
    }
    
    virtual ~UndoCmd_DeleteFrame()
    { }

    void redo() override
    {
        static_cast<IModel *>(m_ItemRef.GetModel())->RelinquishFrames(m_iStateIndex, m_Frames);
        m_ItemRef.RefreshWidget(m_iStateIndex);
    }
    
    void undo() override
    {
        static_cast<IModel *>(m_ItemRef.GetModel())->RequestFrames(m_iStateIndex, m_Frames);
        m_ItemRef.RefreshWidget(m_iStateIndex);
    }
};

#endif // UNDOCMDS
