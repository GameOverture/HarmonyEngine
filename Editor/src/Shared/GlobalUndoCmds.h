/**************************************************************************
 *	GlobalUndoCmds.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef UNDOCMDS
#define UNDOCMDS

#include "Global.h"
#include "ProjectItemData.h"
#include "Project.h"
#include "IModel.h"
#include "GlobalWidgetMappers.h"
#include "AtlasModel.h"

#include <QUndoCommand>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QRadioButton>

 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//class UndoCmd_CameraUpdate : public QUndoCommand
//{
//	ProjectItemData &	m_ItemRef;
//
//	glm::vec2			m_ptOldCamPos;
//	float				m_fOldCamZoom;
//
//	glm::vec2			m_ptNewCamPos;
//	float				m_fNewCamZoom;
//
//public:
//	UndoCmd_CameraUpdate(QString sText, ProjectItemData &itemRef, glm::vec2 ptOldCamPos, float fOldCamZoom, glm::vec2 ptNewCamPos, float fNewCamZoom, QUndoCommand *pParent = nullptr) :
//		QUndoCommand(pParent),
//		m_ItemRef(itemRef),
//		m_ptOldCamPos(ptOldCamPos),
//		m_fOldCamZoom(fOldCamZoom),
//		m_ptNewCamPos(ptNewCamPos),
//		m_fNewCamZoom(fNewCamZoom)
//	{
//		setText(sText);
//	}
//
//	virtual ~UndoCmd_CameraUpdate()
//	{ }
//
//	void redo() override
//	{
//		m_ItemRef.GetDraw()->SetCamera(m_ptNewCamPos, m_fNewCamZoom);
//	}
//
//	void undo() override
//	{
//		m_ItemRef.GetDraw()->SetCamera(m_ptOldCamPos, m_fOldCamZoom);
//	}
//};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename STATEDATA>
class UndoCmd_AddState : public QUndoCommand
{
	ProjectItemData &	m_ItemRef;
	int					m_iIndex;
	int					m_iCopyFromStateIndex;

public:
	UndoCmd_AddState(QString sText, ProjectItemData &itemRef, int iCopyFromStateIndex, QUndoCommand *pParent = nullptr) :
		QUndoCommand(pParent),
		m_ItemRef(itemRef),
		m_iCopyFromStateIndex(iCopyFromStateIndex)
	{
		setText(sText);
	}
	
	virtual ~UndoCmd_AddState()
	{ }

	void redo() override
	{
		FileDataPair fileDataPair;
		if(m_iCopyFromStateIndex >= 0)
		{
			fileDataPair = m_ItemRef.GetModel()->GetStateFileData(m_iCopyFromStateIndex);
			fileDataPair.m_Meta.insert("name", QJsonValue(fileDataPair.m_Meta["name"].toString() + " (Copy)"));
		}

		m_iIndex = m_ItemRef.GetModel()->AppendState<STATEDATA>(fileDataPair);
		m_ItemRef.FocusWidgetState(m_iIndex, -1);
	}
	
	void undo() override
	{
		m_ItemRef.GetModel()->PopState(m_iIndex);
		m_ItemRef.FocusWidgetState(m_iIndex - 1, -1);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename STATEDATA>
class UndoCmd_RemoveState : public QUndoCommand
{
	ProjectItemData &       m_ItemRef;
	int                 m_iIndex;
	FileDataPair        m_PoppedState;

public:
	UndoCmd_RemoveState(QString sText, ProjectItemData &itemRef, int iIndex, QUndoCommand *pParent = nullptr) :
		QUndoCommand(pParent),
		m_ItemRef(itemRef),
		m_iIndex(iIndex)
	{
		setText(sText);
	}

	virtual ~UndoCmd_RemoveState()
	{ }

	void redo() override
	{
		m_PoppedState = m_ItemRef.GetModel()->PopState(m_iIndex);
		m_ItemRef.FocusWidgetState(0, -1);
	}
	
	void undo() override
	{
		m_ItemRef.GetModel()->InsertState<STATEDATA>(m_iIndex, m_PoppedState);
		m_ItemRef.FocusWidgetState(m_iIndex, -1);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class UndoCmd_RenameState : public QUndoCommand
{
	ProjectItemData &       m_ItemRef;
	int                 m_iIndex;

	QString             m_sNewName;
	QString             m_sOldName;

public:
	UndoCmd_RenameState(QString sText, ProjectItemData &itemRef, QString sNewName, int iIndex, QUndoCommand *pParent = nullptr) :
		QUndoCommand(pParent),
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
		m_sOldName = m_ItemRef.GetModel()->SetStateName(m_iIndex, m_sNewName);
		m_ItemRef.FocusWidgetState(m_iIndex, -1);
	}
	
	void undo() override
	{
		m_ItemRef.GetModel()->SetStateName(m_iIndex, m_sOldName);
		m_ItemRef.FocusWidgetState(m_iIndex, -1);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class UndoCmd_MoveStateBack : public QUndoCommand
{
	ProjectItemData &       m_ItemRef;
	int                 m_iStateIndex;

public:
	UndoCmd_MoveStateBack(QString sText, ProjectItemData &itemRef, int iStateIndex, QUndoCommand *pParent = nullptr) :
		QUndoCommand(pParent),
		m_ItemRef(itemRef),
		m_iStateIndex(iStateIndex)
	{
		setText(sText);
	}
	
	virtual ~UndoCmd_MoveStateBack()
	{ }

	void redo() override
	{
		m_ItemRef.GetModel()->MoveStateBack(m_iStateIndex);
		m_iStateIndex -= 1;

		m_ItemRef.FocusWidgetState(m_iStateIndex, -1);
	}
	
	void undo() override
	{
		m_ItemRef.GetModel()->MoveStateForward(m_iStateIndex);
		m_iStateIndex += 1;

		m_ItemRef.FocusWidgetState(m_iStateIndex, -1);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class UndoCmd_MoveStateForward : public QUndoCommand
{
	ProjectItemData &       m_ItemRef;
	int                 m_iStateIndex;

public:
	UndoCmd_MoveStateForward(QString sText, ProjectItemData &itemRef, int iStateIndex, QUndoCommand *pParent = nullptr) :
		QUndoCommand(pParent),
		m_ItemRef(itemRef),
		m_iStateIndex(iStateIndex)
	{
		setText(sText);
	}
	
	virtual ~UndoCmd_MoveStateForward()
	{ }

	void redo() override
	{
		m_ItemRef.GetModel()->MoveStateForward(m_iStateIndex);
		m_iStateIndex += 1;

		m_ItemRef.FocusWidgetState(m_iStateIndex, -1);
	}
	
	void undo() override
	{
		m_ItemRef.GetModel()->MoveStateBack(m_iStateIndex);
		m_iStateIndex -= 1;

		m_ItemRef.FocusWidgetState(m_iStateIndex, -1);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class UndoCmd_AddGuide : public QUndoCommand
{
	IDraw &				m_DrawRef;
	HyOrientation		m_eOrientation;
	int					m_iNewPos;
	bool				m_bNewGuidePlaced;

public:
	UndoCmd_AddGuide(IDraw &drawRef, HyOrientation eOrientation, int iPos, QUndoCommand *pParent = nullptr) :
		QUndoCommand(pParent),
		m_DrawRef(drawRef),
		m_eOrientation(eOrientation),
		m_iNewPos(iPos),
		m_bNewGuidePlaced(false)
	{
		setText(QString("New ") % (m_eOrientation == HYORIENT_Horizontal ? "Horizontal" : "Vertical") % " Guide at " % QString::number(m_iNewPos));
	}

	virtual ~UndoCmd_AddGuide()
	{ }

	void redo() override
	{
		m_bNewGuidePlaced = m_DrawRef.AllocateGuide(m_eOrientation, m_iNewPos);
	}

	void undo() override
	{
		if(m_bNewGuidePlaced)
			m_DrawRef.DeleteGuide(m_eOrientation, m_iNewPos);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class UndoCmd_RemoveGuide : public QUndoCommand
{
	IDraw &				m_DrawRef;
	HyOrientation		m_eOrientation;
	int					m_iOldPos;

public:
	UndoCmd_RemoveGuide(IDraw &drawRef, HyOrientation eOrientation, int iOldPos, QUndoCommand *pParent = nullptr) :
		QUndoCommand(pParent),
		m_DrawRef(drawRef),
		m_eOrientation(eOrientation),
		m_iOldPos(iOldPos)
	{
		setText(QString("Remove ") % (m_eOrientation == HYORIENT_Horizontal ? "Horizontal" : "Vertical") % " Guide from " % QString::number(m_iOldPos));
	}

	virtual ~UndoCmd_RemoveGuide()
	{ }

	void redo() override
	{
		m_DrawRef.DeleteGuide(m_eOrientation, m_iOldPos);
	}

	void undo() override
	{
		m_DrawRef.AllocateGuide(m_eOrientation, m_iOldPos);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class UndoCmd_MoveGuide : public QUndoCommand
{
	IDraw &m_DrawRef;
	HyOrientation		m_eOrientation;
	int					m_iNewPos;
	int					m_iOldPos;
	bool				m_bNewGuidePlaced;

public:
	UndoCmd_MoveGuide(IDraw &drawRef, HyOrientation eOrientation, int iOldPos, int iNewPos, QUndoCommand *pParent = nullptr) :
		QUndoCommand(pParent),
		m_DrawRef(drawRef),
		m_eOrientation(eOrientation),
		m_iOldPos(iOldPos),
		m_iNewPos(iNewPos),
		m_bNewGuidePlaced(false)
	{
		setText(QString("Move ") % (m_eOrientation == HYORIENT_Horizontal ? "Horizontal" : "Vertical") % " Guide to " % QString::number(m_iNewPos));
	}

	virtual ~UndoCmd_MoveGuide()
	{ }

	void redo() override
	{
		m_DrawRef.DeleteGuide(m_eOrientation, m_iOldPos);
		m_bNewGuidePlaced = m_DrawRef.AllocateGuide(m_eOrientation, m_iNewPos);
	}

	void undo() override
	{
		if(m_bNewGuidePlaced)
			m_DrawRef.DeleteGuide(m_eOrientation, m_iNewPos);
		m_DrawRef.AllocateGuide(m_eOrientation, m_iOldPos);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class UndoCmd_ComboBox : public QUndoCommand
{
	ProjectItemData &       m_ItemRef;
	ComboBoxMapper *    m_pMapper;
	int                 m_iStateIndex;

	int                 m_iPrevValue;
	int                 m_iNewValue;
	
public:
	UndoCmd_ComboBox(QString sText, ProjectItemData &itemRef, ComboBoxMapper *pMapper, int iStateIndex, int iPrevValue, int iNewValue, QUndoCommand *pParent = nullptr) :
		QUndoCommand(pParent),
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
		m_ItemRef.FocusWidgetState(m_iStateIndex, -1);
	}
	
	void undo() override
	{
		m_pMapper->SetIndex(m_iPrevValue);
		m_ItemRef.FocusWidgetState(m_iStateIndex, -1);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class UndoCmd_CheckBox : public QUndoCommand
{
	ProjectItemData &       m_ItemRef;
	CheckBoxMapper *    m_pMapper;
	int                 m_iStateIndex;

	bool                m_bNewValue;

public:
	UndoCmd_CheckBox(QString sText, ProjectItemData &itemRef, CheckBoxMapper *pMapper, int iStateIndex, QUndoCommand *pParent = 0) :    QUndoCommand(pParent),
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
		m_ItemRef.FocusWidgetState(m_iStateIndex, -1);
	}
	
	void undo() override
	{
		m_pMapper->SetChecked(!m_bNewValue);
		m_ItemRef.FocusWidgetState(m_iStateIndex, -1);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class UndoCmd_SpinBox : public QUndoCommand
{
	ProjectItemData &       m_ItemRef;
	SpinBoxMapper *     m_pMapper;
	int                 m_iStateIndex;

	int                 m_iNew;
	int                 m_iOld;

public:
	UndoCmd_SpinBox(QString sText, ProjectItemData &itemRef, SpinBoxMapper *pMapper, int iStateIndex, int iNew, int iOld, QUndoCommand *pParent = 0) :  QUndoCommand(pParent),
																																					m_ItemRef(itemRef),
																																					m_pMapper(pMapper),
																																					m_iStateIndex(iStateIndex),
																																					m_iNew(iNew),
																																					m_iOld(iOld)
	{
		setText(sText);
	}
	
	virtual ~UndoCmd_SpinBox()
	{ }

	void redo() override
	{
		m_pMapper->SetValue(m_iNew);
		m_ItemRef.FocusWidgetState(m_iStateIndex, -1);
	}
	
	void undo() override
	{
		m_pMapper->SetValue(m_iOld);
		m_ItemRef.FocusWidgetState(m_iStateIndex, -1);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class UndoCmd_DoubleSpinBox : public QUndoCommand
{
	ProjectItemData &           m_ItemRef;
	DoubleSpinBoxMapper *   m_pMapper;
	int                     m_iStateIndex;

	double                  m_dNew;
	double                  m_dOld;

public:
	UndoCmd_DoubleSpinBox(QString sText, ProjectItemData &itemRef, DoubleSpinBoxMapper *pMapper, int iStateIndex, double dNew, double dOld, QUndoCommand *pParent = 0) :    QUndoCommand(pParent),
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
		m_ItemRef.FocusWidgetState(m_iStateIndex, -1);
	}
	
	void undo() override
	{
		m_pMapper->SetValue(m_dOld);
		m_ItemRef.FocusWidgetState(m_iStateIndex, -1);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class UndoCmd_LineEdit : public QUndoCommand
{
	ProjectItemData &       m_ItemRef;
	LineEditMapper *    m_pMapper;
	int                 m_iStateIndex;

	QString             m_sNew;
	QString             m_sOld;

public:
	UndoCmd_LineEdit(QString sText, ProjectItemData &itemRef, LineEditMapper *pMapper, int iStateIndex, QString sNew, QString sOld, QUndoCommand *pParent = 0) :    QUndoCommand(pParent),
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
		m_ItemRef.FocusWidgetState(m_iStateIndex, -1);
	}
	
	void undo() override
	{
		m_pMapper->SetString(m_sOld);
		m_ItemRef.FocusWidgetState(m_iStateIndex, -1);
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
	UndoCmd_RadioToggle(QString sText, OWNER *pOwner, QRadioButton *pNewRadBtnToggle, QRadioButton *pPrevRadBtnToggle, QUndoCommand *pParent = 0) :
		QUndoCommand(pParent),
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
		
		m_pOwner->UpdateActions(); // TODO: remove this, it's called elsewhere
	}
	
	void undo() override
	{
		m_pNewRadBtnToggle->blockSignals(true);
		m_pPrevRadBtnToggle->blockSignals(true);
		
		m_pPrevRadBtnToggle->setChecked(true);
		
		m_pNewRadBtnToggle->blockSignals(false);
		m_pPrevRadBtnToggle->blockSignals(false);
		
		m_pOwner->UpdateActions(); // TODO: remove this, it's called elsewhere 
	}
};

#endif // UNDOCMDS
