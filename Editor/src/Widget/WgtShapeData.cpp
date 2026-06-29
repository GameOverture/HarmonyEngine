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

	EditModeModel *pEditModeModel = static_cast<EditModeModel *>(m_pTreeItemData->GetEditModel());

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

void WgtShapeData::UpdateModel(bool bIsActiveEditModeItem, EditModeType eEditModeType, EditorShape eShapeType)
{
	bool bValueChanged = false;

	EditModeModel *pEditModeModel = static_cast<EditModeModel *>(m_pTreeItemData->GetEditModel());

	if(pEditModeModel->GetEditModeType() != eEditModeType)
	{
		switch(eEditModeType)
		{
		case EDITMODETYPE_PrimitiveShape:
			pEditModeModel->ChangeToShape(bIsActiveEditModeItem, eShapeType, false);
			break;
		case EDITMODETYPE_FixtureShape:
			pEditModeModel->ChangeToShape(bIsActiveEditModeItem, eShapeType, true);
			break;
		case EDITMODETYPE_PrimitiveLineChain:
			pEditModeModel->ChangeToLineChain(bIsActiveEditModeItem, false);
			break;
		case EDITMODETYPE_FixtureChain:
			pEditModeModel->ChangeToLineChain(bIsActiveEditModeItem, true);
			break;
		case EDITMODETYPE_FixturePoint:
			pEditModeModel->ChangeToPoint(bIsActiveEditModeItem);
			break;
		}
		bValueChanged = true;
	}

	if(bValueChanged)
		Q_EMIT ValueChanged(GetValue());
}

QVariant WgtShapeData::GetValue()
{
	EditModeModel *pEditModeModel = static_cast<EditModeModel *>(m_pTreeItemData->GetEditModel());
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

	EditModeType eEditModeType = EDITMODETYPE_Invalid;
	if(sType == HYLINECHAIN_Name)
	{
		if(static_cast<EditModeModel *>(m_pTreeItemData->GetEditModel())->IsFixture())
			eEditModeType = EDITMODETYPE_FixtureChain;
		else
			eEditModeType = EDITMODETYPE_PrimitiveLineChain;
	}
	else if(sType == HYPOINT_Name)
		eEditModeType = EDITMODETYPE_FixturePoint;
	else
	{
		if(static_cast<EditModeModel *>(m_pTreeItemData->GetEditModel())->IsFixture())
			eEditModeType = EDITMODETYPE_FixtureShape;
		else
			eEditModeType = EDITMODETYPE_PrimitiveShape;
	}

	UpdateModel(bIsActiveEditModeItem, eEditModeType, HyGlobal::GetShapeFromString(sType));
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

	EditModeType eEditModeType = EDITMODETYPE_Invalid;
	EditorShape eShapeType = SHAPE_None;
	if(ui->cmbPrimType->currentIndex() >= NUM_SHAPES)
		eEditModeType = EDITMODETYPE_PrimitiveLineChain;
	else
	{
		eEditModeType = EDITMODETYPE_PrimitiveShape;
		eShapeType = static_cast<EditorShape>(ui->cmbPrimType->currentIndex());
	}

	UpdateModel(true, eEditModeType, eShapeType);
}

void WgtShapeData::on_sbPrimOutline_valueChanged(double dValue)
{
	static_cast<EditModeModel *>(m_pTreeItemData->GetEditModel())->SetOutline(ui->sbPrimOutline->value());
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

	UpdateModel(true, EDITMODETYPE_FixtureShape, eShapeType);
}
