/**************************************************************************
 *	WidgetUndoCmds.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETUNDOCMDS
#define WIDGETUNDOCMDS

#include "HyGlobal.h"
#include "ItemWidget.h"

#include "ItemProject.h"

#include <QUndoCommand>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QRadioButton>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename OWNER, typename STATE>
class WidgetUndoCmd_AddState : public QUndoCommand
{
    OWNER *             m_pOwner;
    STATE *             m_pState;
    QComboBox *         m_pComboBox;

public:
    WidgetUndoCmd_AddState(QString sText, OWNER *pOwner, QList<QAction *> stateActionList, QComboBox *pCmb, QUndoCommand *pParent = 0) :    QUndoCommand(pParent),
                                                                                                                                            m_pOwner(pOwner),
                                                                                                                                            m_pState(new STATE(m_pOwner, stateActionList)),
                                                                                                                                            m_pComboBox(pCmb)
    {
        setText(sText);
    }
    
    virtual ~WidgetUndoCmd_AddState()
    { }

    void redo() Q_DECL_OVERRIDE
    {
        int iIndex = m_pComboBox->count();
    
        QVariant v;
        v.setValue(m_pState);
    
        m_pComboBox->addItem(QString::number(iIndex) % " - " % m_pState->GetName(), v);
        SetStateNamingConventionInComboBox<STATE>(m_pComboBox);
    
        m_pComboBox->setCurrentIndex(iIndex);
        
        m_pOwner->UpdateActions();
    }
    
    void undo() Q_DECL_OVERRIDE
    {
        QVariant v;
        v.setValue(m_pState);
    
        int iIndex = m_pComboBox->findData(v);
        m_pComboBox->removeItem(iIndex);
    
        SetStateNamingConventionInComboBox<STATE>(m_pComboBox);
        
        m_pOwner->UpdateActions();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename OWNER, typename STATE>
class WidgetUndoCmd_RemoveState : public QUndoCommand
{
    OWNER *             m_pOwner;
    QComboBox *         m_pComboBox;
    STATE *             m_pState;
    int                 m_iIndex;

public:
    WidgetUndoCmd_RemoveState(QString sText, OWNER *pOwner, QComboBox *pCmb, QUndoCommand *pParent = 0) :   QUndoCommand(pParent),
                                                                                                            m_pOwner(pOwner),
                                                                                                            m_pComboBox(pCmb),
                                                                                                            m_pState(m_pComboBox->currentData().value<STATE *>()),
                                                                                                            m_iIndex(m_pComboBox->currentIndex())
    {
        setText(sText);
    }

    virtual ~WidgetUndoCmd_RemoveState()
    { }

    void redo() Q_DECL_OVERRIDE
    {
        m_pComboBox->removeItem(m_iIndex);
        SetStateNamingConventionInComboBox<STATE>(m_pComboBox);
        
        m_pOwner->UpdateActions();
    }
    
    void undo() Q_DECL_OVERRIDE
    {
        QVariant v;
        v.setValue(m_pState);
    
        m_pComboBox->insertItem(m_iIndex, QString::number(m_iIndex) % " - " % m_pState->GetName(), v);
        m_pComboBox->setCurrentIndex(m_iIndex);
    
        SetStateNamingConventionInComboBox<STATE>(m_pComboBox);
        
        m_pOwner->UpdateActions();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename STATE>
class WidgetUndoCmd_RenameState : public QUndoCommand
{
    QComboBox *         m_pComboBox;
    STATE *             m_pState;
    QString             m_sNewName;
    QString             m_sOldName;

public:
    WidgetUndoCmd_RenameState(QString sText, QComboBox *pCmb, QString sNewName, QUndoCommand *pParent = 0) :    QUndoCommand(pParent),
                                                                                                                m_pComboBox(pCmb),
                                                                                                                m_pState(m_pComboBox->currentData().value<STATE *>()),
                                                                                                                m_sNewName(sNewName),
                                                                                                                m_sOldName(m_pState->GetName())
    {
        setText(sText);
    }
    
    virtual ~WidgetUndoCmd_RenameState()
    { }

    void redo() Q_DECL_OVERRIDE
    {
        m_pState->SetName(m_sNewName);
        SetStateNamingConventionInComboBox<STATE>(m_pComboBox);
    
        for(int i = 0; i < m_pComboBox->count(); ++i)
        {
            if(m_pComboBox->itemData(i).value<STATE *>() == m_pState)
            {
                m_pComboBox->setCurrentIndex(i);
                break;
            }
        }
    }
    
    void undo() Q_DECL_OVERRIDE
    {
        m_pState->SetName(m_sOldName);
        SetStateNamingConventionInComboBox<STATE>(m_pComboBox);
    
        for(int i = 0; i < m_pComboBox->count(); ++i)
        {
            if(m_pComboBox->itemData(i).value<STATE *>() == m_pState)
            {
                m_pComboBox->setCurrentIndex(i);
                break;
            }
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename OWNER, typename STATE>
class WidgetUndoCmd_MoveStateBack : public QUndoCommand
{
    OWNER *             m_pOwner;
    QComboBox *         m_pComboBox;
    STATE *             m_pState;

public:
    WidgetUndoCmd_MoveStateBack(QString sText, OWNER *pOwner, QComboBox *pCmb, QUndoCommand *pParent = 0) : QUndoCommand(pParent),
                                                                                                            m_pOwner(pOwner),
                                                                                                            m_pComboBox(pCmb),
                                                                                                            m_pState(m_pComboBox->currentData().value<STATE *>())
    {
        setText(sText);
    }
    
    virtual ~WidgetUndoCmd_MoveStateBack()
    { }

    void redo() Q_DECL_OVERRIDE
    {
        QVariant v;
        v.setValue(m_pState);
    
        int iIndex = m_pComboBox->findData(v);
    
        m_pComboBox->removeItem(iIndex);
        iIndex -= 1;
        m_pComboBox->insertItem(iIndex, QString::number(iIndex) % " - " % m_pState->GetName(), v);
        m_pComboBox->setCurrentIndex(iIndex);
    
        SetStateNamingConventionInComboBox<STATE>(m_pComboBox);
        
        m_pOwner->UpdateActions();
    }
    
    void undo() Q_DECL_OVERRIDE
    {
        QVariant v;
        v.setValue(m_pState);
    
        int iIndex = m_pComboBox->findData(v);
    
        m_pComboBox->removeItem(iIndex);
        iIndex += 1;
        m_pComboBox->insertItem(iIndex, QString::number(iIndex) % " - " % m_pState->GetName(), v);
        m_pComboBox->setCurrentIndex(iIndex);
    
        SetStateNamingConventionInComboBox<STATE>(m_pComboBox);
        
        m_pOwner->UpdateActions();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename OWNER, typename STATE>
class WidgetUndoCmd_MoveStateForward : public QUndoCommand
{
    OWNER *             m_pOwner;
    QComboBox *         m_pComboBox;
    STATE *             m_pState;

public:
    WidgetUndoCmd_MoveStateForward(QString sText, OWNER *pOwner, QComboBox *pCmb, QUndoCommand *pParent = 0) :  QUndoCommand(pParent),
                                                                                                                m_pOwner(pOwner),
                                                                                                                m_pComboBox(pCmb),
                                                                                                                m_pState(m_pComboBox->currentData().value<STATE *>())
    {
        setText(sText);
    }
    
    virtual ~WidgetUndoCmd_MoveStateForward()
    { }

    void redo() Q_DECL_OVERRIDE
    {
        QVariant v;
        v.setValue(m_pState);
    
        int iIndex = m_pComboBox->findData(v);
    
        m_pComboBox->removeItem(iIndex);
        iIndex += 1;
        m_pComboBox->insertItem(iIndex, QString::number(iIndex) % " - " % m_pState->GetName(), v);
        m_pComboBox->setCurrentIndex(iIndex);
    
        SetStateNamingConventionInComboBox<STATE>(m_pComboBox);
        
        m_pOwner->UpdateActions();
    }
    
    void undo() Q_DECL_OVERRIDE
    {
        QVariant v;
        v.setValue(m_pState);
    
        int iIndex = m_pComboBox->findData(v);
    
        m_pComboBox->removeItem(iIndex);
        iIndex -= 1;
        m_pComboBox->insertItem(iIndex, QString::number(iIndex) % " - " % m_pState->GetName(), v);
        m_pComboBox->setCurrentIndex(iIndex);
    
        SetStateNamingConventionInComboBox<STATE>(m_pComboBox);
        
        m_pOwner->UpdateActions();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename OWNER>
class WidgetUndoCmd_ComboBox : public QUndoCommand
{
    OWNER *             m_pOwner;

    QComboBox *         m_pComboBox;
    int                 m_iPrevIndex;
    int                 m_iNewIndex;
    
public:
    WidgetUndoCmd_ComboBox(QString sText, OWNER *pOwner, QComboBox *pCmb, int iPrevIndex, int iNewIndex, QUndoCommand *pParent = 0) :   QUndoCommand(pParent),
                                                                                                                                        m_pOwner(pOwner),
                                                                                                                                        m_pComboBox(pCmb),
                                                                                                                                        m_iPrevIndex(iPrevIndex),
                                                                                                                                        m_iNewIndex(iNewIndex)
    {
        setText(sText);
    }
            
    virtual ~WidgetUndoCmd_ComboBox()
    { }

    void redo() Q_DECL_OVERRIDE
    {
        m_pComboBox->blockSignals(true);
        m_pComboBox->setCurrentIndex(m_iNewIndex);
        m_pComboBox->blockSignals(false);
        
        m_pOwner->UpdateActions();
    }
    
    void undo() Q_DECL_OVERRIDE
    {
        m_pComboBox->blockSignals(true);
        m_pComboBox->setCurrentIndex(m_iPrevIndex);
        m_pComboBox->blockSignals(false);
        
        m_pOwner->UpdateActions();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename OWNER>
class WidgetUndoCmd_CheckBox : public QUndoCommand
{
    OWNER *             m_pOwner;
    QCheckBox *         m_pCheckBox;
    bool                m_bInitialValue;

public:
    WidgetUndoCmd_CheckBox(OWNER *pOwner, QCheckBox *pCheckBox, QUndoCommand *pParent = 0) :    QUndoCommand(pParent),
                                                                                                m_pOwner(pOwner),
                                                                                                m_pCheckBox(pCheckBox)
    {
        m_bInitialValue = m_pCheckBox->isChecked();
        setText((m_bInitialValue ? "Checked " : "Unchecked ") % m_pCheckBox->text());
    }
    
    virtual ~WidgetUndoCmd_CheckBox()
    { }

    void redo() Q_DECL_OVERRIDE
    {
        m_pCheckBox->setChecked(m_bInitialValue);
        m_pOwner->UpdateActions();
    }
    
    void undo() Q_DECL_OVERRIDE
    {
        m_pCheckBox->setChecked(!m_bInitialValue);
        m_pOwner->UpdateActions();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename OWNER>
class WidgetUndoCmd_SpinBox : public QUndoCommand
{
    OWNER *             m_pOwner;
    QSpinBox *          m_pSpinBox;
    int                 m_iPrevSize;
    int                 m_iNewSize;

public:
    WidgetUndoCmd_SpinBox(QString sText, OWNER *pOwner, QSpinBox *pSpinBox, int iPrevSize, int iNewSize, QUndoCommand *pParent = 0) :   QUndoCommand(pParent),
                                                                                                                                        m_pOwner(pOwner),
                                                                                                                                        m_pSpinBox(pSpinBox),
                                                                                                                                        m_iPrevSize(iPrevSize),
                                                                                                                                        m_iNewSize(iNewSize)
    {
        setText(sText);
    }
    
    virtual ~WidgetUndoCmd_SpinBox()
    { }

    void redo() Q_DECL_OVERRIDE
    {
        m_pSpinBox->blockSignals(true);
        m_pSpinBox->setValue(m_iNewSize);
        m_pSpinBox->blockSignals(false);
        
        m_pOwner->UpdateActions();
    }
    
    void undo() Q_DECL_OVERRIDE
    {
        m_pSpinBox->blockSignals(true);
        m_pSpinBox->setValue(m_iPrevSize);
        m_pSpinBox->blockSignals(false);
        
        m_pOwner->UpdateActions();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename OWNER>
class WidgetUndoCmd_DoubleSpinBox : public QUndoCommand
{
    OWNER *             m_pOwner;
    QDoubleSpinBox *    m_pDoubleSpinBox;
    double              m_dPrevSize;
    double              m_dNewSize;

public:
    WidgetUndoCmd_DoubleSpinBox(QString sText, OWNER *pOwner, QDoubleSpinBox *pDoubleSpinBox, double dPrevSize, double dNewSize, QUndoCommand *pParent = 0) :   QUndoCommand(pParent),
                                                                                                                                                                m_pOwner(pOwner),
                                                                                                                                                                m_pDoubleSpinBox(pDoubleSpinBox),
                                                                                                                                                                m_dPrevSize(dPrevSize),
                                                                                                                                                                m_dNewSize(dNewSize)
    {
        setText(sText);
    }
    
    virtual ~WidgetUndoCmd_DoubleSpinBox()
    { }

    void redo() Q_DECL_OVERRIDE
    {
        m_pDoubleSpinBox->blockSignals(true);
        m_pDoubleSpinBox->setValue(m_dNewSize);
        m_pDoubleSpinBox->blockSignals(false);
        
        m_pOwner->UpdateActions();
    }
    
    void undo() Q_DECL_OVERRIDE
    {
        m_pDoubleSpinBox->blockSignals(true);
        m_pDoubleSpinBox->setValue(m_dPrevSize);
        m_pDoubleSpinBox->blockSignals(false);
        
        m_pOwner->UpdateActions();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename OWNER>
class WidgetUndoCmd_LineEdit : public QUndoCommand
{
    OWNER *             m_pOwner;
    QLineEdit *         m_pLineEdit;
    bool                m_bFirstTimeSkipRedo;

public:
    WidgetUndoCmd_LineEdit(QString sText, OWNER *pOwner, QLineEdit *pLineEdit, QUndoCommand *pParent = 0) : QUndoCommand(pParent),
                                                                                                            m_pOwner(pOwner),
                                                                                                            m_pLineEdit(pLineEdit),
                                                                                                            m_bFirstTimeSkipRedo(true)
    {
    }
    
    virtual ~WidgetUndoCmd_LineEdit()
    { }

    void redo() Q_DECL_OVERRIDE
    {
        if(m_bFirstTimeSkipRedo)
            m_bFirstTimeSkipRedo = false;
        else
            m_pLineEdit->redo();
    
        m_pOwner->UpdateActions();
        
    }
    
    void undo() Q_DECL_OVERRIDE
    {
        m_pLineEdit->undo();
        m_pOwner->UpdateActions();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename OWNER>
class WidgetUndoCmd_RadioToggle : public QUndoCommand
{
    QComboBox *         m_pCmbStates;
    OWNER *             m_pOwner;
    QRadioButton *      m_pNewRadBtnToggle;
    QRadioButton *      m_pPrevRadBtnToggle;
    
public:
    WidgetUndoCmd_RadioToggle(QString sText, OWNER *pOwner, QRadioButton *pNewRadBtnToggle, QRadioButton *pPrevRadBtnToggle, QUndoCommand *pParent = 0) :   QUndoCommand(pParent),
                                                                                                                                                            m_pOwner(pOwner),
                                                                                                                                                            m_pNewRadBtnToggle(pNewRadBtnToggle),
                                                                                                                                                            m_pPrevRadBtnToggle(pPrevRadBtnToggle)
    {
        setText(sText);
    }
    
    virtual ~WidgetUndoCmd_RadioToggle()
    { }
    
    void redo() Q_DECL_OVERRIDE
    {
        m_pNewRadBtnToggle->blockSignals(true);
        m_pPrevRadBtnToggle->blockSignals(true);
        
        m_pNewRadBtnToggle->setChecked(true);
        
        m_pNewRadBtnToggle->blockSignals(false);
        m_pPrevRadBtnToggle->blockSignals(false);
        
        m_pOwner->UpdateActions();
    }
    
    void undo() Q_DECL_OVERRIDE
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
template<typename OWNER>
class WidgetUndoCmd_AddFrames : public QUndoCommand
{
    OWNER *                                 m_pOwner;
    QList<HyGuiFrame *>                     m_Frames;
    
public:
    WidgetUndoCmd_AddFrames(QString sText, OWNER *pOwner, QUndoCommand *pParent = 0) :  QUndoCommand(pParent),
                                                                                        m_pOwner(pOwner)
    {
        setText(sText);
        m_Frames.clear();
    }
    
    virtual ~WidgetUndoCmd_AddFrames()
    { }

    void redo() Q_DECL_OVERRIDE
    {
        m_Frames = m_pOwner->GetData()->GetItemProject()->GetAtlasesData().RequestFrames(m_pOwner->GetData(), m_Frames);
        m_pOwner->UpdateActions();
    }
    
    void undo() Q_DECL_OVERRIDE
    {
        m_pOwner->GetData()->GetItemProject()->GetAtlasesData().RelinquishFrames(m_pOwner->GetData(), m_Frames);
        m_pOwner->UpdateActions();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename OWNER>
class WidgetUndoCmd_DeleteFrame : public QUndoCommand
{
    OWNER *                                 m_pOwner;
    QList<HyGuiFrame *>                     m_Frames;

public:
    WidgetUndoCmd_DeleteFrame(QString sText, OWNER *pOwner, HyGuiFrame *pFrame, QUndoCommand *pParent = 0) :    QUndoCommand(pParent),
                                                                                                                m_pOwner(pOwner)
    {
        setText(sText);
        m_Frames.append(pFrame);
    }
    
    virtual ~WidgetUndoCmd_DeleteFrame()
    { }

    void redo() Q_DECL_OVERRIDE
    {
        m_pOwner->GetData()->GetItemProject()->GetAtlasesData().RelinquishFrames(m_pOwner->GetData(), m_Frames);
        m_pOwner->UpdateActions();
    }
    
    void undo() Q_DECL_OVERRIDE
    {
        m_Frames = m_pOwner->GetData()->GetItemProject()->GetAtlasesData().RequestFrames(m_pOwner->GetData(), m_Frames);
        m_pOwner->UpdateActions();
    }
};

#endif // WIDGETUNDOCMDS
