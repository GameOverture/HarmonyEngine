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
		if(0 == m_pRootItem->GetChild(i)->data(PROPERTIESCOLUMN_Name).toString().compare(sCategoryName, Qt::CaseSensitive))
		{
			TreeModelItem *pCategoryTreeItem = m_pRootItem->GetChild(i);
			for(int j = 0; j < pCategoryTreeItem->GetNumChildren(); ++j)
			{
				if(0 == pCategoryTreeItem->GetChild(j)->data(PROPERTIESCOLUMN_Name).toString().compare(sPropertyName, Qt::CaseSensitive))
				{
					TreeModelItem *pPropertyItem = pCategoryTreeItem->GetChild(j);
					return m_PropertyDefMap[pPropertyItem];
				}
			}
		}
	}

	return PropertiesDef();
}

void PropertiesTreeModel::SetToggle(QString sCategoryName, bool bToggleOn)
{
	for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
	{
		if(0 == m_pRootItem->GetChild(i)->data(PROPERTIESCOLUMN_Name).toString().compare(sCategoryName, Qt::CaseSensitive))
		{
			SetToggle(createIndex(i, PROPERTIESCOLUMN_Name, m_pRootItem->GetChild(i)), bToggleOn);
			return;
		}
	}
}

void PropertiesTreeModel::SetToggle(QString sCategoryName, QString sPropertyName, bool bToggleOn)
{
	for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
	{
		if(0 == m_pRootItem->GetChild(i)->data(PROPERTIESCOLUMN_Name).toString().compare(sCategoryName, Qt::CaseSensitive))
		{
			TreeModelItem *pCategoryTreeItem = m_pRootItem->GetChild(i);
			for(int j = 0; j < pCategoryTreeItem->GetNumChildren(); ++j)
			{
				if(0 == pCategoryTreeItem->GetChild(j)->data(PROPERTIESCOLUMN_Name).toString().compare(sPropertyName, Qt::CaseSensitive))
				{
					SetToggle(createIndex(j, PROPERTIESCOLUMN_Name, pCategoryTreeItem->GetChild(j)), bToggleOn);
					return;
				}
			}
		}
	}
}

/*virtual*/ void PropertiesTreeModel::SetToggle(const QModelIndex &indexRef, bool bToggleOn)
{
	TreeModelItem *pTreeItem = GetItem(indexRef);
	m_PropertyDefMap[pTreeItem].eAccessType = bToggleOn ? PROPERTIESACCESS_ToggleOn : PROPERTIESACCESS_ToggleOff;

	// Apply the dataChanged signal
	if(pTreeItem->GetNumChildren() > 0)
	{
		// This is a category
		Q_EMIT dataChanged(createIndex(0, PROPERTIESCOLUMN_Name, pTreeItem->GetChild(0)),
						   createIndex(pTreeItem->GetNumChildren() - 1, PROPERTIESCOLUMN_Value, pTreeItem->GetChild(pTreeItem->GetNumChildren() - 1)));
	}
	else
		Q_EMIT dataChanged(indexRef, indexRef);
}

int PropertiesTreeModel::GetNumProperties(int iCategoryIndex) const
{
	return m_pRootItem->GetChild(iCategoryIndex)->GetNumChildren();
}

QString PropertiesTreeModel::GetPropertyName(const QModelIndex &indexRef) const
{
	TreeModelItem *pTreeItem = GetItem(indexRef);
	return pTreeItem->data(PROPERTIESCOLUMN_Name).toString();
}

QString PropertiesTreeModel::GetPropertyName(int iCategoryIndex, int iPropertyIndex) const
{
	return m_pRootItem->GetChild(iCategoryIndex)->GetChild(iPropertyIndex)->data(PROPERTIESCOLUMN_Name).toString();
}

QVariant PropertiesTreeModel::GetPropertyValue(const QModelIndex &indexRef) const
{
	TreeModelItem *pTreeItem = GetItem(indexRef);
	return pTreeItem->data(PROPERTIESCOLUMN_Value);
}

QVariant PropertiesTreeModel::GetPropertyValue(int iCategoryIndex, int iPropertyIndex) const
{
	TreeModelItem *pCategoryTreeItem = m_pRootItem->GetChild(iCategoryIndex);
	TreeModelItem *pPropertyTreeItem = pCategoryTreeItem->GetChild(iPropertyIndex);
	return pPropertyTreeItem->data(PROPERTIESCOLUMN_Value);
}

QJsonValue PropertiesTreeModel::GetPropertyJsonValue(const QModelIndex &indexRef) const
{
	TreeModelItem *pTreeItem = GetItem(indexRef);
	return ConvertValueToJson(pTreeItem);
}

QJsonValue PropertiesTreeModel::GetPropertyJsonValue(int iCategoryIndex, int iPropertyIndex) const
{
	TreeModelItem *pCategoryTreeItem = m_pRootItem->GetChild(iCategoryIndex);
	TreeModelItem *pPropertyTreeItem = pCategoryTreeItem->GetChild(iPropertyIndex);
	return ConvertValueToJson(pPropertyTreeItem);
}

bool PropertiesTreeModel::IsPropertyDefaultValue(int iCategoryIndex, int iPropertyIndex) const
{
	TreeModelItem *pCategoryTreeItem = m_pRootItem->GetChild(iCategoryIndex);
	TreeModelItem *pPropertyTreeItem = pCategoryTreeItem->GetChild(iPropertyIndex);
	return pPropertyTreeItem->data(PROPERTIESCOLUMN_Value) == m_PropertyDefMap[pPropertyTreeItem].defaultData;
}

QVariant PropertiesTreeModel::FindPropertyValue(QString sCategoryName, QString sPropertyName) const
{
	for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
	{
		if(0 == m_pRootItem->GetChild(i)->data(PROPERTIESCOLUMN_Name).toString().compare(sCategoryName, Qt::CaseSensitive))
		{
			TreeModelItem *pCategoryTreeItem = m_pRootItem->GetChild(i);
			for(int j = 0; j < pCategoryTreeItem->GetNumChildren(); ++j)
			{
				if(0 == pCategoryTreeItem->GetChild(j)->data(PROPERTIESCOLUMN_Name).toString().compare(sPropertyName, Qt::CaseSensitive))
					return pCategoryTreeItem->GetChild(j)->data(PROPERTIESCOLUMN_Value);
			}
		}
	}

	return QVariant();
}

QJsonValue PropertiesTreeModel::FindPropertyJsonValue(QString sCategoryName, QString sPropertyName) const
{
	for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
	{
		if(0 == m_pRootItem->GetChild(i)->data(PROPERTIESCOLUMN_Name).toString().compare(sCategoryName, Qt::CaseSensitive))
		{
			TreeModelItem *pCategoryTreeItem = m_pRootItem->GetChild(i);
			for(int j = 0; j < pCategoryTreeItem->GetNumChildren(); ++j)
			{
				if(0 == pCategoryTreeItem->GetChild(j)->data(PROPERTIESCOLUMN_Name).toString().compare(sPropertyName, Qt::CaseSensitive))
					return ConvertValueToJson(pCategoryTreeItem->GetChild(j));
			}
		}
	}

	return QJsonValue();
}

QModelIndex PropertiesTreeModel::FindPropertyModelIndex(QString sCategoryName, QString sPropertyName) const
{
	for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
	{
		if(0 == m_pRootItem->GetChild(i)->data(PROPERTIESCOLUMN_Name).toString().compare(sCategoryName, Qt::CaseSensitive))
		{
			TreeModelItem *pCategoryTreeItem = m_pRootItem->GetChild(i);
			for(int j = 0; j < pCategoryTreeItem->GetNumChildren(); ++j)
			{
				if(0 == pCategoryTreeItem->GetChild(j)->data(PROPERTIESCOLUMN_Name).toString().compare(sPropertyName, Qt::CaseSensitive))
					return index(j, PROPERTIESCOLUMN_Name, index(i, PROPERTIESCOLUMN_Name));
			}
		}
	}

	return QModelIndex();
}

/*virtual*/ void PropertiesTreeModel::SetPropertyValue(QString sCategoryName, QString sPropertyName, const QVariant &valueRef, bool bIsProceduralObj)
{
	for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
	{
		if(0 == m_pRootItem->GetChild(i)->data(PROPERTIESCOLUMN_Name).toString().compare(sCategoryName, Qt::CaseSensitive))
		{
			TreeModelItem *pCategoryTreeItem = m_pRootItem->GetChild(i);
			for(int j = 0; j < pCategoryTreeItem->GetNumChildren(); ++j)
			{
				TreeModelItem *pPropertyTreeItem = pCategoryTreeItem->GetChild(j);

				if(0 == pPropertyTreeItem->data(PROPERTIESCOLUMN_Name).toString().compare(sPropertyName, Qt::CaseSensitive))
				{
					if(setData(createIndex(pPropertyTreeItem->GetIndex(), PROPERTIESCOLUMN_Value, pPropertyTreeItem), valueRef, Qt::UserRole) == false)
						HyGuiLog("PropertiesTreeModel::SetPropertyValue() - setData failed", LOGTYPE_Error);

					m_PropertyDefMap[pPropertyTreeItem].m_bIsProceduralValue = bIsProceduralObj;

					if(m_PropertyDefMap[pPropertyTreeItem].eAccessType == PROPERTIESACCESS_ToggleOff)
						SetToggle(createIndex(pPropertyTreeItem->GetIndex(), PROPERTIESCOLUMN_Name, pPropertyTreeItem), true);

					return;
				}
			}
		}
	}
}

bool PropertiesTreeModel::IsCategory(const QModelIndex &indexRef) const
{
	TreeModelItem *pTreeItem = GetItem(indexRef);
	return m_PropertyDefMap[pTreeItem].eType == PROPERTIESTYPE_Category;
}

bool PropertiesTreeModel::IsCategoryEnabled(QString sCategoryName) const
{
	for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
	{
		if(0 == m_pRootItem->GetChild(i)->data(PROPERTIESCOLUMN_Name).toString().compare(sCategoryName, Qt::CaseSensitive))
		{
			TreeModelItem *pCategoryTreeItem = m_pRootItem->GetChild(i);
			const PropertiesDef &categoryPropDefRef = m_PropertyDefMap[pCategoryTreeItem];

			if(categoryPropDefRef.eType == PROPERTIESTYPE_Category && categoryPropDefRef.eAccessType == PROPERTIESACCESS_ToggleOff)
				return false;
			
			return true;
		}
	}

	return false;
}

bool PropertiesTreeModel::IsCategoryEnabled(int iCategoryIndex) const
{
	TreeModelItem *pCategoryTreeItem = m_pRootItem->GetChild(iCategoryIndex);
	const PropertiesDef &categoryPropDefRef = m_PropertyDefMap[pCategoryTreeItem];
	if(categoryPropDefRef.eType != PROPERTIESTYPE_Category || categoryPropDefRef.eAccessType == PROPERTIESACCESS_ToggleOff)
		return false;
	
	return true;
}

int PropertiesTreeModel::GetNumCategories() const
{
	return m_pRootItem->GetNumChildren();
}

QString PropertiesTreeModel::GetCategoryName(const QModelIndex &indexRef) const
{
	TreeModelItem *pTreeItem = GetItem(indexRef);
	if(m_PropertyDefMap[pTreeItem].eType != PROPERTIESTYPE_Category)
	{
		pTreeItem = GetItem(indexRef.parent());
		if(m_PropertyDefMap[pTreeItem].eType != PROPERTIESTYPE_Category)
			HyGuiLog("PropertiesTreeModel::GetCategoryName() - could not determine category from 'indexRef'", LOGTYPE_Error);
	}

	return pTreeItem->data(PROPERTIESCOLUMN_Name).toString();
}

QString PropertiesTreeModel::GetCategoryName(int iCategoryIndex) const
{
	return m_pRootItem->GetChild(iCategoryIndex)->data(PROPERTIESCOLUMN_Name).toString();
}

QModelIndex PropertiesTreeModel::GetCategoryModelIndex(int iCategoryIndex) const
{
	return index(iCategoryIndex, PROPERTIESCOLUMN_Name);
}

bool PropertiesTreeModel::IsCategoryCheckable(int iCategoryIndex) const
{
	const PropertiesDef &categoryPropDefRef = m_PropertyDefMap[m_pRootItem->GetChild(iCategoryIndex)];
	return categoryPropDefRef.eAccessType == PROPERTIESACCESS_ToggleOn || categoryPropDefRef.eAccessType == PROPERTIESACCESS_ToggleOff;
}

bool PropertiesTreeModel::AppendCategory(QString sCategoryName, QVariant commonDelegateBuilder /*= QVariant()*/, bool bCheckable /*= false*/, QString sToolTip /*= ""*/)
{
	// All category names must be unique
	for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
	{
		if(0 == m_pRootItem->GetChild(i)->data(PROPERTIESCOLUMN_Name).toString().compare(sCategoryName, Qt::CaseSensitive))
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
	if(setData(index(pNewlyAddedTreeItem->GetIndex(), PROPERTIESCOLUMN_Name, rootParentIndex), QVariant(sCategoryName), Qt::UserRole) == false)
		HyGuiLog("PropertiesTreeModel::AppendCategory() - setData failed", LOGTYPE_Error);

	//// Set data in the property's value column
	//if(setData(index(pNewlyAddedTreeItem->GetIndex(), PROPERTIESCOLUMN_Value, rootParentIndex), QVariant(bStartChecked ? Qt::Checked : Qt::Unchecked), Qt::UserRole) == false)
	//	HyGuiLog("PropertiesTreeModel::AppendCategory() - setData failed", LOGTYPE_Error);

	// Link this property definition to the proper TreeModelItem using 'm_PropertyDefMap'
	PropertiesDef def;
	def.eType = PROPERTIESTYPE_Category;
	def.delegateBuilder = commonDelegateBuilder;
	def.sToolTip = sToolTip;
	if(bCheckable == false)
		def.eAccessType = PROPERTIESACCESS_ReadOnly;
	else // If it's checkable, then it is defaulted to unchecked
		def.eAccessType = PROPERTIESACCESS_ToggleOff;

	m_PropertyDefMap[pNewlyAddedTreeItem] = def;

	return true;
}

bool PropertiesTreeModel::AppendProperty(QString sCategoryName,
										 QString sName,
										 PropertiesType eType,
										 QVariant defaultData /*= QVariant()*/,
										 QString sToolTip /*= QString()*/,
										 PropertiesAccessType eAccessType /*= PROPERTIESACCESS_Mutable*/,
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
		if(0 == m_pRootItem->GetChild(i)->data(PROPERTIESCOLUMN_Name).toString().compare(sCategoryName, Qt::CaseSensitive))
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
		if(0 == pCategoryTreeItem->GetChild(i)->data(PROPERTIESCOLUMN_Name).toString().compare(sName, Qt::CaseSensitive))
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
	if(setData(index(pNewlyAddedTreeItem->GetIndex(), PROPERTIESCOLUMN_Name, categoryParentIndex), QVariant(sName), Qt::UserRole) == false)
		HyGuiLog("PropertiesTreeModel::AppendProperty() - setData failed", LOGTYPE_Error);

	// Set data in the property's value column
	if(setData(index(pNewlyAddedTreeItem->GetIndex(), PROPERTIESCOLUMN_Value, categoryParentIndex), defaultData, Qt::UserRole) == false)
		HyGuiLog("PropertiesTreeModel::AppendProperty() - setData failed", LOGTYPE_Error);

	// If it's checkable, then it is always defaulted to unchecked
	if(eAccessType == PROPERTIESACCESS_ToggleOn)
		eAccessType = PROPERTIESACCESS_ToggleOff;

	// Link this property definition to the proper TreeModelItem using 'm_PropertyDefMap'
	PropertiesDef def(eType, eAccessType, sToolTip, defaultData, minRange, maxRange, stepAmt, sPrefix, sSuffix, delegateBuilder);
	m_PropertyDefMap[pNewlyAddedTreeItem] = def;

	return true;
}

QJsonObject PropertiesTreeModel::SerializeJson()
{
	QJsonObject propertiesObj;

	for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
	{
		if(IsCategoryEnabled(i) == false)
			continue;
		
		TreeModelItem *pCategoryTreeItem = m_pRootItem->GetChild(i);
		QJsonObject categoryObj;
		for(int j = 0; j < pCategoryTreeItem->GetNumChildren(); ++j)
		{
			TreeModelItem *pPropertyItem = pCategoryTreeItem->GetChild(j);

			const PropertiesDef &propDefRef = m_PropertyDefMap[pPropertyItem];
			if(propDefRef.eAccessType == PROPERTIESACCESS_ToggleOff) // Properties that aren't found when deserializing are assumed to be 'PROPERTIESACCESS_ToggleOff' - when re-enabled they will be set to their default value
				continue;

			categoryObj.insert(pPropertyItem->data(PROPERTIESCOLUMN_Name).toString(), ConvertValueToJson(pPropertyItem));
		}
		if(categoryObj.size() > 0)
			propertiesObj.insert(pCategoryTreeItem->data(PROPERTIESCOLUMN_Name).toString(), categoryObj);
	}

	return propertiesObj;
}

void PropertiesTreeModel::DeserializeJson(const QJsonObject &propertiesObj)
{
	// Properties that are "togglable" and aren't found when deserializing are assumed to be 'PROPERTIESACCESS_ToggleOff' - when re-enabled they will be set to their default value
	// Initialize all "togglable" properties to 'PROPERTIESACCESS_ToggleOff'
	for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
	{
		TreeModelItem *pCategoryTreeItem = m_pRootItem->GetChild(i);
		for(int j = 0; j < pCategoryTreeItem->GetNumChildren(); ++j)
		{
			TreeModelItem *pPropertyTreeItem = pCategoryTreeItem->GetChild(j);
			if(m_PropertyDefMap[pPropertyTreeItem].eAccessType == PROPERTIESACCESS_ToggleOn)
				m_PropertyDefMap[pPropertyTreeItem].eAccessType = PROPERTIESACCESS_ToggleOff;
		}
	}

	QStringList sCategoryList = propertiesObj.keys();
	for(const QString &sCategory : sCategoryList)
	{
		for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
		{
			if(0 == m_pRootItem->GetChild(i)->data(PROPERTIESCOLUMN_Name).toString().compare(sCategory, Qt::CaseSensitive))
			{
				if(m_PropertyDefMap[m_pRootItem->GetChild(i)].eAccessType == PROPERTIESACCESS_ToggleOff)
					SetToggle(sCategory, true);
			}
		}

		QJsonObject categoryObj = propertiesObj[sCategory].toObject();
		QStringList sPropertyList = categoryObj.keys();
		for(const QString &sProperty : sPropertyList)
		{
			const PropertiesDef propDef = FindPropertyDefinition(sCategory, sProperty);

			QVariant propValue;
			bool bIsProceduralObj = false;
			if(categoryObj[sProperty].isObject())
			{
				propValue = categoryObj[sProperty].toObject();
				bIsProceduralObj = true;
			}
			else
			{
				switch(propDef.eType)
				{
				case PROPERTIESTYPE_Unknown:
					break;

				case PROPERTIESTYPE_bool:
					propValue = categoryObj[sProperty].toBool() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked;
					break;

				case PROPERTIESTYPE_ComboBoxString:
					if(propDef.delegateBuilder.toStringList().contains(categoryObj[sProperty].toString()) == false)
						HyGuiLog("PropertiesTreeModel::DeserializeJson could not resolve ComboBoxString: " % categoryObj[sProperty].toString(), LOGTYPE_Error);

					propValue = categoryObj[sProperty].toString();
					break;

				case PROPERTIESTYPE_int:
				case PROPERTIESTYPE_ComboBoxInt:
				case PROPERTIESTYPE_StatesComboBox:
				case PROPERTIESTYPE_Slider:
				case PROPERTIESTYPE_SpriteFrames:
					propValue = categoryObj[sProperty].toInt();
					break;

				case PROPERTIESTYPE_int64:
					propValue = categoryObj[sProperty].toVariant().toLongLong();
					break;

				case PROPERTIESTYPE_double:
					propValue = categoryObj[sProperty].toDouble();
					break;

				case PROPERTIESTYPE_ivec2: {
					QJsonArray ivec2Array = categoryObj[sProperty].toArray();
					QPoint ptPoint(ivec2Array[0].toInt(), ivec2Array[1].toInt());
					propValue = ptPoint;
					break; }

				case PROPERTIESTYPE_vec2: {
					QJsonArray vec2Array = categoryObj[sProperty].toArray();
					QPointF ptPoint(vec2Array[0].toDouble(), vec2Array[1].toDouble());
					propValue = ptPoint;
					break; }

				case PROPERTIESTYPE_ivec3:
				case PROPERTIESTYPE_Color: {
					QJsonArray ivec3Array = categoryObj[sProperty].toArray();
					QRect rect(ivec3Array[0].toInt(), ivec3Array[1].toInt(), ivec3Array[2].toInt(), 0);
					propValue = rect;
					break; }

				case PROPERTIESTYPE_vec3: {
					QJsonArray vec3Array = categoryObj[sProperty].toArray();
					QRectF rect(vec3Array[0].toDouble(), vec3Array[1].toDouble(), vec3Array[2].toDouble(), 0.0);
					propValue = rect;
					break; }

				case PROPERTIESTYPE_ivec4: {
					QJsonArray ivec4Array = categoryObj[sProperty].toArray();
					QRect rect(ivec4Array[0].toInt(), ivec4Array[1].toInt(), ivec4Array[2].toInt(), ivec4Array[3].toInt());
					propValue = rect;
					break; }

				case PROPERTIESTYPE_vec4: {
					QJsonArray vec4Array = categoryObj[sProperty].toArray();
					QRectF rect(vec4Array[0].toDouble(), vec4Array[1].toDouble(), vec4Array[2].toDouble(), vec4Array[3].toDouble());
					propValue = rect;
					break; }

				case PROPERTIESTYPE_LineEdit:
					propValue = categoryObj[sProperty].toString();
					break;

				default:
					HyGuiLog("Unhandled PropertiesTreeModel::DeserializeJson property", LOGTYPE_Error);
					break;
				}
			}

			SetPropertyValue(sCategory, sProperty, propValue, bIsProceduralObj);
		}
	}
}


void PropertiesTreeModel::ResetValues()
{
	// Set all values to their default values
	for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
	{
		TreeModelItem *pCategoryTreeItem = m_pRootItem->GetChild(i);

		if(m_PropertyDefMap[pCategoryTreeItem].eAccessType == PROPERTIESACCESS_ToggleOn)
			m_PropertyDefMap[pCategoryTreeItem].eAccessType = PROPERTIESACCESS_ToggleOff;

		for(int j = 0; j < pCategoryTreeItem->GetNumChildren(); ++j)
		{
			TreeModelItem *pPropertyTreeItem = pCategoryTreeItem->GetChild(j);

			if(setData(createIndex(pPropertyTreeItem->GetIndex(), PROPERTIESCOLUMN_Value, pPropertyTreeItem), m_PropertyDefMap[pPropertyTreeItem].defaultData, Qt::UserRole) == false)
				HyGuiLog("PropertiesTreeModel::SetPropertyValue() - setData failed", LOGTYPE_Error);

			if(m_PropertyDefMap[pPropertyTreeItem].eAccessType == PROPERTIESACCESS_ToggleOn)
				m_PropertyDefMap[pPropertyTreeItem].eAccessType = PROPERTIESACCESS_ToggleOff;
		}
	}
}

/*virtual*/ bool PropertiesTreeModel::setData(const QModelIndex &indexRef, const QVariant &valueRef, int iRole /*= Qt::EditRole*/) /*override*/
{
	if(iRole == Qt::UserRole)
		return ITreeModel::setData(indexRef, valueRef, iRole);

	// NOTE: There are three cases to handle here.
	//       1) A normal setData() call that changes the data value in the PROPERTIESCOLUMN_Value When PROPERTIESACCESS_Toggle* is set here, handle it
	//       2) A category that's checkable is changed, which changes the m_PropertyDefMap[]'s 'eAccessType'
	//       3) Check/Uncheck a property that is 'togglable', which changes the m_PropertyDefMap[]'s 'eAccessType'
	if(indexRef.column() == PROPERTIESCOLUMN_Name) // Indicates either case '2' or '3'
	{
		TreeModelItem *pTreeItem = GetItem(indexRef);
		if((m_PropertyDefMap[pTreeItem].eAccessType == PROPERTIESACCESS_ToggleOn && valueRef == Qt::Unchecked) ||
			(m_PropertyDefMap[pTreeItem].eAccessType == PROPERTIESACCESS_ToggleOff && valueRef == Qt::Checked))
		{
			PropertiesUndoCmd *pUndoCmd = AllocateUndoCmd(indexRef, static_cast<bool>(valueRef == Qt::Checked));
			GetOwner().GetUndoStack()->push(pUndoCmd);
		}
	}
	else // PROPERTIESCOLUMN_Value - indicating case '1'
	{
		const QVariant &origValue = GetPropertyValue(indexRef);
		if(origValue != valueRef)
		{
			PropertiesUndoCmd *pUndoCmd = AllocateUndoCmd(indexRef, valueRef);
			GetOwner().GetUndoStack()->push(pUndoCmd);
		}
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
		if(indexRef.column() == PROPERTIESCOLUMN_Name)
			return pTreeItem->data(PROPERTIESCOLUMN_Name);
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
			return QBrush(HyGlobal::ConvertHyColor(propDefRef.GetColor()));
		else if(indexRef.column() == PROPERTIESCOLUMN_Value && propDefRef.eType == PROPERTIESTYPE_Color && propDefRef.eAccessType != PROPERTIESACCESS_ToggleOff)
			return QBrush(QColor(pTreeItem->data(PROPERTIESCOLUMN_Value).toRect().left(), pTreeItem->data(PROPERTIESCOLUMN_Value).toRect().top(), pTreeItem->data(PROPERTIESCOLUMN_Value).toRect().width()));
		//	return QBrush((0 == (pTreeItem->GetIndex() & 1)) ? propDefRef.GetColor() : propDefRef.GetColor().lighter());

	case Qt::ForegroundRole:
		if(propDefRef.IsCategory())
			return QBrush(QColor::fromRgb(255, 255, 255));
		else if(indexRef.column() == PROPERTIESCOLUMN_Value && propDefRef.eType == PROPERTIESTYPE_Color && propDefRef.eAccessType != PROPERTIESACCESS_ToggleOff)
		{
			QColor clr(pTreeItem->data(PROPERTIESCOLUMN_Value).toRect().left(), pTreeItem->data(PROPERTIESCOLUMN_Value).toRect().top(), pTreeItem->data(PROPERTIESCOLUMN_Value).toRect().width());
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
		if(indexRef.column() == 0)
		{
			if(propDefRef.eAccessType == PROPERTIESACCESS_ToggleOn || propDefRef.eAccessType == PROPERTIESACCESS_ToggleOff)
				return propDefRef.eAccessType == PROPERTIESACCESS_ToggleOn ? Qt::Checked : Qt::Unchecked;
		}
		else if(propDefRef.eType == PROPERTIESTYPE_bool && (propDefRef.eAccessType == PROPERTIESACCESS_ToggleOn || propDefRef.eAccessType == PROPERTIESACCESS_Mutable))
			return pTreeItem->data(PROPERTIESCOLUMN_Value);
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
		if(propDefRef.eAccessType != PROPERTIESACCESS_ReadOnly)
			returnFlags |= Qt::ItemIsEnabled;

		if(propDefRef.IsTogglable())
			returnFlags |= Qt::ItemIsUserCheckable | Qt::ItemIsEditable;
	}
	else
	{
		TreeModelItem *pCategoryTreeItem = pTreeItem->GetParent();
		const PropertiesDef &categoryPropDefRef = m_PropertyDefMap[pCategoryTreeItem];
		if(categoryPropDefRef.IsCategory() == false)
			HyGuiLog("PropertiesTreeModel::flags() passed in index is not a category and its parent is not one either", LOGTYPE_Error);

		if(categoryPropDefRef.IsTogglable() == false ||
		   (categoryPropDefRef.IsTogglable() && categoryPropDefRef.eAccessType == PROPERTIESACCESS_ToggleOn))
		{
			if(propDefRef.eAccessType != PROPERTIESACCESS_ReadOnly)
			{
				if(propDefRef.eAccessType == PROPERTIESACCESS_ToggleOn || propDefRef.eAccessType == PROPERTIESACCESS_ToggleOff)
				{
					if(indexRef.column() == PROPERTIESCOLUMN_Name)
						returnFlags |= (Qt::ItemIsUserCheckable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
					else // column is PROPERTIESCOLUMN_Value
					{
						if(propDefRef.eAccessType == PROPERTIESACCESS_ToggleOn)
						{
							returnFlags |= (Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
							if(propDefRef.eType == PROPERTIESTYPE_bool)
								returnFlags |= Qt::ItemIsUserCheckable;
						}
					}
				}
				else // PROPERTIESACCESS_Mutable
				{
					returnFlags |= (Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
					if(propDefRef.eType == PROPERTIESTYPE_bool)
						returnFlags |= Qt::ItemIsUserCheckable;
				}
			}
		}
	}

	return returnFlags;
}

/*virtual*/ void PropertiesTreeModel::OnTreeModelItemRemoved(TreeModelItem *pTreeItem) /*PropertiesTreeModel*/
{
}

/*static*/ QVariant PropertiesTreeModel::ConvertJsonToVariant(PropertiesType ePropType, const QJsonValue &valueRef)
{
	switch(ePropType)
	{
	case PROPERTIESTYPE_bool:
		return QVariant(valueRef.toBool());

	case PROPERTIESTYPE_ComboBoxString:
		return QVariant(valueRef.toString());

	case PROPERTIESTYPE_int:
	case PROPERTIESTYPE_ComboBoxInt:
	case PROPERTIESTYPE_Slider:
	case PROPERTIESTYPE_StatesComboBox:
	case PROPERTIESTYPE_SpriteFrames:
		return QVariant(valueRef.toInt());

	case PROPERTIESTYPE_int64:
		return QVariant(valueRef.toVariant().toLongLong());

	case PROPERTIESTYPE_double:
		return QVariant(valueRef.toDouble());

	case PROPERTIESTYPE_ivec2: {
		QJsonArray ivec2Array = valueRef.toArray();
		return QVariant(QPoint(ivec2Array[0].toInt(), ivec2Array[1].toInt()));
	}

	case PROPERTIESTYPE_vec2: {
		QJsonArray vec2Array = valueRef.toArray();
		return QVariant(QPointF(vec2Array[0].toDouble(), vec2Array[1].toDouble()));
	}

	case PROPERTIESTYPE_ivec3:
	case PROPERTIESTYPE_Color: {
		QJsonArray ivec3Array = valueRef.toArray();
		return QVariant(QRect(ivec3Array[0].toInt(), ivec3Array[1].toInt(), ivec3Array[2].toInt(), 0));
	}

	case PROPERTIESTYPE_vec3: {
		QJsonArray vec3Array = valueRef.toArray();
		return QVariant(QRectF(vec3Array[0].toDouble(), vec3Array[1].toDouble(), vec3Array[2].toDouble(), 0));
	}

	case PROPERTIESTYPE_ivec4: {
		QJsonArray ivec4Array = valueRef.toArray();
		return QVariant(QRect(ivec4Array[0].toInt(), ivec4Array[1].toInt(), ivec4Array[2].toInt(), ivec4Array[3].toInt()));
	}

	case PROPERTIESTYPE_vec4: {
		QJsonArray vec4Array = valueRef.toArray();
		return QVariant(QRectF(vec4Array[0].toDouble(), vec4Array[1].toDouble(), vec4Array[2].toDouble(), vec4Array[3].toDouble()));
	}

	case PROPERTIESTYPE_LineEdit:
		return QVariant(valueRef.toString());

	default:
		HyGuiLog("Unhandled PropertiesTreeModel::ConvertJsonToVariant property type: " % QString::number(ePropType), LOGTYPE_Error);
		break;
	}

	return QVariant();
}

/*static*/ QJsonValue PropertiesTreeModel::ConvertVariantToJson(PropertiesType ePropType, const QVariant &valueRef)
{
	switch(ePropType)
	{
	case PROPERTIESTYPE_bool:
		return QJsonValue(valueRef.toBool());

	case PROPERTIESTYPE_ComboBoxString:
		return QJsonValue(valueRef.toString());

	case PROPERTIESTYPE_int:
	case PROPERTIESTYPE_ComboBoxInt:
	case PROPERTIESTYPE_Slider:
	case PROPERTIESTYPE_StatesComboBox:
	case PROPERTIESTYPE_SpriteFrames:
		return QJsonValue(valueRef.toInt());

	case PROPERTIESTYPE_int64:
		return QJsonValue(valueRef.toLongLong());

	case PROPERTIESTYPE_double:
		return QJsonValue(valueRef.toDouble());

	case PROPERTIESTYPE_ivec2:
		return QJsonValue(QJsonArray() << valueRef.toPoint().x() << valueRef.toPoint().y());

	case PROPERTIESTYPE_vec2:
		return QJsonValue(QJsonArray() << valueRef.toPointF().x() << valueRef.toPointF().y());

	case PROPERTIESTYPE_ivec3:
	case PROPERTIESTYPE_Color:
		return QJsonValue(QJsonArray() << valueRef.toRect().left() << valueRef.toRect().top() << valueRef.toRect().width());

	case PROPERTIESTYPE_vec3:
		return QJsonValue(QJsonArray() << valueRef.toRectF().left() << valueRef.toRectF().top() << valueRef.toRectF().width());

	case PROPERTIESTYPE_ivec4:
		return QJsonValue(QJsonArray() << valueRef.toRect().left() << valueRef.toRect().top() << valueRef.toRect().width() << valueRef.toRect().height());

	case PROPERTIESTYPE_vec4:
		return QJsonValue(QJsonArray() << valueRef.toRectF().left() << valueRef.toRectF().top() << valueRef.toRectF().width() << valueRef.toRectF().height());

	case PROPERTIESTYPE_LineEdit:
		return QJsonValue(valueRef.toString());

	default:
		HyGuiLog("Unhandled PropertiesTreeModel::ConvertVariantToJson property type: " % QString::number(ePropType), LOGTYPE_Error);
		break;
	}

	return QJsonValue();
}

/*virtual*/ PropertiesUndoCmd *PropertiesTreeModel::AllocateUndoCmd(const QModelIndex &index, const QVariant &newData)
{
	return new PropertiesUndoCmd(this, index, newData);
}

QJsonValue PropertiesTreeModel::ConvertValueToJson(TreeModelItem *pTreeItem) const
{
	QVariant propValue = pTreeItem->data(PROPERTIESCOLUMN_Value);
	return ConvertVariantToJson(m_PropertyDefMap[pTreeItem].eType, propValue);
}

QString PropertiesTreeModel::ConvertValueToString(TreeModelItem *pTreeItem) const
{
	const PropertiesDef &propDefRef = m_PropertyDefMap[pTreeItem];
	if(propDefRef.eAccessType == PROPERTIESACCESS_ToggleOff)
		return "<NOT SET>";

	const QVariant &treeItemValue = pTreeItem->data(PROPERTIESCOLUMN_Value);

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
	case PROPERTIESTYPE_int64:
		sRetStr += QString::number(treeItemValue.toLongLong());
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
	case PROPERTIESTYPE_ComboBoxString:
		sRetStr += treeItemValue.toString();
		break;
	case PROPERTIESTYPE_ComboBoxInt:
		sRetStr += propDefRef.delegateBuilder.toStringList()[treeItemValue.toInt()];
		break;
	case PROPERTIESTYPE_StatesComboBox: {
		ProjectItemData *pProjItem = static_cast<ProjectItemData *>(m_OwnerRef.GetProject().FindItemData(propDefRef.delegateBuilder.toUuid()));
		if(pProjItem)
		{
			QComboBox tmpComboBox(nullptr);
			tmpComboBox.setModel(pProjItem->GetModel());
			sRetStr += tmpComboBox.itemText(treeItemValue.toInt());
		}
		else
			HyGuiLog("Project::FindItemData could not find UUID", LOGTYPE_Error);
		break; }
	case PROPERTIESTYPE_Color: {
		QRect rect = treeItemValue.toRect();
		sRetStr += "RGB(" % QString::number(rect.left()) % ", " % QString::number(rect.top()) % ", " % QString::number(rect.width()) % ")";
		break; }
	case PROPERTIESTYPE_Root:
	case PROPERTIESTYPE_Category:
		break;

	default:
		HyGuiLog("PropertiesTreeItem::GetValue not implemented for type: " % QString::number(propDefRef.eType), LOGTYPE_Error);
		break;
	}

	sRetStr += propDefRef.sSuffix;

	return sRetStr;
}
