/**************************************************************************
*	PropertiesTreeView.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "PropertiesTreeView.h"
#include "PropertiesTreeModel.h"
#include "WgtVectorSpinBox.h"
#include "ProjectItemData.h"
#include "IModel.h"
#include "SpriteModels.h"
#include "PropertiesUndoCmd.h"
#include "Project.h"
#include "DlgColorPicker.h"
#include "DlgSetUiPanel.h"

#include <QPainter>
#include <QHeaderView>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QMouseEvent>

PropertiesTreeView::PropertiesTreeView(QWidget *pParent /*= nullptr*/) :
	QTreeView(pParent)
{
	setAnimated(true);
	setAutoFillBackground(true);
}

PropertiesTreeView::~PropertiesTreeView()
{
}

/*virtual*/ void PropertiesTreeView::setModel(QAbstractItemModel *pModel) /*override*/
{
	QTreeView::setModel(pModel);
	setItemDelegate(new PropertiesDelegate(this, this));

	PropertiesTreeModel *pPropModel = static_cast<PropertiesTreeModel *>(model());
	if(pPropModel == nullptr)
		return;

	const int iNUM_CATEGORIES = pPropModel->GetNumCategories();
	for(int i = 0; i < iNUM_CATEGORIES; ++i)
	{
		if(pPropModel->IsCategoryEnabled(i))
			expand(pPropModel->GetCategoryModelIndex(i));
		else
			collapse(pPropModel->GetCategoryModelIndex(i));
	}
}

/*virtual*/ void PropertiesTreeView::paintEvent(QPaintEvent *pEvent) /*override*/
{
	QTreeView::paintEvent(pEvent);

	QPainter painter(viewport());
	for(int i = 0; i < header()->count(); ++i)
	{
		// Draw only visible sections starting from second column
		if(header()->isSectionHidden(i) || header()->visualIndex(i) <= 0)
			continue;

		// Position mapped to viewport
		int iColumnStartPos = header()->sectionViewportPosition(i) - 1;
		if(iColumnStartPos > 0)
			painter.drawLine(QPoint(iColumnStartPos, 0), QPoint(iColumnStartPos, height()));
	}
}

/*virtual*/ void PropertiesTreeView::mousePressEvent(QMouseEvent *pEvent) /*override*/
{
	if(pEvent->button() == Qt::LeftButton)
	{
		QModelIndex index = indexAt(pEvent->pos());
		if(index.column() == PROPERTIESCOLUMN_Value && (model()->flags(index) & Qt::ItemIsEnabled))
		{
			setCurrentIndex(index);
			edit(index);
		}
	}

	QTreeView::mousePressEvent(pEvent);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertiesDelegate::PropertiesDelegate(PropertiesTreeView *pTableView, QObject *pParent /*= 0*/) :
	QStyledItemDelegate(pParent),
	m_pTableView(pTableView)
{
}

/*virtual*/ QWidget *PropertiesDelegate::createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const /*override*/
{
	QWidget *pReturnWidget = nullptr;

	PropertiesTreeModel *pPropertiesTreeModel = static_cast<PropertiesTreeModel *>(m_pTableView->model());
	PropertiesDef propDefRef = pPropertiesTreeModel->GetIndexDefinition(index);
	switch(propDefRef.eType)
	{
	case PROPERTIESTYPE_bool:
		// Handled natively within tree model's CheckStateRole
		break;

	case PROPERTIESTYPE_int:
	case PROPERTIESTYPE_int64:
		pReturnWidget = new QSpinBox(pParent);

		if(propDefRef.defaultData.isValid())
			static_cast<QSpinBox *>(pReturnWidget)->setValue(propDefRef.defaultData.toLongLong());
		if(propDefRef.minRange.isValid())
			static_cast<QSpinBox *>(pReturnWidget)->setMinimum(propDefRef.minRange.toLongLong());
		if(propDefRef.maxRange.isValid())
			static_cast<QSpinBox *>(pReturnWidget)->setMaximum(propDefRef.maxRange.toLongLong());
		if(propDefRef.stepAmt.isValid())
			static_cast<QSpinBox *>(pReturnWidget)->setSingleStep(propDefRef.stepAmt.toLongLong());
		if(propDefRef.sPrefix.isEmpty() == false)
			static_cast<QSpinBox *>(pReturnWidget)->setPrefix(propDefRef.sPrefix);
		if(propDefRef.sSuffix.isEmpty() == false)
			static_cast<QSpinBox *>(pReturnWidget)->setSuffix(propDefRef.sSuffix);
		break;

	case PROPERTIESTYPE_double:
		pReturnWidget = new QDoubleSpinBox(pParent);

		if(propDefRef.defaultData.isValid())
			static_cast<QDoubleSpinBox *>(pReturnWidget)->setValue(propDefRef.defaultData.toDouble());
		if(propDefRef.minRange.isValid())
			static_cast<QDoubleSpinBox *>(pReturnWidget)->setMinimum(propDefRef.minRange.toDouble());
		if(propDefRef.maxRange.isValid())
			static_cast<QDoubleSpinBox *>(pReturnWidget)->setMaximum(propDefRef.maxRange.toDouble());
		if(propDefRef.stepAmt.isValid())
			static_cast<QDoubleSpinBox *>(pReturnWidget)->setSingleStep(propDefRef.stepAmt.toDouble());
		if(propDefRef.sPrefix.isEmpty() == false)
			static_cast<QDoubleSpinBox *>(pReturnWidget)->setPrefix(propDefRef.sPrefix);
		if(propDefRef.sSuffix.isEmpty() == false)
			static_cast<QDoubleSpinBox *>(pReturnWidget)->setSuffix(propDefRef.sSuffix);
		if(propDefRef.delegateBuilder.isValid())
			static_cast<QDoubleSpinBox *>(pReturnWidget)->setDecimals(propDefRef.delegateBuilder.toInt());
		break;

	case PROPERTIESTYPE_ivec2:
		pReturnWidget = new WgtVectorSpinBox(SPINBOXTYPE_Int2d, propDefRef.minRange, propDefRef.maxRange, pParent);
		if(propDefRef.defaultData.isValid())
			static_cast<WgtVectorSpinBox *>(pReturnWidget)->SetValue(propDefRef.defaultData);
		break;

	case PROPERTIESTYPE_vec2:
		pReturnWidget = new WgtVectorSpinBox(SPINBOXTYPE_Double2d, propDefRef.minRange, propDefRef.maxRange, pParent);
		if(propDefRef.defaultData.isValid())
			static_cast<WgtVectorSpinBox *>(pReturnWidget)->SetValue(propDefRef.defaultData);
		break;

	case PROPERTIESTYPE_ivec3:
		pReturnWidget = new WgtVectorSpinBox(SPINBOXTYPE_Int3d, propDefRef.minRange, propDefRef.maxRange, pParent);
		if(propDefRef.defaultData.isValid())
			static_cast<WgtVectorSpinBox *>(pReturnWidget)->SetValue(propDefRef.defaultData);
		break;

	case PROPERTIESTYPE_vec3:
		pReturnWidget = new WgtVectorSpinBox(SPINBOXTYPE_Double3d, propDefRef.minRange, propDefRef.maxRange, pParent);
		if(propDefRef.defaultData.isValid())
			static_cast<WgtVectorSpinBox *>(pReturnWidget)->SetValue(propDefRef.defaultData);
		break;

	case PROPERTIESTYPE_ivec4:
		pReturnWidget = new WgtVectorSpinBox(SPINBOXTYPE_Int4d, propDefRef.minRange, propDefRef.maxRange, pParent);
		if(propDefRef.defaultData.isValid())
			static_cast<WgtVectorSpinBox *>(pReturnWidget)->SetValue(propDefRef.defaultData);
		break;

	case PROPERTIESTYPE_vec4:
		pReturnWidget = new WgtVectorSpinBox(SPINBOXTYPE_Double4d, propDefRef.minRange, propDefRef.maxRange, pParent);
		if(propDefRef.defaultData.isValid())
			static_cast<WgtVectorSpinBox *>(pReturnWidget)->SetValue(propDefRef.defaultData);
		break;

	case PROPERTIESTYPE_LineEdit:
		pReturnWidget = new QLineEdit(pParent);

		if(propDefRef.defaultData.isValid())
			static_cast<QLineEdit *>(pReturnWidget)->setText(propDefRef.defaultData.toString());
		break;

	case PROPERTIESTYPE_ComboBoxString:
		pReturnWidget = new QComboBox(pParent);
		connect(static_cast<QComboBox *>(pReturnWidget), SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBoxEditorSubmit(int)));

		if(propDefRef.delegateBuilder.isValid())
			static_cast<QComboBox *>(pReturnWidget)->addItems(propDefRef.delegateBuilder.toStringList());
		if(propDefRef.defaultData.isValid())
			static_cast<QComboBox *>(pReturnWidget)->setCurrentIndex(propDefRef.delegateBuilder.toStringList().indexOf(propDefRef.defaultData.toString()));
		break;

	case PROPERTIESTYPE_ComboBoxInt:
		pReturnWidget = new QComboBox(pParent);
		connect(static_cast<QComboBox *>(pReturnWidget), SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBoxEditorSubmit(int)));

		if(propDefRef.delegateBuilder.isValid())
			static_cast<QComboBox *>(pReturnWidget)->addItems(propDefRef.delegateBuilder.toStringList());
		if(propDefRef.defaultData.isValid())
			static_cast<QComboBox *>(pReturnWidget)->setCurrentIndex(propDefRef.defaultData.toInt());
		break;

	case PROPERTIESTYPE_ComboBoxItems: {
		pReturnWidget = new QComboBox(pParent);
		connect(static_cast<QComboBox *>(pReturnWidget), SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBoxEditorSubmit(int)));

		ItemType eItemType = static_cast<ItemType>(propDefRef.delegateBuilder.toInt());
		PropertiesTreeModel *pModel = static_cast<PropertiesTreeModel *>(m_pTableView->model());
		const QMap<QUuid, TreeModelItemData *> &projItemMapRef = pModel->GetOwner().GetProject().GetItemMap();
		QList<ProjectItemData *> validItemList;
		for(auto iter = projItemMapRef.keyValueBegin(); iter != projItemMapRef.keyValueEnd(); ++iter)
		{
			if(iter->second->GetType() == eItemType && iter->second->IsProjectItem())
				validItemList.append(static_cast<ProjectItemData *>(iter->second));
		}
		std::sort(validItemList.begin(), validItemList.end(),
			[](const ProjectItemData *pOne, const ProjectItemData *pTwo)
			{
				return pOne->GetName(true) < pTwo->GetName(true);
			});

		static_cast<QComboBox *>(pReturnWidget)->clear();
		for(ProjectItemData *pItem : validItemList)
			static_cast<QComboBox *>(pReturnWidget)->addItem(pItem->GetIcon(SUBICON_None), pItem->GetName(true), pItem->GetUuid());
		break; }

	case PROPERTIESTYPE_StatesComboBox: {
		pReturnWidget = new QComboBox(pParent);
		connect(static_cast<QComboBox *>(pReturnWidget), SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBoxEditorSubmit(int)));

		PropertiesTreeModel *pModel = static_cast<PropertiesTreeModel *>(m_pTableView->model());
		ProjectItemData *pProjItem = static_cast<ProjectItemData *>(pModel->GetOwner().GetProject().FindItemData(propDefRef.delegateBuilder.toUuid()));
		if(pProjItem)
			static_cast<QComboBox *>(pReturnWidget)->setModel(pProjItem->GetModel());
		if(propDefRef.defaultData.isValid())
			static_cast<QComboBox *>(pReturnWidget)->setCurrentIndex(propDefRef.defaultData.toInt());
		break; }

	case PROPERTIESTYPE_Slider:
		pReturnWidget = new QSlider(Qt::Horizontal, pParent);

		if(propDefRef.defaultData.isValid())
			static_cast<QSlider *>(pReturnWidget)->setValue(propDefRef.defaultData.toInt());
		if(propDefRef.minRange.isValid())
			static_cast<QSlider *>(pReturnWidget)->setMinimum(propDefRef.minRange.toInt());
		if(propDefRef.maxRange.isValid())
			static_cast<QSlider *>(pReturnWidget)->setMaximum(propDefRef.maxRange.toInt());
		if(propDefRef.stepAmt.isValid())
			static_cast<QSlider *>(pReturnWidget)->setSingleStep(propDefRef.stepAmt.toInt());
		break;

	case PROPERTIESTYPE_Color: {
		const QVariant &origValue = pPropertiesTreeModel->GetIndexValue(index);

		QColor topColor, botColor;
		botColor = topColor = QColor(origValue.toRect().left(), origValue.toRect().top(), origValue.toRect().width());
		DlgColorPicker *pDlg = new DlgColorPicker("Choose Color", topColor, botColor, pParent);
		if(pDlg->exec() == QDialog::Accepted)
		{
			QColor newColor;
			if(pDlg->IsSolidColor())
				newColor = pDlg->GetSolidColor();
			else
				newColor = pDlg->GetVgTopColor(); // NOTE: Only getting top color!!

			QVariant newValue = QRect(newColor.red(), newColor.green(), newColor.blue(), newColor.alpha());
			if(origValue != newValue)
			{
				QUndoCommand *pUndoCmd = pPropertiesTreeModel->AllocateUndoCmd(index, newValue);
				pPropertiesTreeModel->GetOwner().GetUndoStack()->push(pUndoCmd);
			}
		}
		break; }

	case PROPERTIESTYPE_SpriteFrames: {
		pReturnWidget = new QSlider(Qt::Horizontal, pParent);
		static_cast<QSlider *>(pReturnWidget)->setSingleStep(1);

		PropertiesTreeModel *pModel = static_cast<PropertiesTreeModel *>(m_pTableView->model());
		ProjectItemData *pProjItem = static_cast<ProjectItemData *>(pModel->GetOwner().GetProject().FindItemData(propDefRef.delegateBuilder.toUuid()));
		if(pProjItem)
		{
			int iCurrentState = pModel->FindPropertyValue("Common", "State").toInt();
			static_cast<QSlider *>(pReturnWidget)->setMinimum(0);
			static_cast<QSlider *>(pReturnWidget)->setMaximum(static_cast<SpriteStateData *>(pProjItem->GetModel()->GetStateData(iCurrentState))->GetFramesModel()->rowCount());
		}
		if(propDefRef.defaultData.isValid())
			static_cast<QSlider *>(pReturnWidget)->setValue(propDefRef.defaultData.toInt());
		break; }

	case PROPERTIESTYPE_UiPanel: {
		const QVariant &origValue = pPropertiesTreeModel->GetIndexValue(index);
		QJsonObject panelObj = origValue.toJsonObject();

		HyUiPanelInit init;
		init.m_eNodeType = HyGlobal::ConvertItemType(HyGlobal::GetTypeFromString(panelObj["nodeType"].toString()));
		init.m_uiWidth = panelObj["width"].toInt();
		init.m_uiHeight = panelObj["height"].toInt();
		init.m_NodePath.Clear();
		init.m_uiFrameSize = panelObj["frameSize"].toInt();
		init.m_PanelColor = panelObj["panelColor"].toVariant().toUInt();
		init.m_FrameColor = panelObj["frameColor"].toVariant().toUInt();
		init.m_TertiaryColor = panelObj["tertiaryColor"].toVariant().toUInt();

		Project &projectRef = static_cast<PropertiesTreeModel *>(m_pTableView->model())->GetOwner().GetProject();
		DlgSetUiPanel *pDlg = new DlgSetUiPanel(projectRef, "Setup Widget Panel", init, QUuid(panelObj["nodeUuid"].toString()), pParent);
		if(pDlg->exec() == QDialog::Accepted)
		{
			QVariant newValue = pDlg->GetSerializedPanelInit();
			if(origValue != newValue)
			{
				QUndoCommand *pUndoCmd = pPropertiesTreeModel->AllocateUndoCmd(index, newValue);
				pPropertiesTreeModel->GetOwner().GetUndoStack()->push(pUndoCmd);
			}
		}
		break; }

	default:
		HyGuiLog("PropertiesDelegate::createEditor not implemented for type: " % QString::number(propDefRef.eType), LOGTYPE_Error);
		break;
	}

	if(pReturnWidget)
		pReturnWidget->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
	return pReturnWidget;
}

/*virtual*/ void PropertiesDelegate::setEditorData(QWidget *pEditor, const QModelIndex &index) const /*override*/
{
	const QVariant &propValue = static_cast<PropertiesTreeModel *>(m_pTableView->model())->GetIndexValue(index);
	PropertiesDef propDefRef = static_cast<PropertiesTreeModel *>(m_pTableView->model())->GetIndexDefinition(index);

	if(propValue.toJsonObject().isEmpty() == false)
	{
		HyGuiLog("PropertiesDelegate::setEditorData not implemented for procedural value", LOGTYPE_Error);
	}
	else
	{
		switch(propDefRef.eType)
		{
		case PROPERTIESTYPE_bool:
			// Handled natively within tree model's CheckStateRole
			break;
		case PROPERTIESTYPE_int:
		case PROPERTIESTYPE_int64:
			static_cast<QSpinBox *>(pEditor)->setValue(propValue.toLongLong());
			break;
		case PROPERTIESTYPE_double:
			static_cast<QDoubleSpinBox *>(pEditor)->setValue(propValue.toDouble());
			break;
		case PROPERTIESTYPE_ivec2:
		case PROPERTIESTYPE_vec2:
		case PROPERTIESTYPE_ivec3:
		case PROPERTIESTYPE_vec3:
		case PROPERTIESTYPE_ivec4:
		case PROPERTIESTYPE_vec4:
			static_cast<WgtVectorSpinBox *>(pEditor)->SetValue(propValue);
			break;
		case PROPERTIESTYPE_LineEdit:
			static_cast<QLineEdit *>(pEditor)->setText(propValue.toString());
			break;
		case PROPERTIESTYPE_ComboBoxString:
			static_cast<QComboBox *>(pEditor)->setCurrentIndex(propDefRef.delegateBuilder.toStringList().indexOf(propValue.toString()));
			break;
		case PROPERTIESTYPE_ComboBoxInt:
		case PROPERTIESTYPE_StatesComboBox:
			static_cast<QComboBox *>(pEditor)->setCurrentIndex(propValue.toInt());
			break;
		case PROPERTIESTYPE_ComboBoxItems:
			static_cast<QComboBox *>(pEditor)->setCurrentIndex(static_cast<QComboBox *>(pEditor)->findData(propValue.value<QUuid>()));
			break;

		case PROPERTIESTYPE_Slider:
		case PROPERTIESTYPE_SpriteFrames:
			static_cast<QSlider *>(pEditor)->setValue(propValue.toInt());
			break;

		default:
			HyGuiLog("PropertiesDelegate::setEditorData() Unsupported Delegate type:" % QString::number(propDefRef.eType), LOGTYPE_Error);
		}
	}
}

/*virtual*/ void PropertiesDelegate::setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const /*override*/
{
	PropertiesTreeModel *pPropertiesTreeModel = static_cast<PropertiesTreeModel *>(pModel);

	QUndoCommand *pUndoCmd = nullptr;

	PropertiesDef propDefRef = pPropertiesTreeModel->GetIndexDefinition(index);
	QVariant newValue;
	switch(propDefRef.eType)
	{
	case PROPERTIESTYPE_bool:
		break;
	case PROPERTIESTYPE_int:
	case PROPERTIESTYPE_int64:
		newValue = QVariant(static_cast<QSpinBox *>(pEditor)->value());
		break;
	case PROPERTIESTYPE_double:
		newValue = QVariant(static_cast<QDoubleSpinBox *>(pEditor)->value());
		break;
	case PROPERTIESTYPE_ivec2:
	case PROPERTIESTYPE_vec2:
	case PROPERTIESTYPE_ivec3:
	case PROPERTIESTYPE_vec3:
	case PROPERTIESTYPE_ivec4:
	case PROPERTIESTYPE_vec4:
		newValue = QVariant(static_cast<WgtVectorSpinBox *>(pEditor)->GetValue());
		break;
	case PROPERTIESTYPE_LineEdit:
		newValue = QVariant(static_cast<QLineEdit *>(pEditor)->text());
		break;
	case PROPERTIESTYPE_ComboBoxString:
		newValue = QVariant(propDefRef.delegateBuilder.toStringList()[static_cast<QComboBox *>(pEditor)->currentIndex()]);
		break;
	case PROPERTIESTYPE_ComboBoxInt:
	case PROPERTIESTYPE_StatesComboBox:
		newValue = QVariant(static_cast<QComboBox *>(pEditor)->currentIndex());
		break;
	case PROPERTIESTYPE_ComboBoxItems:
		newValue = static_cast<QComboBox *>(pEditor)->currentData();
		break;
	case PROPERTIESTYPE_Slider:
	case PROPERTIESTYPE_SpriteFrames:
		newValue = QVariant(static_cast<QSlider *>(pEditor)->value());
		break;
	case PROPERTIESTYPE_Color: // Handled in DlgColorPicker
		break;
	default:
		HyGuiLog("PropertiesDelegate::setModelData() Unsupported Delegate type:" % QString::number(propDefRef.eType), LOGTYPE_Error);
	}

	const QVariant &origValue = pPropertiesTreeModel->GetIndexValue(index);
	if(origValue != newValue)
	{
		pUndoCmd = pPropertiesTreeModel->AllocateUndoCmd(index, newValue);
		pPropertiesTreeModel->GetOwner().GetUndoStack()->push(pUndoCmd);
	}

	// Select the name column after editing to force the editor to close
	m_pTableView->selectionModel()->setCurrentIndex(m_pTableView->model()->index(index.row(), PROPERTIESCOLUMN_Name), QItemSelectionModel::Select);
}

/*virtual*/ void PropertiesDelegate::updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const /*override*/
{
	pEditor->setGeometry(option.rect);
}

void PropertiesDelegate::OnComboBoxEditorSubmit(int iIndex)
{
	Q_EMIT commitData(static_cast<QWidget *>(sender()));
}
