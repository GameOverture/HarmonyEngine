/**************************************************************************
*	PropertiesTreeModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "PropertiesTreeModel.h"
#include "PropertiesUndoCmd.h"
#include "IModel.h"
#include "WidgetVectorSpinBox.h"
#include "SpriteModels.h"

#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>

PropertiesTreeModel::PropertiesTreeModel(ProjectItem &ownerRef, int iStateIndex, QVariant subState, QObject *pParent /*= nullptr*/) :
	ITreeModel({ "Property", "Value" }, pParent),
	m_OwnerRef(ownerRef),
	m_iSTATE_INDEX(iStateIndex),
	m_iSUBSTATE(subState)
{
}

/*virtual*/ PropertiesTreeModel::~PropertiesTreeModel()
{
}

ProjectItem &PropertiesTreeModel::GetOwner()
{
	return m_OwnerRef;
}

const PropertiesDef &PropertiesTreeModel::GetPropertyDefinition(const QModelIndex &index) const
{
	TreeModelItem *pTreeItem = GetItem(index);
	return m_PropertyDefMap[pTreeItem];
}

const QVariant &PropertiesTreeModel::GetPropertyValue(const QModelIndex &index) const
{
	TreeModelItem *pTreeItem = GetItem(index);
	return pTreeItem->data(COLUMN_Value);
}

//QVariant PropertiesTreeModel::GetValue(QString sUniquePropertyName) const
//{
//	for(int i = 0; i < m_CategoryList.size(); ++i)
//	{
//		for(int j = 0; j < m_CategoryList[i]->GetNumChildren(); ++j)
//		{
//			if(0 == static_cast<PropertiesTreeItem *>(m_CategoryList[i]->GetChild(j))->GetName().compare(sUniquePropertyName, Qt::CaseSensitive))
//				return static_cast<PropertiesTreeItem *>(m_CategoryList[i]->GetChild(j))->GetData();
//		}
//	}
//
//	return QVariant();
//}

bool PropertiesTreeModel::AppendCategory(QString sName, QColor color, QVariant commonDelegateBuilder /*= QVariant()*/, bool bCheckable /*= false*/, bool bStartChecked /*= false*/, QString sToolTip /*= ""*/)
{
	// All category names must be unique
	for(int i = 0; i < m_pRootItem->childCount(); ++i)
	{
		if(0 == m_pRootItem->child(i)->data(COLUMN_Name).toString().compare(sName, Qt::CaseSensitive))
			return false;
	}

	// Create the row inside the model
	QModelIndex rootParentIndex = createIndex(m_pRootItem->childNumber(), 0, m_pRootItem);
	if(insertRow(m_pRootItem->childCount(), rootParentIndex) == false)
	{
		HyGuiLog("PropertiesTreeModel::AppendCategory() - insertRow failed", LOGTYPE_Error);
		return false;
	}

	// Set data in the property's name column
	TreeModelItem *pNewlyAddedTreeItem = m_pRootItem->child(m_pRootItem->childCount() - 1);
	if(setData(index(pNewlyAddedTreeItem->childNumber(), COLUMN_Name, rootParentIndex), QVariant(sName)) == false)
		HyGuiLog("PropertiesTreeModel::AppendCategory() - setData failed", LOGTYPE_Error);

	// Set data in the property's value column
	if(setData(index(pNewlyAddedTreeItem->childNumber(), COLUMN_Value, rootParentIndex), QVariant(bStartChecked ? Qt::Checked : Qt::Unchecked)) == false)
		HyGuiLog("PropertiesTreeModel::AppendCategory() - setData failed", LOGTYPE_Error);

	// Link this property definition to the proper TreeModelItem using 'm_PropertyDefMap'
	PropertiesDef def;
	def.eType = bCheckable ? PROPERTIESTYPE_CategoryChecked : PROPERTIESTYPE_Category;
	def.bReadOnly = !bCheckable;
	def.delegateBuilder = commonDelegateBuilder;
	def.color = color;
	def.sToolTip = sToolTip;

	m_PropertyDefMap[pNewlyAddedTreeItem] = def;

	return true;
}

bool PropertiesTreeModel::AppendProperty(QString sCategoryName,
										 QString sName,
										 QColor color,
										 PropertiesType eType,
										 QVariant delegateBuilder /*= QVariant()*/,
										 bool bReadOnly /*= false*/,
										 QString sToolTip /*= QString()*/,
										 QVariant defaultData /*= QVariant()*/,
										 QVariant minRange /*= QVariant()*/,
										 QVariant maxRange /*= QVariant()*/,
										 QVariant stepAmt /*= QVariant()*/,
										 QString sPrefix /*= QString()*/,
										 QString sSuffix /*= QString()*/)
{
	// Find category to add property to
	TreeModelItem *pCategoryTreeItem = nullptr;
	for(int i = 0; i < m_pRootItem->childCount(); ++i)
	{
		if(0 == m_pRootItem->child(i)->data(COLUMN_Name).toString().compare(sCategoryName, Qt::CaseSensitive))
		{
			pCategoryTreeItem = m_pRootItem->child(i);
			break;
		}
	}
	if(pCategoryTreeItem == nullptr)
	{
		HyGuiLog("PropertiesTreeModel::AppendProperty() - Could not find category: " % sCategoryName, LOGTYPE_Error);
		return false;
	}

	// Now ensure that no property with this name already exists
	for(int i = 0; i < pCategoryTreeItem->childCount(); ++i)
	{
		if(0 == pCategoryTreeItem->child(i)->data(COLUMN_Name).toString().compare(sName, Qt::CaseSensitive))
		{
			HyGuiLog("PropertiesTreeModel::AppendProperty() - Property with the name: " % sName % " already exists", LOGTYPE_Error);
			return false;
		}
	}

	// Create the row inside the model
	QModelIndex categoryParentIndex = createIndex(pCategoryTreeItem->childNumber(), 0, pCategoryTreeItem);
	if(insertRow(pCategoryTreeItem->childCount(), categoryParentIndex) == false)
	{
		HyGuiLog("PropertiesTreeModel::AppendCategory() - insertRow failed", LOGTYPE_Error);
		return false;
	}

	// Set data in the property's name column
	TreeModelItem *pNewlyAddedTreeItem = pCategoryTreeItem->child(pCategoryTreeItem->childCount() - 1);
	if(setData(index(pNewlyAddedTreeItem->childNumber(), COLUMN_Name, categoryParentIndex), QVariant(sName)) == false)
		HyGuiLog("PropertiesTreeModel::AppendProperty() - setData failed", LOGTYPE_Error);

	// Set data in the property's value column
	if(setData(index(pNewlyAddedTreeItem->childNumber(), COLUMN_Value, categoryParentIndex), defaultData) == false)
		HyGuiLog("PropertiesTreeModel::AppendProperty() - setData failed", LOGTYPE_Error);

	// Link this property definition to the proper TreeModelItem using 'm_PropertyDefMap'
	PropertiesDef def;
	def.eType = eType;
	def.bReadOnly = bReadOnly;
	def.delegateBuilder = delegateBuilder;
	def.color = color;
	def.sToolTip = sToolTip;
	def.minRange = minRange;
	def.maxRange = maxRange;
	def.stepAmt = stepAmt;
	def.sPrefix = sPrefix;
	def.sSuffix = sSuffix;

	m_PropertyDefMap[pNewlyAddedTreeItem] = def;

	return true;
}

//void PropertiesTreeModel::RefreshProperties()
//{
//	for(int i = 0; i < m_CategoryList.size(); ++i)
//	{
//		dataChanged(createIndex(0, 0, m_CategoryList[i]->GetChild(0)),
//					createIndex(m_CategoryList[i]->GetNumChildren() - 1, 1, m_CategoryList[i]->GetChild(m_CategoryList[i]->GetNumChildren() - 1)));
//	}
//}

/*virtual*/ QVariant PropertiesTreeModel::data(const QModelIndex &index, int iRole) const /*override*/
{
	if(index.isValid() == false)
		return QVariant();

	if(iRole == Qt::UserRole)
		return ITreeModel::data(index, iRole);

	TreeModelItem *pTreeItem = GetItem(index);
	const PropertiesDef &propDefRef = m_PropertyDefMap[pTreeItem];

	switch(iRole)
	{
	case Qt::DisplayRole:
		if(index.column() == COLUMN_Name)
			return pTreeItem->data(COLUMN_Name);
		else
			return ConvertValueToString(pTreeItem);

	case Qt::TextAlignmentRole:
		if(propDefRef.IsCategory())
			return Qt::AlignHCenter;

	case Qt::ToolTipRole:
		return propDefRef.sToolTip;

	case Qt::BackgroundRole:
		if(propDefRef.IsCategory())
			return QBrush(QColor::fromRgb(160, 160, 160));
		else
			return QBrush((0 == (pTreeItem->childNumber() & 1)) ? propDefRef.color : propDefRef.color.lighter());

	case Qt::ForegroundRole:
		if(propDefRef.IsCategory())
			return QBrush(QColor::fromRgb(255, 255, 255));

	case Qt::FontRole:
		if(propDefRef.IsCategory())
		{
			QFont font;
			font.setBold(true);
			return font;
		}

	case Qt::CheckStateRole:
		if((index.column() == 0 && propDefRef.eType == PROPERTIESTYPE_CategoryChecked) ||
		   (index.column() == 1 && propDefRef.eType == PROPERTIESTYPE_bool))
		{
			return pTreeItem->data(COLUMN_Value);
		}
	}

	return QVariant();
}

/*virtual*/ Qt::ItemFlags PropertiesTreeModel::flags(const QModelIndex &index) const /*override*/
{
	Qt::ItemFlags returnFlags = Qt::NoItemFlags;

	if(index.isValid() == false)
		return returnFlags;

	TreeModelItem *pTreeItem = GetItem(index);
	const PropertiesDef &propDefRef = m_PropertyDefMap[pTreeItem];

	if(propDefRef.IsCategory())
	{
		if(propDefRef.bReadOnly == false)
			returnFlags |= Qt::ItemIsEnabled;

		if(propDefRef.eType == PROPERTIESTYPE_CategoryChecked && index.column() == COLUMN_Name)
			returnFlags |= Qt::ItemIsUserCheckable;
	}
	else
	{
		TreeModelItem *pCategoryTreeItem = pTreeItem->parent();
		const PropertiesDef &categoryPropDefRef = m_PropertyDefMap[pCategoryTreeItem];

		if(categoryPropDefRef.IsCategory() == false)
			HyGuiLog("PropertiesTreeModel::flags() passed in index is not a category and its parent is not one either", LOGTYPE_Error);

		if(categoryPropDefRef.eType == PROPERTIESTYPE_Category ||
		   (categoryPropDefRef.eType == PROPERTIESTYPE_CategoryChecked && pCategoryTreeItem->data(COLUMN_Value).toInt() == Qt::Checked))
		{
			if(propDefRef.bReadOnly == false)
				returnFlags |= Qt::ItemIsEnabled;
		}

		if(index.column() == COLUMN_Value)
		{
			if(propDefRef.bReadOnly == false)
				returnFlags |= (Qt::ItemIsSelectable | Qt::ItemIsEditable);

			if(propDefRef.eType == PROPERTIESTYPE_bool)
				returnFlags |= Qt::ItemIsUserCheckable;
		}
	}

	return returnFlags;
}

/*virtual*/ bool PropertiesTreeModel::setData(const QModelIndex &index, const QVariant &value, int iRole) /*override*/
{
	QVariant oldData = data(index,
	if(ITreeModel::setData(index, value, iRole))
	{
		TreeModelItem *pTreeItem = static_cast<TreeModelItem*>(index.internalPointer());

		QUndoCommand *pCmd = new PropertiesUndoCmd(*this, m_iSTATE_INDEX, m_iSUBSTATE, *pTreeItem, index, value, iRole);
		m_OwnerRef.GetUndoStack()->push(pCmd);

		return true;
	}

	return false;
}

QString PropertiesTreeModel::ConvertValueToString(TreeModelItem *pTreeItem) const
{
	const PropertiesDef &propDefRef = m_PropertyDefMap[pTreeItem];
	const QVariant &treeItemValue = pTreeItem->data(COLUMN_Value);

	QString sRetStr = propDefRef.sPrefix;

	switch(propDefRef.eType)
	{
	case PROPERTIESTYPE_bool:
		sRetStr += treeItemValue.toBool() ? "True" : "False";
		break;
	case PROPERTIESTYPE_int:
	case PROPERTIESTYPE_Slider:
	case PROPERTIESTYPE_SpriteFrames:
		sRetStr += QString::number(treeItemValue.toInt());
		break;
	case PROPERTIESTYPE_double:
		sRetStr += QString::number(treeItemValue.toDouble());
		break;
	case PROPERTIESTYPE_ivec2: {
		QPoint pt = treeItemValue.toPoint();
		sRetStr += QString::number(pt.x()) % " x " % QString::number(pt.y());
	} break;
	case PROPERTIESTYPE_vec2: {
		QPointF pt = treeItemValue.toPointF();
		sRetStr += QString::number(pt.x()) % " x " % QString::number(pt.y());
	} break;
	case PROPERTIESTYPE_ivec3: {
		QRect rect = treeItemValue.toRect();
		sRetStr += "{ " % QString::number(rect.left()) % ", " % QString::number(rect.top()) % ", " % QString::number(rect.width()) % " }";
	} break;
	case PROPERTIESTYPE_vec3: {
		QRectF rect = treeItemValue.toRectF();
		sRetStr += "{ " % QString::number(rect.left()) % ", " % QString::number(rect.top()) % ", " % QString::number(rect.width()) % " }";
	} break;
	case PROPERTIESTYPE_ivec4: {
		QRect rect = treeItemValue.toRect();
		sRetStr += "{ " % QString::number(rect.left()) % ", " % QString::number(rect.top()) % ", " % QString::number(rect.width()) % ", " % QString::number(rect.height()) % " }";
	} break;
	case PROPERTIESTYPE_vec4: {
		QRectF rect = treeItemValue.toRectF();
		sRetStr += "{ " % QString::number(rect.left()) % ", " % QString::number(rect.top()) % ", " % QString::number(rect.width()) % ", " % QString::number(rect.height()) % " }";
	} break;
	case PROPERTIESTYPE_LineEdit:
		sRetStr += treeItemValue.toString();
		break;
	case PROPERTIESTYPE_ComboBox:
		sRetStr += propDefRef.delegateBuilder.toStringList()[treeItemValue.toInt()];
		break;
	case PROPERTIESTYPE_StatesComboBox: {
		QComboBox tmpComboBox(nullptr);
		tmpComboBox.setModel(propDefRef.delegateBuilder.value<ProjectItem *>()->GetModel());
		sRetStr += tmpComboBox.itemText(treeItemValue.toInt());
	} break;

	case PROPERTIESTYPE_Root:
	case PROPERTIESTYPE_Category:
	case PROPERTIESTYPE_CategoryChecked:
		break;

	default:
		HyGuiLog("PropertiesTreeItem::GetValue not implemented for type: " % QString::number(propDefRef.eType), LOGTYPE_Error);
		break;
	}

	sRetStr += propDefRef.sSuffix;

	return sRetStr;
}
