/**************************************************************************
*	EntityDopeSheetScene.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2023 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "EntityDopeSheetScene.h"
#include "EntityModel.h"

#include <QGraphicsRectItem>


EntityDopeSheetScene::EntityDopeSheetScene(EntityStateData *pStateData, QJsonObject metaFileObj) :
	QGraphicsScene(),
	m_pEntStateData(pStateData),
	m_iFramesPerSecond(metaFileObj["framesPerSecond"].toInt()),
	m_iCurrentFrame(0),
	m_fZoom(1.0f)
{
	QJsonObject keyFramesObj = metaFileObj["keyFrames"].toObject();
	for(auto iter = keyFramesObj.begin(); iter != keyFramesObj.end(); ++iter)
	{
		EntityTreeItemData *pItemData = static_cast<EntityModel &>(m_pEntStateData->GetModel()).GetTreeModel().FindTreeItemData(QUuid(iter.key()));
		if(pItemData == nullptr)
		{
			HyGuiLog("EntityStateData::EntityStateData - item " % iter.key() % " was not found", LOGTYPE_Error);
			continue;
		}

		QJsonArray keyFramesArray = iter.value().toArray();
		for(int i = 0; i < keyFramesArray.size(); ++i)
			m_KeyFramesMap[pItemData][keyFramesArray[i].toObject()["frame"].toInt()] = keyFramesArray[i].toObject()["props"].toObject();
	}

	setBackgroundBrush(HyGlobal::CovertHyColor(HyColor::ContainerFrame));

	UpdateSceneItems();
}

/*virtual*/ EntityDopeSheetScene::~EntityDopeSheetScene()
{
}

int EntityDopeSheetScene::GetFramesPerSecond() const
{
	return m_iFramesPerSecond;
}

void EntityDopeSheetScene::SetFramesPerSecond(int iFramesPerSecond)
{
	m_iFramesPerSecond = iFramesPerSecond;

	UpdateSceneItems();
}

int EntityDopeSheetScene::GetCurrentFrame() const
{
	return m_iCurrentFrame;
}

float EntityDopeSheetScene::GetZoom() const
{
	return m_fZoom;
}

const QMap<EntityTreeItemData *, QMap<int, QJsonObject>> &EntityDopeSheetScene::GetKeyFramesMap() const
{
	return m_KeyFramesMap;
}

QJsonArray EntityDopeSheetScene::SerializeAllKeyFrames(EntityTreeItemData *pItemData) const
{
	if(m_KeyFramesMap.contains(pItemData) == false)
		return QJsonArray();

	const QMap<int, QJsonObject> &itemKeyFrameMapRef = m_KeyFramesMap[pItemData];

	QJsonArray serializedKeyFramesArray;
	for(QMap<int, QJsonObject>::const_iterator iter = itemKeyFrameMapRef.begin(); iter != itemKeyFrameMapRef.end(); ++iter)
	{
		QJsonObject serializedKeyFramesObj;
		serializedKeyFramesObj["frame"] = iter.key();
		serializedKeyFramesObj["props"] = iter.value();
		serializedKeyFramesArray.append(serializedKeyFramesObj);
	}

	return serializedKeyFramesArray;
}

QJsonObject EntityDopeSheetScene::ExtrapolateKeyFramesProperties(EntityTreeItemData *pItemData) const
{
	if(m_KeyFramesMap.contains(pItemData) == false)
		return QJsonObject();

	const QMap<int, QJsonObject> &itemKeyFrameMapRef = m_KeyFramesMap[pItemData];

	// Get the closest key frame that is less than or equal to 'm_iCurrentFrame'
	QMap<int, QJsonObject>::const_iterator iter = itemKeyFrameMapRef.find(m_iCurrentFrame);
	if(iter == itemKeyFrameMapRef.end())
	{
		iter = itemKeyFrameMapRef.lowerBound(m_iCurrentFrame);
		if(iter != itemKeyFrameMapRef.begin())
			iter--;
	}

	// TODO: Need to extrapolate any tweens that are currently active on this frame



	// Starting with this key frame and going backwards in time, combine any properties from key frames that haven't been set yet
	// This creates an 'extrapolatedPropObj' that contains all the properties that have been set from the beginning of the timeline, up to 'iFrameIndex'
	QJsonObject extrapolatedPropObj = iter.value();
	while(iter != itemKeyFrameMapRef.begin())
	{
		iter--;
		QJsonObject curKeyFrameObj = iter.value();
		for(QString sCategoryName : curKeyFrameObj.keys())
		{
			if(extrapolatedPropObj.contains(sCategoryName) == false)
				extrapolatedPropObj.insert(sCategoryName, curKeyFrameObj[sCategoryName]);
			else
			{
				QJsonObject extrapolatedCategoryObj = extrapolatedPropObj[sCategoryName].toObject();
				QJsonObject curCategoryObj = curKeyFrameObj[sCategoryName].toObject();

				for(QString sPropName : curCategoryObj.keys())
				{
					if(extrapolatedCategoryObj.contains(sPropName) == false)
						extrapolatedCategoryObj.insert(sPropName, curCategoryObj[sPropName]);
				}

				extrapolatedPropObj.insert(sCategoryName, extrapolatedCategoryObj);
			}
		}
	}

	return extrapolatedPropObj;
}

QJsonValue EntityDopeSheetScene::GetKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName) const
{
	if(m_KeyFramesMap.contains(pItemData) == false)
		return QJsonValue();

	const QMap<int, QJsonObject> &itemKeyFrameMapRef = m_KeyFramesMap[pItemData];
	if(itemKeyFrameMapRef.contains(iFrameIndex) == false)
		return QJsonValue();

	if(itemKeyFrameMapRef[iFrameIndex].contains("sCategoryName") == false)
		return QJsonValue();

	if(itemKeyFrameMapRef[iFrameIndex]["sCategoryName"].toObject().contains(sPropName) == false)
		return QJsonValue();

	return itemKeyFrameMapRef[iFrameIndex]["sCategoryName"].toObject()[sPropName];
}

void EntityDopeSheetScene::SetKeyFrameProperties(EntityTreeItemData *pItemData, int iFrameIndex, QJsonObject propsObj)
{
	QJsonObject curPropsObj = m_KeyFramesMap[pItemData][iFrameIndex];

	// Merge 'propsObj' into 'curPropsObj', overwrite any properties that are already set, but preserve any properties in that aren't
	for(auto iterCategory = propsObj.begin(); iterCategory != propsObj.end(); ++iterCategory)
	{
		// Check if category exists
		if(curPropsObj.contains(iterCategory.key()))
		{
			QJsonObject curCategoryObj = curPropsObj[iterCategory.key()].toObject();
			QJsonObject newCategoryObj = iterCategory.value().toObject();
			for(auto iterProp = newCategoryObj.begin(); iterProp != newCategoryObj.end(); ++iterProp)
				curCategoryObj.insert(iterProp.key(), iterProp.value()); // Update the category with new properties
			
			curPropsObj.insert(iterCategory.key(), curCategoryObj);
		}
		else
			curPropsObj.insert(iterCategory.key(), iterCategory.value());
	}

	m_KeyFramesMap[pItemData][iFrameIndex] = curPropsObj;

	UpdateSceneItems();
}

void EntityDopeSheetScene::SetKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName, QJsonValue jsonValue)
{
	if(m_KeyFramesMap.contains(pItemData) == false)
		m_KeyFramesMap.insert(pItemData, QMap<int, QJsonObject>());

	QMap<int, QJsonObject> &itemKeyFrameMapRef = m_KeyFramesMap[pItemData];
	if(itemKeyFrameMapRef.contains(iFrameIndex) == false)
		itemKeyFrameMapRef.insert(iFrameIndex, QJsonObject());

	QJsonObject &keyFrameObjRef = itemKeyFrameMapRef[iFrameIndex];
	if(keyFrameObjRef.contains(sCategoryName) == false)
		keyFrameObjRef.insert(sCategoryName, QJsonObject());

	QJsonObject categoryObj = keyFrameObjRef[sCategoryName].toObject();
	categoryObj.insert(sPropName, jsonValue);

	keyFrameObjRef.insert(sCategoryName, categoryObj);

	UpdateSceneItems();
}

void EntityDopeSheetScene::UpdateSceneItems()
{
	QRectF rect = sceneRect();
}
