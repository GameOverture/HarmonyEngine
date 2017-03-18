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
#include "IProjItem.h"
#include "Project.h"

#include <QUndoCommand>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QRadioButton>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename WIDGET, typename MODEL>
class UndoCmd_AddState : public QUndoCommand
{
    WIDGET *            m_pWidget;
    MODEL *             m_pModel;
    QComboBox *         m_pComboBox;

    int                 m_iIndex;

public:
    UndoCmd_AddState(QString sText, WIDGET *pWidget, MODEL *pModel, QComboBox *pCmb, QUndoCommand *pParent = 0) :   QUndoCommand(pParent),
                                                                                                                    m_pWidget(pWidget),
                                                                                                                    m_pModel(pModel),
                                                                                                                    m_pComboBox(pCmb)
    {
        setText(sText);
    }
    
    virtual ~UndoCmd_AddState()
    { }

    void redo() override
    {
        m_iIndex = m_pModel->AppendState(QJsonObject());
        m_pWidget->UpdateActions();

        m_pComboBox->setCurrentIndex(m_iIndex);
    }
    
    void undo() override
    {
        m_pModel->PopStateAt(m_iIndex);
        m_pWidget->UpdateActions();

        m_pComboBox->setCurrentIndex(m_iIndex);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename WIDGET, typename MODEL>
class UndoCmd_RemoveState : public QUndoCommand
{
    WIDGET *            m_pWidget;
    MODEL *             m_pModel;
    QComboBox *         m_pComboBox;

    int                 m_iIndex;
    QJsonObject         m_PoppedStateObj;

public:
    UndoCmd_RemoveState(QString sText, WIDGET *pWidget, MODEL *pModel, int iIndex, QComboBox *pCmb, QUndoCommand *pParent = 0) :    QUndoCommand(pParent),
                                                                                                                                    m_pWidget(pWidget),
                                                                                                                                    m_pModel(pModel),
                                                                                                                                    m_pComboBox(pCmb),
                                                                                                                                    m_iIndex(iIndex)
    {
        setText(sText);
    }

    virtual ~UndoCmd_RemoveState()
    { }

    void redo() override
    {
        m_PoppedStateObj = m_pModel->PopStateAt(m_iIndex);
        m_pWidget->UpdateActions();

        m_pComboBox->setCurrentIndex(m_iIndex);
    }
    
    void undo() override
    {
        m_iIndex = m_pModel->AppendState(m_PoppedStateObj);
        m_pWidget->UpdateActions();

        m_pComboBox->setCurrentIndex(m_iIndex);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename MODEL>
class UndoCmd_RenameState : public QUndoCommand
{
    MODEL *             m_pModel;
    QComboBox *         m_pComboBox;
    int                 m_iIndex;

    QString             m_sNewName;
    QString             m_sOldName;

public:
    UndoCmd_RenameState(QString sText, MODEL *pModel, int iIndex, QString sNewName, QComboBox *pCmb, QUndoCommand *pParent = 0) :   QUndoCommand(pParent),
                                                                                                                                    m_pModel(pModel),
                                                                                                                                    m_pComboBox(pCmb),
                                                                                                                                    m_iIndex(iIndex),
                                                                                                                                    m_sNewName(sNewName)
    {
        setText(sText);
    }
    
    virtual ~UndoCmd_RenameState()
    { }

    void redo() override
    {
        m_sOldName = m_pModel->SetStateName(m_iIndex, m_sNewName);
        m_pComboBox->setCurrentIndex(m_iIndex);
    }
    
    void undo() override
    {
        m_pModel->SetStateName(m_iIndex, m_sOldName);
        m_pComboBox->setCurrentIndex(m_iIndex);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename WIDGET, typename MODEL>
class UndoCmd_MoveStateBack : public QUndoCommand
{
    WIDGET *            m_pWidget;
    MODEL *             m_pModel;
    QComboBox *         m_pComboBox;
    int                 m_iIndex;

public:
    UndoCmd_MoveStateBack(QString sText, WIDGET *pWidget, MODEL *pModel, QComboBox *pCmb, QUndoCommand *pParent = 0) :  QUndoCommand(pParent),
                                                                                                                        m_pWidget(pWidget),
                                                                                                                        m_pModel(pModel),
                                                                                                                        m_pComboBox(pCmb),
                                                                                                                        m_iIndex(m_pComboBox->currentIndex())
    {
        setText(sText);
    }
    
    virtual ~UndoCmd_MoveStateBack()
    { }

    void redo() override
    {
        m_pModel->MoveStateBack(m_iIndex);
        m_iIndex -= 1;
        m_pComboBox->setCurrentIndex(m_iIndex);
        m_pWidget->UpdateActions();
    }
    
    void undo() override
    {
        m_pModel->MoveStateForward(m_iIndex);
        m_iIndex += 1;
        m_pComboBox->setCurrentIndex(m_iIndex);
        m_pWidget->UpdateActions();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename WIDGET, typename MODEL>
class UndoCmd_MoveStateForward : public QUndoCommand
{
    WIDGET *            m_pWidget;
    MODEL *             m_pModel;
    QComboBox *         m_pComboBox;
    int                 m_iIndex;

public:
    UndoCmd_MoveStateForward(QString sText, WIDGET *pWidget, MODEL *pModel, QComboBox *pCmb, QUndoCommand *pParent = 0) :   QUndoCommand(pParent),
                                                                                                                            m_pWidget(pWidget),
                                                                                                                            m_pModel(pModel),
                                                                                                                            m_pComboBox(pCmb),
                                                                                                                            m_iIndex(m_pComboBox->currentIndex())
    {
        setText(sText);
    }
    
    virtual ~UndoCmd_MoveStateForward()
    { }

    void redo() override
    {
        m_pModel->MoveStateForward(m_iIndex);
        m_iIndex += 1;
        m_pComboBox->setCurrentIndex(m_iIndex);
        m_pWidget->UpdateActions();
    }
    
    void undo() override
    {
        m_pModel->MoveStateBack(m_iIndex);
        m_iIndex -= 1;
        m_pComboBox->setCurrentIndex(m_iIndex);
        m_pWidget->UpdateActions();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename OWNER>
class UndoCmd_ComboBox : public QUndoCommand
{
    OWNER *             m_pOwner;

    QComboBox *         m_pComboBox;
    int                 m_iPrevIndex;
    int                 m_iNewIndex;
    
public:
    UndoCmd_ComboBox(QString sText, OWNER *pOwner, QComboBox *pCmb, int iPrevIndex, int iNewIndex, QUndoCommand *pParent = 0) :   QUndoCommand(pParent),
                                                                                                                                        m_pOwner(pOwner),
                                                                                                                                        m_pComboBox(pCmb),
                                                                                                                                        m_iPrevIndex(iPrevIndex),
                                                                                                                                        m_iNewIndex(iNewIndex)
    {
        setText(sText);
    }
            
    virtual ~UndoCmd_ComboBox()
    { }

    void redo() override
    {
        m_pComboBox->blockSignals(true);
        m_pComboBox->setCurrentIndex(m_iNewIndex);
        m_pComboBox->blockSignals(false);
        
        m_pOwner->UpdateActions();
    }
    
    void undo() override
    {
        m_pComboBox->blockSignals(true);
        m_pComboBox->setCurrentIndex(m_iPrevIndex);
        m_pComboBox->blockSignals(false);
        
        m_pOwner->UpdateActions();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename OWNER>
class UndoCmd_CheckBox : public QUndoCommand
{
    OWNER *             m_pOwner;
    QCheckBox *         m_pCheckBox;
    bool                m_bInitialValue;

public:
    UndoCmd_CheckBox(OWNER *pOwner, QCheckBox *pCheckBox, QUndoCommand *pParent = 0) :    QUndoCommand(pParent),
                                                                                                m_pOwner(pOwner),
                                                                                                m_pCheckBox(pCheckBox)
    {
        m_bInitialValue = m_pCheckBox->isChecked();
        setText((m_bInitialValue ? "Checked " : "Unchecked ") % m_pCheckBox->text());
    }
    
    virtual ~UndoCmd_CheckBox()
    { }

    void redo() override
    {
        m_pCheckBox->setChecked(m_bInitialValue);
        m_pOwner->UpdateActions();
    }
    
    void undo() override
    {
        m_pCheckBox->setChecked(!m_bInitialValue);
        m_pOwner->UpdateActions();
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
template<typename WIDGET>
class UndoCmd_AddFrames : public QUndoCommand
{
    WIDGET *                                m_pWidget;
    QList<AtlasFrame *>                     m_Frames;
    
public:
    UndoCmd_AddFrames(QString sText, WIDGET *pWidget, QUndoCommand *pParent = 0) :  QUndoCommand(pParent),
                                                                                    m_pWidget(pWidget)
    {
        setText(sText);
        m_Frames.clear();
    }
    
    virtual ~UndoCmd_AddFrames()
    { }

    void redo() override
    {
        m_Frames = m_pWidget->GetItem()->GetProject()->GetAtlasesData().RequestFrames(m_pWidget->GetItem(), m_Frames);
        m_pWidget->UpdateActions();
    }
    
    void undo() override
    {
        m_pWidget->GetItem()->GetProject()->GetAtlasesData().RelinquishFrames(m_pWidget->GetItem(), m_Frames);
        m_pWidget->UpdateActions();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename OWNER>
class UndoCmd_DeleteFrame : public QUndoCommand
{
    OWNER *                                 m_pOwner;
    QList<AtlasFrame *>                     m_Frames;

public:
    UndoCmd_DeleteFrame(QString sText, OWNER *pOwner, AtlasFrame *pFrame, QUndoCommand *pParent = 0) :    QUndoCommand(pParent),
                                                                                                                m_pOwner(pOwner)
    {
        setText(sText);
        m_Frames.append(pFrame);
    }
    
    virtual ~UndoCmd_DeleteFrame()
    { }

    void redo() override
    {
        m_pOwner->GetItem()->GetProject()->GetAtlasesData().RelinquishFrames(m_pOwner->GetItem(), m_Frames);
        m_pOwner->UpdateActions();
    }
    
    void undo() override
    {
        m_Frames = m_pOwner->GetItem()->GetProject()->GetAtlasesData().RequestFrames(m_pOwner->GetItem(), m_Frames);
        m_pOwner->UpdateActions();
    }
};

#endif // UNDOCMDS
