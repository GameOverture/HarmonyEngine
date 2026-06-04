/**************************************************************************
*	WgtShapeData.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "WgtShapeData.h"
#include "ui_WgtShapeData.h"
#include "EditModeModel.h"
#include "EntityWidget.h"

WgtShapeData::WgtShapeData(QWidget *parent) :
	QWidget(parent),
	m_pTreeItemData(nullptr),
	ui(new Ui::WgtShapeData)
{
	ui->setupUi(this);
	HyGuiLog("WgtShapeData default constructor used - this is only for use when creating the widget in a .ui file.", LOGTYPE_Error);
	// NOTE: WHEN THIS CONSTRUCTOR IS USED. Init() MUST BE CALLED
}

WgtShapeData::WgtShapeData(EntityTreeItemData *pTreeItemData, QWidget *parent /*= nullptr*/) :
	QWidget(parent),
	m_pTreeItemData(pTreeItemData),
	ui(new Ui::WgtShapeData)
{
	ui->setupUi(this);
	Init(pTreeItemData);
}

/*virtual*/ WgtShapeData::~WgtShapeData()
{
	delete ui;
}

void WgtShapeData::Init(EntityTreeItemData *pTreeItemData)
{
	m_pTreeItemData = pTreeItemData;

	EditModeModel *pEditModeModel = m_pTreeItemData->GetEditModel();

	ShapeDataType eShapeDataType;
	if(pEditModeModel->IsFixture() == false)
		eShapeDataType = SHAPEDATATYPE_Primitive;
	else if(pEditModeModel->IsLineChain())
		eShapeDataType = SHAPEDATATYPE_Chain;
	else
		eShapeDataType = SHAPEDATATYPE_Shape;

	ui->stackedWidget->setCurrentIndex(eShapeDataType);

	ui->cmbPrimType->clear();
	QStringList sComboBoxList = HyGlobal::GetShapeNameList();
	switch(eShapeDataType)
	{
	case SHAPEDATATYPE_Primitive:
		sComboBoxList.append(HYLINECHAIN_Name);
		ui->cmbPrimType->blockSignals(true);
		ui->cmbPrimType->addItems(sComboBoxList);
		ui->cmbPrimType->blockSignals(false);
		if(pEditModeModel->IsLineChain())
			ui->cmbPrimType->setCurrentIndex(sComboBoxList.size() - 1);
		else
			ui->cmbPrimType->setCurrentIndex(pEditModeModel->GetShapeType());
		ui->sbPrimOutline->setValue(pEditModeModel->GetOutline());
		break;

	case SHAPEDATATYPE_Shape:
		ui->cmbShapeType->blockSignals(true);
		ui->cmbShapeType->addItems(sComboBoxList);
		ui->cmbShapeType->blockSignals(false);
		ui->cmbShapeType->setCurrentIndex(pEditModeModel->GetShapeType());
		break;

	case SHAPEDATATYPE_Chain:
		break;

	default:
		HyGuiLog("Unknown WgtShapeData type: " % QString::number(eShapeDataType), LOGTYPE_Error);
		break;
	}
}

void WgtShapeData::UpdateModel(bool bIsActiveEditModeItem, bool bIsFixture, bool bIsLineChain, EditorShape eShapeType)
{
	bool bValueChanged = false;

	EditModeModel *pEditModeModel = m_pTreeItemData->GetEditModel();

	if(pEditModeModel->IsFixture() != bIsFixture)
	{
		pEditModeModel->SetIsFixture(bIsFixture);
		bValueChanged = true;
	}
	if(bIsLineChain && pEditModeModel->IsLineChain() == false)
	{
		pEditModeModel->ChangeToLineChain(bIsActiveEditModeItem);
		bValueChanged = true;
	}
	else if(pEditModeModel->GetShapeType() != eShapeType)
	{
		pEditModeModel->ChangeToShape(bIsActiveEditModeItem, eShapeType);
		bValueChanged = true;
	}

	if(bValueChanged)
		Q_EMIT ValueChanged(GetValue());
}

QVariant WgtShapeData::GetValue()
{
	EditModeModel *pEditModeModel = m_pTreeItemData->GetEditModel();
	QJsonObject serializedObj = pEditModeModel->Serialize();
	//switch(ui->stackedWidget->currentIndex())
	//{
	//case SHAPEDATATYPE_Primitive:
	//	serializedObj.insert("type", ui->cmbPrimType->currentText());
	//	serializedObj.insert("outline", ui->sbPrimOutline->value());
	//	break;
	//	
	//case SHAPEDATATYPE_Shape:
	//	serializedObj.insert("type", ui->cmbShapeType->currentText());
	//	serializedObj.insert("outline", 1.0f);
	//	break;

	//case SHAPEDATATYPE_Chain:
	//	serializedObj.insert("type", HYLINECHAIN_Name);
	//	serializedObj.insert("outline", 1.0f);
	//	break;

	//default:
	//	HyGuiLog("Unknown WgtShapeData type: " % QString::number(ui->stackedWidget->currentIndex()), LOGTYPE_Error);
	//	return QVariant();
	//}

	//serializedObj.insert("data", m_DataArray);
	return serializedObj;
}

void WgtShapeData::SetValue(bool bIsActiveEditModeItem, QVariant data)
{
	QJsonObject serializedObj = data.toJsonObject();

	QString sType = serializedObj["type"].toString();
	UpdateModel(bIsActiveEditModeItem, m_pTreeItemData->GetEditModel()->IsFixture(), sType == HYLINECHAIN_Name, HyGlobal::GetShapeFromString(sType));
}

void WgtShapeData::OnValueChanged(int iValue)
{
	QVariant data = GetValue();
	Q_EMIT ValueChanged(data);
}

void WgtShapeData::OnValueChanged(double dValue)
{
	QVariant data = GetValue();
	Q_EMIT ValueChanged(data);
}

void WgtShapeData::on_cmbPrimType_currentIndexChanged(int iIndex)
{
	EntityWidget *pEntWidget = static_cast<EntityWidget *>(m_pTreeItemData->GetEntityModel().GetItem().GetWidget());
	if(pEntWidget == nullptr)
	{
		HyGuiLog("WgtShapeData::on_cmbPrimType_currentIndexChanged - EntityWidget is null", LOGTYPE_Error);
		return;
	}
	pEntWidget->SetEditMode(m_pTreeItemData);

	bool bIsLineChain = false;
	EditorShape eShapeType = SHAPE_None;
	if(ui->cmbPrimType->currentIndex() >= NUM_SHAPES)
		bIsLineChain = true;
	else
		eShapeType = static_cast<EditorShape>(ui->cmbPrimType->currentIndex());

	UpdateModel(true, m_pTreeItemData->GetEditModel()->IsFixture(), bIsLineChain, eShapeType);
}

void WgtShapeData::on_sbPrimOutline_valueChanged(double dValue)
{
	m_pTreeItemData->GetEditModel()->SetOutline(ui->sbPrimOutline->value());
}

void WgtShapeData::on_cmbShapeType_currentIndexChanged(int iIndex)
{
	EntityWidget *pEntWidget = static_cast<EntityWidget *>(m_pTreeItemData->GetEntityModel().GetItem().GetWidget());
	if(pEntWidget == nullptr)
	{
		HyGuiLog("WgtShapeData::on_cmbPrimType_currentIndexChanged - EntityWidget is null", LOGTYPE_Error);
		return;
	}
	pEntWidget->SetEditMode(m_pTreeItemData);

	EditorShape eShapeType = static_cast<EditorShape>(ui->cmbShapeType->currentIndex());

	UpdateModel(true, m_pTreeItemData->GetEditModel()->IsFixture(), false, eShapeType);
}
