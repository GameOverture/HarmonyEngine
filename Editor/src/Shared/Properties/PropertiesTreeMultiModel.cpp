/**************************************************************************
*	PropertiesTreeMultiModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2023 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "PropertiesTreeMultiModel.h"

PropertiesTreeMultiModel::PropertiesTreeMultiModel(ProjectItemData &ownerRef, int iStateIndex, QVariant subState, const QList<PropertiesTreeModel *> &multiModelListRef, QList<QJsonObject> multiPropsObjList, QObject *pParent /*= nullptr*/) :
	PropertiesTreeModel(ownerRef, iStateIndex, subState, pParent),
	m_MultiModelList(multiModelListRef)
{
	if(m_MultiModelList.isEmpty())
	{
		HyGuiLog("PropertiesTreeMultiModel::PropertiesTreeMultiModel() - 'multiModelListRef' is empty", LOGTYPE_Error);
		return;
	}
	if(multiPropsObjList.size() != m_MultiModelList.size())
	{
		HyGuiLog("PropertiesTreeMultiModel::PropertiesTreeMultiModel() - 'multiPropsObjList' size does not match 'multiModelListRef' size", LOGTYPE_Error);
		return;
	}

	// Construct this model only with categories/properties that are shared in all the models within 'm_MultiModelList'

	// Determine which properties are shared between all models
	PropertiesTreeModel *pModel = m_MultiModelList[0];
	QList<QPair<QString, QString>> propList = pModel->GetPropertiesList();
	for(PropertiesTreeModel *pCheckAgainst : m_MultiModelList)
	{
		if(pModel == pCheckAgainst)
			continue;

		for(int i = 0; i < propList.size(); ++i)
		{
			if(pCheckAgainst->GetPropertiesList().contains(propList[i]) == false)
			{
				propList.removeAt(i);
				--i; // Decrement 'i' so we don't skip the next element
			}
		}
	}

	// Append the shared properties to this model
	for(auto propPair : propList)
	{
		if(DoesCategoryExist(propPair.first) == false)
		{
			PropertiesDef catDef = pModel->GetCategoryDefinition(propPair.first);
			AppendCategory(propPair.first, catDef.delegateBuilder, catDef.IsTogglable(), catDef.sToolTip);
		}

		PropertiesDef propDef = pModel->FindPropertyDefinition(propPair.first, propPair.second);
		PropertiesAccessType eAccessType = (propDef.IsTogglable()) ? PROPERTIESACCESS_ToggleOff : propDef.eAccessType;
		AppendProperty(propPair.first, propPair.second, propDef.eType, propDef.defaultData, propDef.sToolTip, eAccessType, propDef.minRange, propDef.maxRange, propDef.stepAmt, propDef.sPrefix, propDef.sSuffix, propDef.delegateBuilder);
	}

	// Initialize the values of the shared properties. If the values differ between models, set the value to '<different values>'
	for(auto propPair : propList)
	{
		bool bChecked = pModel->FindPropertyDefinition(propPair.first, propPair.second).eAccessType != PROPERTIESACCESS_ToggleOff;

		QVariant value = pModel->FindPropertyValue(propPair.first, propPair.second);
		bool bDifferentValues = false;
		for(int i = 0; i < m_MultiModelList.size(); ++i)
		{
			PropertiesTreeModel *pCheckAgainst = m_MultiModelList[i];
			QJsonObject propsObj = multiPropsObjList[i];

			if(pModel == pCheckAgainst)
				continue;

			bChecked = bChecked || pCheckAgainst->FindPropertyDefinition(propPair.first, propPair.second).eAccessType != PROPERTIESACCESS_ToggleOff;

			if(value != pCheckAgainst->FindPropertyValue(propPair.first, propPair.second))
			{
				bDifferentValues = true;
				break;
			}
		}
		if(bDifferentValues)
			SetPropertyAsDifferentValues(propPair.first, propPair.second);
		else if(bChecked)
			SetPropertyValue(propPair.first, propPair.second, value, false);
	}
}

/*virtual*/ PropertiesTreeMultiModel::~PropertiesTreeMultiModel()
{
}
