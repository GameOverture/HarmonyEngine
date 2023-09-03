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

/*virtual*/ void PropertiesTreeModel::SetToggle(const QModelIndex &indexRef, bool bToggleOn)
{
	TreeModelItem *pTreeItem = GetItem(indexRef);
	m_PropertyDefMap[pTreeItem].eAccessType = bToggleOn ? PROPERTIESACCESS_ToggleOn : PROPERTIESACCESS_ToggleOff;
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

/*virtual*/ void PropertiesTreeModel::SetPropertyValue(QString sCategoryName, QString sPropertyName, const QVariant &valueRef)
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

					if(m_PropertyDefMap[pPropertyTreeItem].eAccessType == PROPERTIESACCESS_ToggleOff)
						m_PropertyDefMap[pPropertyTreeItem].eAccessType = PROPERTIESACCESS_ToggleOn;

					return;
				}
			}
		}
	}
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

QString PropertiesTreeModel::GetCategoryName(int iCategoryIndex) const
{
	return m_pRootItem->GetChild(iCategoryIndex)->data(PROPERTIESCOLUMN_Name).toString();
}

bool PropertiesTreeModel::IsCategoryCheckable(int iCategoryIndex) const
{
	const PropertiesDef &categoryPropDefRef = m_PropertyDefMap[m_pRootItem->GetChild(iCategoryIndex)];
	return categoryPropDefRef.eAccessType == PROPERTIESACCESS_ToggleOn || categoryPropDefRef.eAccessType == PROPERTIESACCESS_ToggleOff;
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

bool PropertiesTreeModel::AppendCategory(QString sCategoryName, QVariant commonDelegateBuilder /*= QVariant()*/, bool bCheckable /*= false*/, bool bStartChecked /*= false*/, QString sToolTip /*= ""*/)
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
	else
		def.eAccessType = bStartChecked ? PROPERTIESACCESS_ToggleOn : PROPERTIESACCESS_ToggleOff;

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

	// Link this property definition to the proper TreeModelItem using 'm_PropertyDefMap'
	PropertiesDef def(eType, eAccessType, sToolTip, defaultData, minRange, maxRange, stepAmt, sPrefix, sSuffix, delegateBuilder);
	m_PropertyDefMap[pNewlyAddedTreeItem] = def;

	return true;
}

void PropertiesTreeModel::RefreshCategory(const QModelIndex &indexRef)
{
	TreeModelItem *pCategoryTreeItem = GetItem(indexRef);
	if(pCategoryTreeItem->GetNumChildren() > 0)
	{
		dataChanged(createIndex(0, PROPERTIESCOLUMN_Name, pCategoryTreeItem->GetChild(0)),
					createIndex(pCategoryTreeItem->GetNumChildren() - 1, PROPERTIESCOLUMN_Value, pCategoryTreeItem->GetChild(pCategoryTreeItem->GetNumChildren() - 1)));
	}
}

QJsonObject PropertiesTreeModel::SerializeJson()
{
	QJsonObject propertiesObj;

	for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
	{
		TreeModelItem *pCategoryTreeItem = m_pRootItem->GetChild(i);

		QJsonObject categoryObj;

		// If this category is checkable, then we need to store the checked state (as a boolean) to "<name>_checked"
		if(IsCategoryCheckable(i))
			categoryObj.insert(pCategoryTreeItem->data(PROPERTIESCOLUMN_Name).toString() % "_checked", IsCategoryEnabled(i)); //pCategoryTreeItem->data(PROPERTIESCOLUMN_Value).toInt() == Qt::Checked);
		
		for(int j = 0; j < pCategoryTreeItem->GetNumChildren(); ++j)
		{
			TreeModelItem *pPropertyItem = pCategoryTreeItem->GetChild(j);
			QString sPropName = pPropertyItem->data(PROPERTIESCOLUMN_Name).toString();
			QVariant propValue = pPropertyItem->data(PROPERTIESCOLUMN_Value);

			const PropertiesDef &propDefRef = m_PropertyDefMap[pPropertyItem];

			if(propDefRef.eAccessType == PROPERTIESACCESS_ToggleOff) // Properties that aren't found when deserializing are assumed to be 'PROPERTIESACCESS_ToggleOff' - when re-enabled they will be set to their default value
				continue;

			switch(propDefRef.eType)
			{
			case PROPERTIESTYPE_bool:
				categoryObj.insert(sPropName, propValue.toBool());
				break;
			case PROPERTIESTYPE_ComboBoxString:
				categoryObj.insert(sPropName, propValue.toString());
				break;
			case PROPERTIESTYPE_int:
			case PROPERTIESTYPE_ComboBoxInt:
			case PROPERTIESTYPE_Slider:
			case PROPERTIESTYPE_StatesComboBox:
			case PROPERTIESTYPE_SpriteFrames:
				categoryObj.insert(sPropName, propValue.toInt());
				break;
			case PROPERTIESTYPE_int64:
				categoryObj.insert(sPropName, propValue.toLongLong());
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
			if(m_PropertyDefMap[pPropertyTreeItem].eAccessType == PROPERTIESACCESS_ToggleOn || m_PropertyDefMap[pPropertyTreeItem].eAccessType == PROPERTIESACCESS_ToggleOff)
				m_PropertyDefMap[pPropertyTreeItem].eAccessType = PROPERTIESACCESS_ToggleOff;
		}
	}

	QStringList sCategoryList = propertiesObj.keys();
	for(const QString &sCategory : sCategoryList)
	{
		QJsonObject categoryObj = propertiesObj[sCategory].toObject();

		QStringList sPropertyList = categoryObj.keys();
		if(sPropertyList.contains(sCategory % "_checked"))
		{
			for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
			{
				if(0 == m_pRootItem->GetChild(i)->data(PROPERTIESCOLUMN_Name).toString().compare(sCategory, Qt::CaseSensitive))
				{
					TreeModelItem *pCategoryTreeItem = m_pRootItem->GetChild(i);
					PropertiesDef &categoryPropDefRef = m_PropertyDefMap[pCategoryTreeItem];
					if(categoryPropDefRef.eType != PROPERTIESTYPE_Category)
						HyGuiLog("PropertiesTreeModel::DeserializeJson() - " % sCategory % " is not a category", LOGTYPE_Error);
					else if(categoryPropDefRef.eAccessType != PROPERTIESACCESS_ToggleOn && categoryPropDefRef.eAccessType != PROPERTIESACCESS_ToggleOff)
						HyGuiLog("PropertiesTreeModel::DeserializeJson() - " % sCategory % " is not 'togglable'", LOGTYPE_Error);

					categoryPropDefRef.eAccessType = categoryObj[sCategory % "_checked"].toBool() ? PROPERTIESACCESS_ToggleOn : PROPERTIESACCESS_ToggleOff;
					break;
				}
			}
		}

		for(const QString &sProperty : sPropertyList)
		{
			const PropertiesDef propDef = FindPropertyDefinition(sCategory, sProperty);
			switch(propDef.eType)
			{
			case PROPERTIESTYPE_Unknown:
				break;

			case PROPERTIESTYPE_bool:
				SetPropertyValue(sCategory, sProperty, categoryObj[sProperty].toBool() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
				break;

			case PROPERTIESTYPE_ComboBoxString:
				if(propDef.delegateBuilder.toStringList().contains(categoryObj[sProperty].toString()) == false)
					HyGuiLog("PropertiesTreeModel::DeserializeJson could not resolve ComboBoxString: " % categoryObj[sProperty].toString(), LOGTYPE_Error);

				SetPropertyValue(sCategory, sProperty, categoryObj[sProperty].toString());
				break;

			case PROPERTIESTYPE_int:
			case PROPERTIESTYPE_ComboBoxInt:
			case PROPERTIESTYPE_StatesComboBox:
			case PROPERTIESTYPE_Slider:
			case PROPERTIESTYPE_SpriteFrames:
				SetPropertyValue(sCategory, sProperty, categoryObj[sProperty].toInt());
				break;

			case PROPERTIESTYPE_int64:
				SetPropertyValue(sCategory, sProperty, categoryObj[sProperty].toVariant().toLongLong());
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
			PropertiesUndoCmd *pUndoCmd = new PropertiesUndoCmd(this, indexRef, static_cast<bool>(valueRef == Qt::Checked));
			GetOwner().GetUndoStack()->push(pUndoCmd);
		}
	}
	else // PROPERTIESCOLUMN_Value - indicating case '1'
	{
		const QVariant &origValue = GetPropertyValue(indexRef);
		if(origValue != valueRef)
		{
			PropertiesUndoCmd *pUndoCmd = new PropertiesUndoCmd(this, indexRef, valueRef);
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
			return QBrush(propDefRef.GetColor());
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
		else if(propDefRef.eType == PROPERTIESTYPE_bool)
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
			returnFlags |= Qt::ItemIsUserCheckable;
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
						returnFlags |= (Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
					else // column is PROPERTIESCOLUMN_Value
					{
						if(propDefRef.eAccessType == PROPERTIESACCESS_ToggleOn)
							returnFlags |= (Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
					}
				}
				else // PROPERTIESACCESS_Mutable
					returnFlags |= (Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);

				if(propDefRef.eType == PROPERTIESTYPE_bool)
					returnFlags |= Qt::ItemIsUserCheckable;
			}
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
