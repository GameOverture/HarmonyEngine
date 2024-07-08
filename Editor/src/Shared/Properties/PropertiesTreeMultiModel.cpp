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

PropertiesTreeMultiModel::PropertiesTreeMultiModel(ProjectItemData &ownerRef, int iStateIndex, QVariant subState, const QList<PropertiesTreeModel *> &multiModelListRef, QObject *pParent /*= nullptr*/) :
	PropertiesTreeModel(ownerRef, iStateIndex, subState, pParent),
	m_MultiModelList(multiModelListRef)
{
	if(m_MultiModelList.isEmpty())
	{
		HyGuiLog("PropertiesTreeMultiModel::PropertiesTreeMultiModel() - 'multiModelListRef' is empty", LOGTYPE_Error);
		return;
	}

	// Construct this model only with categories/properties that are shared in all the models within 'm_MultiModelList'

	// Determine which properties are shared between all models and store them in 'sharedPropList'
	PropertiesTreeModel *pModel = m_MultiModelList[0];
	QList<QPair<QString, QString>> sharedPropList = pModel->GetPropertiesList();
	for(PropertiesTreeModel *pCheckAgainst : m_MultiModelList)
	{
		if(pModel == pCheckAgainst)
			continue;

		for(int i = 0; i < sharedPropList.size(); ++i)
		{
			if(pCheckAgainst->GetPropertiesList().contains(sharedPropList[i]) == false)
			{
				sharedPropList.removeAt(i);
				--i; // Decrement 'i' so we don't skip the next element
			}
		}
	}

	// - Append the shared properties to this model
	// - Initialize the checkstate (if toggleable) of the shared properties
	// - Initialize the values of the shared properties. If the values differ between models, set the value to '<different values>'
	for(auto propPair : sharedPropList)
	{
		if(DoesCategoryExist(propPair.first) == false)
		{
			PropertiesDef catDef = pModel->GetCategoryDefinition(propPair.first);
			AppendCategory(propPair.first, catDef.delegateBuilder, catDef.IsToggleable(), catDef.sToolTip);

			if(catDef.IsToggleable())
			{
				Qt::CheckState eCheckState = pModel->GetCategoryDefinition(propPair.first).eAccessType == PROPERTIESACCESS_ToggleChecked ? Qt::Checked : Qt::Unchecked;
				for(int i = 0; i < m_MultiModelList.size(); ++i)
				{
					PropertiesTreeModel *pCheckAgainst = m_MultiModelList[i];
					if(pModel == pCheckAgainst)
						continue;

					Qt::CheckState eCheckAgainstCheckState = pCheckAgainst->GetCategoryDefinition(propPair.first).eAccessType == PROPERTIESACCESS_ToggleChecked ? Qt::Checked : Qt::Unchecked;
					if(eCheckState != eCheckAgainstCheckState)
						eCheckState = Qt::PartiallyChecked;
				}

				SetToggleState(propPair.first, eCheckState);
			}
		}

		PropertiesDef propDef = pModel->FindPropertyDefinition(propPair.first, propPair.second);
		PropertiesAccessType eAccessType = (propDef.IsToggleable()) ? PROPERTIESACCESS_ToggleUnchecked: propDef.eAccessType;
		AppendProperty(propPair.first, propPair.second, propDef.eType, propDef.defaultData, propDef.sToolTip, eAccessType, propDef.minRange, propDef.maxRange, propDef.stepAmt, propDef.sPrefix, propDef.sSuffix, propDef.delegateBuilder);

		QVariant value = pModel->FindPropertyValue(propPair.first, propPair.second);

		Qt::CheckState eCheckState = pModel->FindPropertyDefinition(propPair.first, propPair.second).eAccessType == PROPERTIESACCESS_ToggleChecked ? Qt::Checked : Qt::Unchecked;
		bool bDifferentValues = false;
		for(int i = 0; i < m_MultiModelList.size(); ++i)
		{
			PropertiesTreeModel *pCheckAgainst = m_MultiModelList[i];
			if(pModel == pCheckAgainst)
				continue;

			Qt::CheckState eCheckAgainstCheckState = pCheckAgainst->FindPropertyDefinition(propPair.first, propPair.second).eAccessType == PROPERTIESACCESS_ToggleChecked ? Qt::Checked : Qt::Unchecked;
			if(eCheckState != eCheckAgainstCheckState)
				eCheckState = Qt::PartiallyChecked;

			if(bDifferentValues == false && value != pCheckAgainst->FindPropertyValue(propPair.first, propPair.second))
				bDifferentValues = true;
		}

		if(bDifferentValues)
			SetPropertyAsDifferentValues(propPair.first, propPair.second, eCheckState);
		else
			SetPropertyValue(propPair.first, propPair.second, value, eCheckState);
	}
}

/*virtual*/ PropertiesTreeMultiModel::~PropertiesTreeMultiModel()
{
}
