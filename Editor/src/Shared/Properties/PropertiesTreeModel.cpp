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
#include "MainWindow.h"

PropertiesTreeModel::PropertiesTreeModel(ProjectItemData &ownerRef, int iStateIndex, QVariant subState, QObject *pParent /*= nullptr*/) :
	ITreeModel(2, { "Property", "Value" }, pParent),
	m_OwnerRef(ownerRef),
	m_iSTATE_INDEX(iStateIndex),
	m_iSUBSTATE(subState)
{
}

/*virtual*/ PropertiesTreeModel::~PropertiesTreeModel()
{
}

ProjectItemData &PropertiesTreeModel::GetOwner()
{
	return m_OwnerRef;
}

int PropertiesTreeModel::GetStateIndex() const
{
	return m_iSTATE_INDEX;
}

const QVariant &PropertiesTreeModel::GetSubstate() const
{
	return m_iSUBSTATE;
}

const PropertiesDef PropertiesTreeModel::GetPropertyDefinition(const QModelIndex &indexRef) const
{
	TreeModelItem *pTreeItem = GetItem(indexRef);
	return m_PropertyDefMap[pTreeItem];
}

const PropertiesDef PropertiesTreeModel::FindPropertyDefinition(QString sCategoryName, QString sPropertyName) const
{
	for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
	{
		if(0 == m_pRootItem->GetChild(i)->data(COLUMN_Name).toString().compare(sCategoryName, Qt::CaseSensitive))
		{
			TreeModelItem *pCategoryTreeItem = m_pRootItem->GetChild(i);
			for(int j = 0; j < pCategoryTreeItem->GetNumChildren(); ++j)
			{
				if(0 == pCategoryTreeItem->GetChild(j)->data(COLUMN_Name).toString().compare(sPropertyName, Qt::CaseSensitive))
				{
					TreeModelItem *pPropertyItem = pCategoryTreeItem->GetChild(j);
					return m_PropertyDefMap[pPropertyItem];
				}
			}
		}
	}

	return PropertiesDef();
}

QString PropertiesTreeModel::GetPropertyName(const QModelIndex &indexRef) const
{
	TreeModelItem *pTreeItem = GetItem(indexRef);
	return pTreeItem->data(COLUMN_Name).toString();
}

QVariant PropertiesTreeModel::GetPropertyValue(const QModelIndex &indexRef) const
{
	TreeModelItem *pTreeItem = GetItem(indexRef);
	return pTreeItem->data(COLUMN_Value);
}

QVariant PropertiesTreeModel::FindPropertyValue(QString sCategoryName, QString sPropertyName) const
{
	for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
	{
		if(0 == m_pRootItem->GetChild(i)->data(COLUMN_Name).toString().compare(sCategoryName, Qt::CaseSensitive))
		{
			TreeModelItem *pCategoryTreeItem = m_pRootItem->GetChild(i);
			for(int j = 0; j < pCategoryTreeItem->GetNumChildren(); ++j)
			{
				if(0 == pCategoryTreeItem->GetChild(j)->data(COLUMN_Name).toString().compare(sPropertyName, Qt::CaseSensitive))
					return pCategoryTreeItem->GetChild(j)->data(COLUMN_Value);
			}
		}
	}

	return QVariant();
}

void PropertiesTreeModel::SetPropertyValue(QString sCategoryName, QString sPropertyName, const QVariant &valueRef)
{
	for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
	{
		if(0 == m_pRootItem->GetChild(i)->data(COLUMN_Name).toString().compare(sCategoryName, Qt::CaseSensitive))
		{
			TreeModelItem *pCategoryTreeItem = m_pRootItem->GetChild(i);
			for(int j = 0; j < pCategoryTreeItem->GetNumChildren(); ++j)
			{
				if(0 == pCategoryTreeItem->GetChild(j)->data(COLUMN_Name).toString().compare(sPropertyName, Qt::CaseSensitive))
				{
					if(setData(createIndex(pCategoryTreeItem->GetChild(j)->GetIndex(), COLUMN_Value, pCategoryTreeItem->GetChild(j)), valueRef, Qt::UserRole) == false)
						HyGuiLog("PropertiesTreeModel::SetPropertyValue() - setData failed", LOGTYPE_Error);

					return;
				}
			}
		}
	}
}

bool PropertiesTreeModel::IsCategoryEnabled(QString sCategoryName)
{
	for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
	{
		if(0 == m_pRootItem->GetChild(i)->data(COLUMN_Name).toString().compare(sCategoryName, Qt::CaseSensitive))
		{
			TreeModelItem *pCategoryTreeItem = m_pRootItem->GetChild(i);
			const PropertiesDef &categoryPropDefRef = m_PropertyDefMap[pCategoryTreeItem];

			if(categoryPropDefRef.eType == PROPERTIESTYPE_Category ||
			   (categoryPropDefRef.eType == PROPERTIESTYPE_CategoryChecked && pCategoryTreeItem->data(COLUMN_Value).toInt() == Qt::Checked))
			{
				return true;
			}
			
			return false;
		}
	}

	return false;
}

bool PropertiesTreeModel::AppendCategory(QString sCategoryName, QVariant commonDelegateBuilder /*= QVariant()*/, bool bCheckable /*= false*/, bool bStartChecked /*= false*/, QString sToolTip /*= ""*/)
{
	// All category names must be unique
	for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
	{
		if(0 == m_pRootItem->GetChild(i)->data(COLUMN_Name).toString().compare(sCategoryName, Qt::CaseSensitive))
			return false;
	}

	// Create the row inside the model
	QModelIndex rootParentIndex = createIndex(m_pRootItem->GetIndex(), 0, m_pRootItem);
	if(insertRow(m_pRootItem->GetNumChildren(), rootParentIndex) == false)
	{
		HyGuiLog("PropertiesTreeModel::AppendCategory() - insertRow failed", LOGTYPE_Error);
		return false;
	}

	// Set data in the property's name column
	TreeModelItem *pNewlyAddedTreeItem = m_pRootItem->GetChild(m_pRootItem->GetNumChildren() - 1);
	if(setData(index(pNewlyAddedTreeItem->GetIndex(), COLUMN_Name, rootParentIndex), QVariant(sCategoryName), Qt::UserRole) == false)
		HyGuiLog("PropertiesTreeModel::AppendCategory() - setData failed", LOGTYPE_Error);

	// Set data in the property's value column
	if(setData(index(pNewlyAddedTreeItem->GetIndex(), COLUMN_Value, rootParentIndex), QVariant(bStartChecked ? Qt::Checked : Qt::Unchecked), Qt::UserRole) == false)
		HyGuiLog("PropertiesTreeModel::AppendCategory() - setData failed", LOGTYPE_Error);

	// Link this property definition to the proper TreeModelItem using 'm_PropertyDefMap'
	PropertiesDef def;
	def.eType = bCheckable ? PROPERTIESTYPE_CategoryChecked : PROPERTIESTYPE_Category;
	def.bReadOnly = !bCheckable;
	def.delegateBuilder = commonDelegateBuilder;
	def.sToolTip = sToolTip;

	m_PropertyDefMap[pNewlyAddedTreeItem] = def;

	return true;
}

bool PropertiesTreeModel::AppendProperty(QString sCategoryName,
										 QString sName,
										 PropertiesType eType,
										 QVariant defaultData /*= QVariant()*/,
										 QString sToolTip /*= QString()*/,
										 bool bReadOnly /*= false*/,
										 QVariant minRange /*= QVariant()*/,
										 QVariant maxRange /*= QVariant()*/,
										 QVariant stepAmt /*= QVariant()*/,
										 QString sPrefix /*= QString()*/,
										 QString sSuffix /*= QString()*/,
										 QVariant delegateBuilder /*= QVariant()*/)
{
	// Find category to add property to
	TreeModelItem *pCategoryTreeItem = nullptr;
	for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
	{
		if(0 == m_pRootItem->GetChild(i)->data(COLUMN_Name).toString().compare(sCategoryName, Qt::CaseSensitive))
		{
			pCategoryTreeItem = m_pRootItem->GetChild(i);
			break;
		}
	}
	if(pCategoryTreeItem == nullptr)
	{
		HyGuiLog("PropertiesTreeModel::AppendProperty() - Could not find category: " % sCategoryName, LOGTYPE_Error);
		return false;
	}

	// Now ensure that no property with this name already exists
	for(int i = 0; i < pCategoryTreeItem->GetNumChildren(); ++i)
	{
		if(0 == pCategoryTreeItem->GetChild(i)->data(COLUMN_Name).toString().compare(sName, Qt::CaseSensitive))
		{
			HyGuiLog("PropertiesTreeModel::AppendProperty() - Property with the name: " % sName % " already exists", LOGTYPE_Error);
			return false;
		}
	}

	// Create the row inside the model
	QModelIndex categoryParentIndex = createIndex(pCategoryTreeItem->GetIndex(), 0, pCategoryTreeItem);
	if(insertRow(pCategoryTreeItem->GetNumChildren(), categoryParentIndex) == false)
	{
		HyGuiLog("PropertiesTreeModel::AppendCategory() - insertRow failed", LOGTYPE_Error);
		return false;
	}

	// Set data in the property's name column
	TreeModelItem *pNewlyAddedTreeItem = pCategoryTreeItem->GetChild(pCategoryTreeItem->GetNumChildren() - 1);
	if(setData(index(pNewlyAddedTreeItem->GetIndex(), COLUMN_Name, categoryParentIndex), QVariant(sName), Qt::UserRole) == false)
		HyGuiLog("PropertiesTreeModel::AppendProperty() - setData failed", LOGTYPE_Error);

	// Set data in the property's value column
	if(setData(index(pNewlyAddedTreeItem->GetIndex(), COLUMN_Value, categoryParentIndex), defaultData, Qt::UserRole) == false)
		HyGuiLog("PropertiesTreeModel::AppendProperty() - setData failed", LOGTYPE_Error);

	// Link this property definition to the proper TreeModelItem using 'm_PropertyDefMap'
	PropertiesDef def(eType, bReadOnly, sToolTip, defaultData, minRange, maxRange, stepAmt, sPrefix, sSuffix, delegateBuilder);
	m_PropertyDefMap[pNewlyAddedTreeItem] = def;

	return true;
}

void PropertiesTreeModel::RefreshCategory(const QModelIndex &indexRef)
{
	TreeModelItem *pCategoryTreeItem = GetItem(indexRef);
	if(pCategoryTreeItem->GetNumChildren() > 0)
	{
		dataChanged(createIndex(0, COLUMN_Name, pCategoryTreeItem->GetChild(0)),
					createIndex(pCategoryTreeItem->GetNumChildren() - 1, COLUMN_Value, pCategoryTreeItem->GetChild(pCategoryTreeItem->GetNumChildren() - 1)));
	}
}

QJsonObject PropertiesTreeModel::SerializeJson()
{
	QJsonObject propertiesObj;

	for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
	{
		TreeModelItem *pCategoryTreeItem = m_pRootItem->GetChild(i);

		QJsonObject categoryObj;
		for(int j = 0; j < pCategoryTreeItem->GetNumChildren(); ++j)
		{
			TreeModelItem *pPropertyItem = pCategoryTreeItem->GetChild(j);
			QString sPropName = pPropertyItem->data(COLUMN_Name).toString();
			QVariant propValue = pPropertyItem->data(COLUMN_Value);

			const PropertiesDef &propDefRef = m_PropertyDefMap[pPropertyItem];
			switch(propDefRef.eType)
			{
			case PROPERTIESTYPE_bool:
				categoryObj.insert(sPropName, propValue.toBool());
				break;
			case PROPERTIESTYPE_int:
			case PROPERTIESTYPE_ComboBox:
			case PROPERTIESTYPE_Slider:
			case PROPERTIESTYPE_StatesComboBox:
			case PROPERTIESTYPE_SpriteFrames:
				categoryObj.insert(sPropName, propValue.toInt());
				break;
			case PROPERTIESTYPE_double:
				categoryObj.insert(sPropName, propValue.toDouble());
				break;
			case PROPERTIESTYPE_ivec2:
				categoryObj.insert(sPropName, QJsonArray() << propValue.toPoint().x() << propValue.toPoint().y());
				break;
			case PROPERTIESTYPE_vec2:
				categoryObj.insert(sPropName, QJsonArray() << propValue.toPointF().x() << propValue.toPointF().y());
				break;
			case PROPERTIESTYPE_ivec3:
			case PROPERTIESTYPE_Color:
				categoryObj.insert(sPropName, QJsonArray() << propValue.toRect().left() << propValue.toRect().top() << propValue.toRect().width());
				break;
			case PROPERTIESTYPE_vec3:
				categoryObj.insert(sPropName, QJsonArray() << propValue.toRectF().left() << propValue.toRectF().top() << propValue.toRectF().width());
				break;
			case PROPERTIESTYPE_ivec4:
				categoryObj.insert(sPropName, QJsonArray() << propValue.toRect().left() << propValue.toRect().top() << propValue.toRect().width() << propValue.toRect().height());
				break;
			case PROPERTIESTYPE_vec4:
				categoryObj.insert(sPropName, QJsonArray() << propValue.toRectF().left() << propValue.toRectF().top() << propValue.toRectF().width() << propValue.toRectF().height());
				break;
			case PROPERTIESTYPE_LineEdit:
				categoryObj.insert(sPropName, propValue.toString());
				break;

			default:
				HyGuiLog("Unhandled PropertiesTreeModel::SerializeJson property", LOGTYPE_Error);
				break;
			}
			
		}
		propertiesObj.insert(pCategoryTreeItem->data(COLUMN_Name).toString(), categoryObj);
	}

	return propertiesObj;
}

void PropertiesTreeModel::DeserializeJson(const QJsonObject &propertiesObj)
{
	QStringList sCategoryList = propertiesObj.keys();
	for(const QString &sCategory : sCategoryList)
	{
		QJsonObject categoryObj = propertiesObj[sCategory].toObject();

		QStringList sPropertyList = categoryObj.keys();
		for(const QString &sProperty : sPropertyList)
		{
			const PropertiesDef propDef = FindPropertyDefinition(sCategory, sProperty);
			switch(propDef.eType)
			{
			case PROPERTIESTYPE_Unknown:
				break;

			case PROPERTIESTYPE_bool:
				SetPropertyValue(sCategory, sProperty, categoryObj[sProperty].toBool());
				break;

			case PROPERTIESTYPE_int:
			case PROPERTIESTYPE_ComboBox:
			case PROPERTIESTYPE_StatesComboBox:
			case PROPERTIESTYPE_Slider:
			case PROPERTIESTYPE_SpriteFrames:
				SetPropertyValue(sCategory, sProperty, categoryObj[sProperty].toInt());
				break;
					
			case PROPERTIESTYPE_double:
				SetPropertyValue(sCategory, sProperty, categoryObj[sProperty].toDouble());
				break;

			case PROPERTIESTYPE_ivec2: {
				QJsonArray ivec2Array = categoryObj[sProperty].toArray();
				QPoint ptPoint(ivec2Array[0].toInt(), ivec2Array[1].toInt());
				SetPropertyValue(sCategory, sProperty, ptPoint);
				break; }
					
			case PROPERTIESTYPE_vec2: {
				QJsonArray vec2Array = categoryObj[sProperty].toArray();
				QPointF ptPoint(vec2Array[0].toDouble(), vec2Array[1].toDouble());
				SetPropertyValue(sCategory, sProperty, ptPoint);
				break; }

			case PROPERTIESTYPE_ivec3:
			case PROPERTIESTYPE_Color: {
				QJsonArray ivec3Array = categoryObj[sProperty].toArray();
				QRect rect(ivec3Array[0].toInt(), ivec3Array[1].toInt(), ivec3Array[2].toInt(), 0);
				SetPropertyValue(sCategory, sProperty, rect);
				break; }

			case PROPERTIESTYPE_vec3: {
				QJsonArray vec3Array = categoryObj[sProperty].toArray();
				QRectF rect(vec3Array[0].toDouble(), vec3Array[1].toDouble(), vec3Array[2].toDouble(), 0.0);
				SetPropertyValue(sCategory, sProperty, rect);
				break; }

			case PROPERTIESTYPE_ivec4: {
				QJsonArray ivec4Array = categoryObj[sProperty].toArray();
				QRect rect(ivec4Array[0].toInt(), ivec4Array[1].toInt(), ivec4Array[2].toInt(), ivec4Array[3].toInt());
				SetPropertyValue(sCategory, sProperty, rect);
				break; }

			case PROPERTIESTYPE_vec4: {
				QJsonArray vec4Array = categoryObj[sProperty].toArray();
				QRectF rect(vec4Array[0].toDouble(), vec4Array[1].toDouble(), vec4Array[2].toDouble(), vec4Array[3].toDouble());
				SetPropertyValue(sCategory, sProperty, rect);
				break; }

			case PROPERTIESTYPE_LineEdit:
				SetPropertyValue(sCategory, sProperty, categoryObj[sProperty].toString());
				break;

			default:
				HyGuiLog("Unhandled PropertiesTreeModel::DeserializeJson property", LOGTYPE_Error);
				break;
			}
		}
	}
}

/*virtual*/ bool PropertiesTreeModel::setData(const QModelIndex &indexRef, const QVariant &valueRef, int iRole /*= Qt::EditRole*/) /*override*/
{
	if(iRole == Qt::UserRole)
		return ITreeModel::setData(indexRef, valueRef, iRole);

	const QVariant &origValue = GetPropertyValue(indexRef);
	if(origValue != valueRef)
	{
		PropertiesUndoCmd *pUndoCmd = new PropertiesUndoCmd(this, indexRef, valueRef);
		GetOwner().GetUndoStack()->push(pUndoCmd);
	}

	return false; // Return false because another setData() will be invoked via the UndoCmd, which actually changes the data
}

/*virtual*/ QVariant PropertiesTreeModel::data(const QModelIndex &indexRef, int iRole) const /*override*/
{
	if(indexRef.isValid() == false)
		return QVariant();

	if(iRole == Qt::UserRole)
		return ITreeModel::data(indexRef, iRole);

	TreeModelItem *pTreeItem = GetItem(indexRef);
	const PropertiesDef &propDefRef = m_PropertyDefMap[pTreeItem];

	switch(iRole)
	{
	case Qt::DisplayRole:
		if(indexRef.column() == COLUMN_Name)
			return pTreeItem->data(COLUMN_Name);
		else
			return ConvertValueToString(pTreeItem);

	case Qt::TextAlignmentRole:
		return Qt::AlignLeft;
		//if(propDefRef.IsCategory())
		//	return Qt::AlignHCenter;

	case Qt::ToolTipRole:
		return propDefRef.sToolTip;

	case Qt::BackgroundRole:
		if(propDefRef.IsCategory())
			return QBrush(propDefRef.GetColor());
		else if(indexRef.column() == COLUMN_Value && propDefRef.eType == PROPERTIESTYPE_Color)
			return QBrush(QColor(pTreeItem->data(COLUMN_Value).toRect().left(), pTreeItem->data(COLUMN_Value).toRect().top(), pTreeItem->data(COLUMN_Value).toRect().width()));
		//	return QBrush((0 == (pTreeItem->GetIndex() & 1)) ? propDefRef.GetColor() : propDefRef.GetColor().lighter());

	case Qt::ForegroundRole:
		if(propDefRef.IsCategory())
			return QBrush(QColor::fromRgb(255, 255, 255));
		else if(indexRef.column() == COLUMN_Value && propDefRef.eType == PROPERTIESTYPE_Color)
		{
			QColor clr(pTreeItem->data(COLUMN_Value).toRect().left(), pTreeItem->data(COLUMN_Value).toRect().top(), pTreeItem->data(COLUMN_Value).toRect().width());
			double a = 1 - (0.299 * clr.redF() + 0.587 * clr.greenF() + 0.114 * clr.blueF()) / 255;
			if(a < 0.5)
			{
				QBrush bgColorBrush(Qt::black);
				return QVariant(bgColorBrush);
			}
			else
			{
				QBrush bgColorBrush(Qt::white);
				return QVariant(bgColorBrush);
			}
		}

	case Qt::FontRole:
		if(propDefRef.IsCategory())
		{
			QFont font;
			font.setBold(true);
			return font;
		}

	case Qt::CheckStateRole:
		if((indexRef.column() == 0 && propDefRef.eType == PROPERTIESTYPE_CategoryChecked) ||
		   (indexRef.column() == 1 && propDefRef.eType == PROPERTIESTYPE_bool))
		{
			return pTreeItem->data(COLUMN_Value);
		}
	}

	return QVariant();
}

/*virtual*/ Qt::ItemFlags PropertiesTreeModel::flags(const QModelIndex &indexRef) const /*override*/
{
	Qt::ItemFlags returnFlags = Qt::NoItemFlags;

	if(indexRef.isValid() == false)
		return returnFlags;

	TreeModelItem *pTreeItem = GetItem(indexRef);
	const PropertiesDef &propDefRef = m_PropertyDefMap[pTreeItem];

	if(propDefRef.IsCategory())
	{
		if(propDefRef.bReadOnly == false)
			returnFlags |= Qt::ItemIsEnabled;

		if(propDefRef.eType == PROPERTIESTYPE_CategoryChecked && indexRef.column() == COLUMN_Name)
			returnFlags |= Qt::ItemIsUserCheckable;
	}
	else
	{
		TreeModelItem *pCategoryTreeItem = pTreeItem->GetParent();
		const PropertiesDef &categoryPropDefRef = m_PropertyDefMap[pCategoryTreeItem];

		if(categoryPropDefRef.IsCategory() == false)
			HyGuiLog("PropertiesTreeModel::flags() passed in index is not a category and its parent is not one either", LOGTYPE_Error);

		if(categoryPropDefRef.eType == PROPERTIESTYPE_Category ||
		   (categoryPropDefRef.eType == PROPERTIESTYPE_CategoryChecked && pCategoryTreeItem->data(COLUMN_Value).toInt() == Qt::Checked))
		{
			if(propDefRef.bReadOnly == false)
				returnFlags |= Qt::ItemIsEnabled;
		}

		if(indexRef.column() == COLUMN_Value)
		{
			if(propDefRef.bReadOnly == false)
				returnFlags |= (Qt::ItemIsSelectable | Qt::ItemIsEditable);

			if(propDefRef.eType == PROPERTIESTYPE_bool)
				returnFlags |= Qt::ItemIsUserCheckable;
		}
	}

	return returnFlags;
}

/*virtual*/ void PropertiesTreeModel::OnTreeModelItemRemoved(TreeModelItem *pTreeItem) /*PropertiesTreeModel*/
{
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
		break; }
	case PROPERTIESTYPE_vec2: {
		QPointF pt = treeItemValue.toPointF();
		sRetStr += QString::number(pt.x()) % " x " % QString::number(pt.y());
		break; }
	case PROPERTIESTYPE_ivec3: {
		QRect rect = treeItemValue.toRect();
		sRetStr += "{ " % QString::number(rect.left()) % ", " % QString::number(rect.top()) % ", " % QString::number(rect.width()) % " }";
		break; }
	case PROPERTIESTYPE_vec3: {
		QRectF rect = treeItemValue.toRectF();
		sRetStr += "{ " % QString::number(rect.left()) % ", " % QString::number(rect.top()) % ", " % QString::number(rect.width()) % " }";
		break; }
	case PROPERTIESTYPE_ivec4: {
		QRect rect = treeItemValue.toRect();
		sRetStr += "{ " % QString::number(rect.left()) % ", " % QString::number(rect.top()) % ", " % QString::number(rect.width()) % ", " % QString::number(rect.height()) % " }";
		break; }
	case PROPERTIESTYPE_vec4: {
		QRectF rect = treeItemValue.toRectF();
		sRetStr += "{ " % QString::number(rect.left()) % ", " % QString::number(rect.top()) % ", " % QString::number(rect.width()) % ", " % QString::number(rect.height()) % " }";
		break; }
	case PROPERTIESTYPE_LineEdit:
		sRetStr += treeItemValue.toString();
		break;
	case PROPERTIESTYPE_ComboBox:
		sRetStr += propDefRef.delegateBuilder.toStringList()[treeItemValue.toInt()];
		break;
	case PROPERTIESTYPE_StatesComboBox: {
		sRetStr += treeItemValue.toString();
		//ProjectItemData *pProjItem = MainWindow::GetExplorerModel().FindByUuid(QUuid(propDefRef.delegateBuilder.toString()));
		//if(pProjItem)
		//{
		//	QComboBox tmpComboBox(nullptr);
		//	tmpComboBox.setModel(pProjItem->GetModel());
		//	sRetStr += tmpComboBox.itemText(treeItemValue.toInt());
		//}
		//else
		//	HyGuiLog("PROPERTIESTYPE_StatesComboBox could not find UUID", LOGTYPE_Error);
		break; }
	case PROPERTIESTYPE_Color: {
		QRect rect = treeItemValue.toRect();
		sRetStr += "RGB(" % QString::number(rect.left()) % ", " % QString::number(rect.top()) % ", " % QString::number(rect.width()) % ")";
		break; }
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
