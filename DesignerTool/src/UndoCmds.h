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

#include <QUndoCommand>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QRadioButton>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename MODEL>
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
        m_iIndex = static_cast<MODEL *>(m_ItemRef.GetModel())->AppendState(QJsonObject());
        m_ItemRef.RefreshWidget(m_iIndex);
    }
    
    void undo() override
    {
        static_cast<MODEL *>(m_ItemRef.GetModel())->PopStateAt(m_iIndex);
        m_ItemRef.RefreshWidget(m_iIndex);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename MODEL>
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
        m_PoppedStateObj = static_cast<MODEL *>(m_ItemRef.GetModel())->PopStateAt(m_iIndex);
        m_ItemRef.RefreshWidget(m_iIndex);
    }
    
    void undo() override
    {
        m_iIndex = static_cast<MODEL *>(m_ItemRef.GetModel())->AppendState(m_PoppedStateObj);
        m_ItemRef.RefreshWidget(m_iIndex);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename MODEL>
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
        m_sOldName = static_cast<MODEL *>(m_ItemRef.GetModel())->SetStateName(m_iIndex, m_sNewName);
        m_ItemRef.RefreshWidget(m_iIndex);
    }
    
    void undo() override
    {
        static_cast<MODEL *>(m_ItemRef.GetModel())->SetStateName(m_iIndex, m_sOldName);
        m_ItemRef.RefreshWidget(m_iIndex);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename MODEL>
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
        static_cast<MODEL *>(m_ItemRef.GetModel())->MoveStateBack(m_iStateIndex);
        m_iStateIndex -= 1;

        m_ItemRef.RefreshWidget(m_iStateIndex);
    }
    
    void undo() override
    {
        static_cast<MODEL *>(m_ItemRef.GetModel())->MoveStateForward(m_iStateIndex);
        m_iStateIndex += 1;

        m_ItemRef.RefreshWidget(m_iStateIndex);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename MODEL>
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
        static_cast<MODEL *>(m_ItemRef.GetModel())->MoveStateForward(m_iStateIndex);
        m_iStateIndex += 1;

        m_ItemRef.RefreshWidget(m_iStateIndex);
    }
    
    void undo() override
    {
        static_cast<MODEL *>(m_ItemRef.GetModel())->MoveStateBack(m_iStateIndex);
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
template<typename MAPPER>
class UndoCmd_CheckBox : public QUndoCommand
{
    ProjectItem &       m_ItemRef;
    MAPPER *            m_pMapper;
    int                 m_iStateIndex;

    bool                m_bNewValue;

public:
    UndoCmd_CheckBox(QString sText, ProjectItem &itemRef, MAPPER *pMapper, int iStateIndex, QUndoCommand *pParent = 0) :    QUndoCommand(pParent),
                                                                                                                            m_ItemRef(itemRef),
                                                                                                                            m_pMapper(pMapper),
                                                                                                                            m_iStateIndex(iStateIndex),
                                                                                                                            m_bNewValue(pMapper->IsChecked())
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
template<typename OWNER>
class UndoCmd_DoubleSpinBox : public QUndoCommand
{
    OWNER *             m_pOwner;
    QDoubleSpinBox *    m_pDoubleSpinBox;
    double              m_dPrevSize;
    double              m_dNewSize;

public:
    UndoCmd_DoubleSpinBox(QString sText, OWNER *pOwner, QDoubleSpinBox *pDoubleSpinBox, double dPrevSize, double dNewSize, QUndoCommand *pParent = 0) :   QUndoCommand(pParent),
                                                                                                                                                                m_pOwner(pOwner),
                                                                                                                                                                m_pDoubleSpinBox(pDoubleSpinBox),
                                                                                                                                                                m_dPrevSize(dPrevSize),
                                                                                                                                                                m_dNewSize(dNewSize)
    {
        setText(sText);
    }
    
    virtual ~UndoCmd_DoubleSpinBox()
    { }

    void redo() override
    {
        m_pDoubleSpinBox->blockSignals(true);
        m_pDoubleSpinBox->setValue(m_dNewSize);
        m_pDoubleSpinBox->blockSignals(false);
        
        m_pOwner->UpdateActions();
    }
    
    void undo() override
    {
        m_pDoubleSpinBox->blockSignals(true);
        m_pDoubleSpinBox->setValue(m_dPrevSize);
        m_pDoubleSpinBox->blockSignals(false);
        
        m_pOwner->UpdateActions();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename OWNER>
class UndoCmd_LineEdit : public QUndoCommand
{
    OWNER *             m_pOwner;
    QLineEdit *         m_pLineEdit;
    bool                m_bFirstTimeSkipRedo;

public:
    UndoCmd_LineEdit(QString sText, OWNER *pOwner, QLineEdit *pLineEdit, QUndoCommand *pParent = 0) : QUndoCommand(pParent),
                                                                                                            m_pOwner(pOwner),
                                                                                                            m_pLineEdit(pLineEdit),
                                                                                                            m_bFirstTimeSkipRedo(true)
    {
    }
    
    virtual ~UndoCmd_LineEdit()
    { }

    void redo() override
    {
        if(m_bFirstTimeSkipRedo)
            m_bFirstTimeSkipRedo = false;
        else
            m_pLineEdit->redo();
    
        m_pOwner->UpdateActions();
        
    }
    
    void undo() override
    {
        m_pLineEdit->undo();
        m_pOwner->UpdateActions();
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
template<typename MODEL>
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
        m_Frames = static_cast<MODEL *>(m_ItemRef.GetModel())->RequestFrames(m_iStateIndex, m_Frames);
        m_ItemRef.RefreshWidget(m_iStateIndex);
    }
    
    void undo() override
    {
        static_cast<MODEL *>(m_ItemRef.GetModel())->RelinquishFrames(m_iStateIndex, m_Frames);
        m_ItemRef.RefreshWidget(m_iStateIndex);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename MODEL>
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
        static_cast<MODEL *>(m_ItemRef.GetModel())->RelinquishFrames(m_iStateIndex, m_Frames);
        m_ItemRef.RefreshWidget(m_iStateIndex);
    }
    
    void undo() override
    {
        static_cast<MODEL *>(m_ItemRef.GetModel())->RequestFrames(m_iStateIndex, m_Frames);
        m_ItemRef.RefreshWidget(m_iStateIndex);
    }
};

#endif // UNDOCMDS
