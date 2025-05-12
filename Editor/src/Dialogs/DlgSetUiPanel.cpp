/**************************************************************************
 *	DlgSetUiPanel.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "DlgSetUiPanel.h"
#include "ui_DlgSetUiPanel.h"
#include "Project.h"
//#include "TreeModelItemData.h"

DlgSetUiPanel::DlgSetUiPanel(Project &projectRef, QString sTitle, HyUiPanelInit &init, QUuid selectedNodeUuid, QWidget *parent /*= nullptr*/) :
	QDialog(parent),
	m_ProjectRef(projectRef),
	m_Init(init),
	m_SelectedNodeUuid(selectedNodeUuid),
	ui(new Ui::DlgSetUiPanel)
{
	ui->setupUi(this);
	setWindowTitle(sTitle);


	ui->vsbBoundingVolumeSize->Init(SPINBOXTYPE_Double2d, static_cast<float>(-MAX_INT_RANGE), static_cast<float>(MAX_INT_RANGE));
	connect(ui->vsbBoundingVolumeSize, SIGNAL(ValueChanged(QVariant)), this, SLOT(OnSizeChanged(QVariant)));
	ui->vsbPrimSize->Init(SPINBOXTYPE_Double2d, static_cast<float>(-MAX_INT_RANGE), static_cast<float>(MAX_INT_RANGE));
	connect(ui->vsbPrimSize, SIGNAL(ValueChanged(QVariant)), this, SLOT(OnSizeChanged(QVariant)));
	ui->vsbNodeSize->Init(SPINBOXTYPE_Double2d, static_cast<float>(-MAX_INT_RANGE), static_cast<float>(MAX_INT_RANGE));
	connect(ui->vsbNodeSize, SIGNAL(ValueChanged(QVariant)), this, SLOT(OnSizeChanged(QVariant)));

	if(m_Init.m_eNodeType == HYTYPE_Unknown)
		m_Init.m_eNodeType = HYTYPE_Primitive;

	SyncNodeComboBox();
	SyncWidgets();
}

DlgSetUiPanel::~DlgSetUiPanel()
{
	delete ui;
}

QVariant DlgSetUiPanel::GetSerializedPanelInit() const
{
	QJsonObject serializedObj;
	serializedObj.insert("nodeType", HyGlobal::ItemName(HyGlobal::ConvertHyType(m_Init.m_eNodeType), false));
	serializedObj.insert("width", QJsonValue(static_cast<qint64>(m_Init.m_uiWidth)));
	serializedObj.insert("height", QJsonValue(static_cast<qint64>(m_Init.m_uiHeight)));
	serializedObj.insert("nodeUuid", m_SelectedNodeUuid.toString(QUuid::WithoutBraces));
	serializedObj.insert("frameSize", QJsonValue(static_cast<qint64>(m_Init.m_uiFrameSize)));
	serializedObj.insert("panelColor", QJsonValue(static_cast<qint64>(m_Init.m_PanelColor.GetAsRGBA())));
	serializedObj.insert("frameColor", QJsonValue(static_cast<qint64>(m_Init.m_FrameColor.GetAsRGBA())));
	serializedObj.insert("tertiaryColor", QJsonValue(static_cast<qint64>(m_Init.m_TertiaryColor.GetAsRGBA())));

	return QVariant(serializedObj);
}

void DlgSetUiPanel::SyncNodeComboBox()
{
	const QMap<QUuid, TreeModelItemData *> &projItemMapRef = m_ProjectRef.GetItemMap();

	QList<ProjectItemData *> validItemList;
	for(auto iter = projItemMapRef.keyValueBegin(); iter != projItemMapRef.keyValueEnd(); ++iter)
	{
		if(iter->second->IsProjectItem() && (iter->second->GetType() == ITEM_Sprite || iter->second->GetType() == ITEM_Spine))
			validItemList.append(static_cast<ProjectItemData *>(iter->second));
	}
	if(validItemList.empty() == false)
	{
		std::sort(validItemList.begin(), validItemList.end(),
			[](const ProjectItemData *pOne, const ProjectItemData *pTwo)
			{
				if(pOne->GetType() == ITEM_Sprite && pTwo->GetType() != ITEM_Sprite)
					return true;
				else if(pTwo->GetType() == ITEM_Sprite && pOne->GetType() != ITEM_Sprite)
					return false;
				else
					return pOne->GetName(true) < pTwo->GetName(true);
			});
	}

	ui->cmbNode->clear();
	for(ProjectItemData *pItem : validItemList)
		ui->cmbNode->addItem(pItem->GetIcon(SUBICON_None), pItem->GetName(true), pItem->GetUuid());
}

void DlgSetUiPanel::SyncWidgets()
{
	if(m_Init.m_eNodeType == HYTYPE_Entity)
		ui->stackedWidget->setCurrentIndex(PAGE_BoundingVolume);
	else if(m_Init.m_eNodeType == HYTYPE_Primitive)
		ui->stackedWidget->setCurrentIndex(PAGE_Primitive);
	else
		ui->stackedWidget->setCurrentIndex(PAGE_Node);

	QVariant size = QPointF(m_Init.m_uiWidth, m_Init.m_uiHeight);

	if(ui->grpBoundingVolumeSetSize->isChecked())
		ui->vsbBoundingVolumeSize->SetValue(size);
	else
		ui->vsbBoundingVolumeSize->SetValue(QPointF(0.0f, 0.0f));

	ui->vsbPrimSize->SetValue(size);
	ui->sbPrimFrame->setValue(static_cast<int>(m_Init.m_uiFrameSize));
	ui->primPanelColor->setStyleSheet(QString("background-color: rgb(") % QString::number(m_Init.m_PanelColor.GetRed()) % ", " % QString::number(m_Init.m_PanelColor.GetGreen()) % ", " % QString::number(m_Init.m_PanelColor.GetBlue()) % ");");
	ui->primFrameColor->setStyleSheet(QString("background-color: rgb(") % QString::number(m_Init.m_FrameColor.GetRed()) % ", " % QString::number(m_Init.m_FrameColor.GetGreen()) % ", " % QString::number(m_Init.m_FrameColor.GetBlue()) % ");");
	ui->primTertiaryColor->setStyleSheet(QString("background-color: rgb(") % QString::number(m_Init.m_TertiaryColor.GetRed()) % ", " % QString::number(m_Init.m_TertiaryColor.GetGreen()) % ", " % QString::number(m_Init.m_TertiaryColor.GetBlue()) % ");");

	if(ui->grpNodeSetSize->isChecked())
		ui->vsbNodeSize->SetValue(size);
	else
		ui->vsbNodeSize->SetValue(QPointF(0.0f, 0.0f));

	ui->cmbNode->setCurrentIndex(ui->cmbNode->findData(m_SelectedNodeUuid));
}

void DlgSetUiPanel::on_radBoundingVolume_toggled(bool bChecked)
{
	if(bChecked)
	{
		m_Init.m_eNodeType = HYTYPE_Entity;
		ui->stackedWidget->setCurrentIndex(PAGE_BoundingVolume);
	}
}

void DlgSetUiPanel::on_radPrimitive_toggled(bool bChecked)
{
	if(bChecked)
	{
		m_Init.m_eNodeType = HYTYPE_Primitive;
		ui->stackedWidget->setCurrentIndex(PAGE_Primitive);
	}
}

void DlgSetUiPanel::on_radNode_toggled(bool bChecked)
{
	if(bChecked)
	{
		m_Init.m_eNodeType = HYTYPE_Unknown;

		const QMap<QUuid, TreeModelItemData *> &projItemMapRef = m_ProjectRef.GetItemMap();
		if(projItemMapRef.contains(ui->cmbNode->currentData().value<QUuid>()))
			m_Init.m_eNodeType = HyGlobal::ConvertItemType(projItemMapRef[ui->cmbNode->currentData().value<QUuid>()]->GetType());
		else
			HyGuiLog("No valid node items found", LOGTYPE_Warning);

		ui->stackedWidget->setCurrentIndex(PAGE_Node);
	}
}

void DlgSetUiPanel::OnSizeChanged(QVariant newSize)
{
	m_Init.m_uiWidth = newSize.toPointF().x();
	m_Init.m_uiHeight = newSize.toPointF().y();
}

void DlgSetUiPanel::on_sbPrimFrame_valueChanged(int arg1)
{
	m_Init.m_uiFrameSize = ui->sbPrimFrame->value();
}

void DlgSetUiPanel::on_primPanelColor_clicked()
{
}
