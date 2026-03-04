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

WgtShapeData::WgtShapeData(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::WgtShapeData)
{
	ui->setupUi(this);
	// NOTE: WHEN THIS CONSTRUCTOR IS USED. Init() MUST BE CALLED
}

WgtShapeData::WgtShapeData(ShapeDataType eShapeDataType, QJsonObject serializedObj, QWidget *parent /*= nullptr*/) :
	QWidget(parent),
	ui(new Ui::WgtShapeData)
{
	ui->setupUi(this);
	Init(eShapeDataType, serializedObj);
}

/*virtual*/ WgtShapeData::~WgtShapeData()
{
	delete ui;
}

void WgtShapeData::Init(ShapeDataType eShapeDataType, QJsonObject serializedObj)
{
	ui->stackedWidget->setCurrentIndex(eShapeDataType);
	m_DataArray = serializedObj["data"].toArray();

	QStringList sComboBoxList = HyGlobal::GetShapeNameList();
	
	switch(eShapeDataType)
	{
	case SHAPEDATATYPE_Primitive:
		sComboBoxList.append("Line Chain");
		ui->cmbPrimType->addItems(sComboBoxList);
		if(serializedObj["type"].toString() == "Line Chain")
			ui->cmbPrimType->setCurrentIndex(sComboBoxList.size() - 1);
		else
			ui->cmbPrimType->setCurrentIndex(HyGlobal::GetShapeFromString(serializedObj["type"].toString()));
		ui->sbPrimOutline->setValue(serializedObj["outline"].toDouble());
		break;

	case SHAPEDATATYPE_Shape:
		ui->cmbShapeType->addItems(sComboBoxList);
		ui->cmbShapeType->setCurrentIndex(HyGlobal::GetShapeFromString(serializedObj["type"].toString()));
		break;

	case SHAPEDATATYPE_Chain:
		break;

	default:
		HyGuiLog("Unknown WgtShapeData type: " % QString::number(eShapeDataType), LOGTYPE_Error);
		break;
	}
}

QVariant WgtShapeData::GetValue()
{
	QJsonObject serializedObj;
	switch(ui->stackedWidget->currentIndex())
	{
	case SHAPEDATATYPE_Primitive:
		serializedObj.insert("type", ui->cmbPrimType->currentText());
		serializedObj.insert("outline", ui->sbPrimOutline->value());
		break;
		
	case SHAPEDATATYPE_Shape:
		serializedObj.insert("type", ui->cmbShapeType->currentText());
		serializedObj.insert("outline", 1.0f);
		break;

	case SHAPEDATATYPE_Chain:
		serializedObj.insert("type", "Line Chain");
		serializedObj.insert("outline", 1.0f);
		break;

	default:
		HyGuiLog("Unknown WgtShapeData type: " % QString::number(ui->stackedWidget->currentIndex()), LOGTYPE_Error);
		return QVariant();
	}

	serializedObj.insert("data", m_DataArray);
	return serializedObj;
}

void WgtShapeData::SetValue(QVariant data)
{
	QJsonObject serializedObj = data.toJsonObject();
	Init(static_cast<ShapeDataType>(ui->stackedWidget->currentIndex()), serializedObj);

	Q_EMIT ValueChanged(data);
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
