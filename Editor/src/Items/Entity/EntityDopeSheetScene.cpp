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
#include "EntityWidget.h"
#include "EntityDraw.h"

#include <QGraphicsRectItem>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>

GraphicsKeyFrameItem::GraphicsKeyFrameItem(KeyFrameKey tupleKey, qreal x, qreal y, qreal width, qreal height, QGraphicsItem *parent /*= nullptr*/) :
	QGraphicsRectItem(x, y, width, height, parent),
	m_bIsTweenKeyFrame(false)
{
	setData(DATAKEY_TreeItemData, QVariant::fromValue(std::get<GraphicsKeyFrameItem::DATAKEY_TreeItemData>(tupleKey)));
	setData(DATAKEY_FrameIndex, std::get<GraphicsKeyFrameItem::DATAKEY_FrameIndex>(tupleKey));
	setData(DATAKEY_CategoryPropString, std::get<GraphicsKeyFrameItem::DATAKEY_CategoryPropString>(tupleKey));

	QString sPropString = std::get<GraphicsKeyFrameItem::DATAKEY_CategoryPropString>(tupleKey);
	if(sPropString.split('/')[0].startsWith("Tween "))
	{
		setRect(x + KEYFRAME_WIDTH + 1.0f, y, width, height);
		m_bIsTweenKeyFrame = true;
	}

	setPen(HyGlobal::ConvertHyColor(HyColor::Black));
	setBrush(HyGlobal::ConvertHyColor(m_bIsTweenKeyFrame ? HyColor::Green : HyColor::LightGray));
	setAcceptHoverEvents(true);
	setAcceptedMouseButtons(Qt::LeftButton);
	setFlags(QGraphicsItem::ItemIsSelectable); // Can't use 'QGraphicsItem::ItemIsMovable' because key frames are only allowed to move horizontally and snapped to frames
}

/*virtual*/ GraphicsKeyFrameItem::~GraphicsKeyFrameItem()
{
}

KeyFrameKey GraphicsKeyFrameItem::GetKey() const
{
	return std::make_tuple(data(DATAKEY_TreeItemData).value<EntityTreeItemData *>(),
						   data(DATAKEY_FrameIndex).toInt(),
						   data(DATAKEY_CategoryPropString).toString());
}

bool GraphicsKeyFrameItem::IsTweenKeyFrame() const
{
	return m_bIsTweenKeyFrame;
}

/*virtual*/ QVariant GraphicsKeyFrameItem::itemChange(GraphicsItemChange eChange, const QVariant &value) /*override*/
{
	switch(eChange)
	{
	case QGraphicsItem::ItemSelectedHasChanged:
		if(value.toBool())
			setCursor(Qt::SizeHorCursor);
		else
			unsetCursor();
		break;
	}

	return QGraphicsRectItem::itemChange(eChange, value);
}

/*virtual*/ void GraphicsKeyFrameItem::hoverEnterEvent(QGraphicsSceneHoverEvent *pEvent) /*override*/
{
	setPen(HyGlobal::ConvertHyColor(HyColor::White));
	scene()->update();
}

/*virtual*/ void GraphicsKeyFrameItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *pEvent) /*override*/
{
	setPen(HyGlobal::ConvertHyColor(HyColor::Black));
	scene()->update();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityDopeSheetScene::EntityDopeSheetScene(EntityStateData *pStateData, QJsonObject metaFileObj) :
	QGraphicsScene(),
	m_pEntStateData(pStateData),
	m_iFramesPerSecond(metaFileObj["framesPerSecond"].toInt()),
	m_iCurrentFrame(0)
{
	if(m_iFramesPerSecond == 0)
		m_iFramesPerSecond = 60;

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
		{
			const QJsonObject &propsObj = keyFramesArray[i].toObject()["props"].toObject();
			if(propsObj.isEmpty() == false)
				m_KeyFramesMap[pItemData][keyFramesArray[i].toObject()["frame"].toInt()] = propsObj;
			else
				HyGuiLog("EntityStateData::EntityStateData - item " % iter.key() % " has an empty keyframe at frame " % QString::number(keyFramesArray[i].toObject()["frame"].toInt()), LOGTYPE_Info);
		}
	}

	setBackgroundBrush(HyGlobal::ConvertHyColor(HyColor::WidgetPanel));

	// These lines allow QGraphicsView to align itself to the top-left corner of the scene
	addLine(0.0, 0.0, 10.0, 0.0f)->setAcceptedMouseButtons(Qt::NoButton);
	addLine(0.0, 0.0, 0.0, 10.0f)->setAcceptedMouseButtons(Qt::NoButton);
	m_pCurrentFrameLine = addLine(0.0f, 0.0f, 0.0f, 10.0f); // This line will hide behind the timeline, but is placed in "scene space" so it will embiggen the sceneRect which sets QGraphicsViews' scrollbars accordingly
	m_pCurrentFrameLine->setAcceptedMouseButtons(Qt::NoButton);

	RefreshAllGfxItems();
	SetCurrentFrame(0);
}

/*virtual*/ EntityDopeSheetScene::~EntityDopeSheetScene()
{
}

EntityStateData *EntityDopeSheetScene::GetStateData() const
{
	return m_pEntStateData;
}

int EntityDopeSheetScene::GetFramesPerSecond() const
{
	return m_iFramesPerSecond;
}

void EntityDopeSheetScene::SetFramesPerSecond(int iFramesPerSecond)
{
	m_iFramesPerSecond = iFramesPerSecond;
}

int EntityDopeSheetScene::GetCurrentFrame() const
{
	return m_iCurrentFrame;
}

void EntityDopeSheetScene::SetCurrentFrame(int iFrame)
{
	m_iCurrentFrame = HyMath::Max(iFrame, 0);
	m_pCurrentFrameLine->setPos(TIMELINE_LEFT_MARGIN + (m_iCurrentFrame * TIMELINE_NOTCH_SUBLINES_WIDTH), 0.0f);
	update();

	IWidget *pWidget = m_pEntStateData->GetModel().GetItem().GetWidget();
	if(pWidget)
		static_cast<EntityWidget *>(pWidget)->SetExtrapolatedProperties();

	IDraw *pDraw = m_pEntStateData->GetModel().GetItem().GetDraw();
	if(pDraw)
		static_cast<EntityDraw *>(pDraw)->SetExtrapolatedProperties();
}

const QMap<EntityTreeItemData *, QMap<int, QJsonObject>> &EntityDopeSheetScene::GetKeyFramesMap() const
{
	return m_KeyFramesMap;
}

QList<QPair<QString, QString>> EntityDopeSheetScene::GetUniquePropertiesList(EntityTreeItemData *pItemData) const
{
	QSet<QPair<QString, QString>> uniquePropertiesSet;
	if(m_KeyFramesMap.contains(pItemData) == false)
		return QList<QPair<QString, QString>>();

	QList<QJsonObject> propsObjList = m_KeyFramesMap[pItemData].values();
	for(QJsonObject propsObj : propsObjList)
	{
		QStringList sCategoryList = propsObj.keys();
		for(QString sCategoryName : sCategoryList)
		{

			//// If this category is a tween, skip over all the properties and "insert" a single QPair<> to represent it
			//if(sCategoryName.startsWith("Tween "))
			//{
			//	QString sTween = sCategoryName.mid(6);
			//	QString sCategory;
			//	// Match the tween category to its corresponding property
			//	if(s
			//	uniquePropertiesSet.insert(QPair<QString, QString>(
			//}
			//else
			{
				QJsonObject categoryObj = propsObj[sCategoryName].toObject();
				QStringList sPropList = categoryObj.keys();
				for(QString sPropName : sPropList)
				{
					QPair<QString, QString> newPair(sCategoryName, sPropName);
					uniquePropertiesSet.insert(newPair);
				}
			}
		}
	}

	QList<QPair<QString, QString>> uniquePropertiesList = uniquePropertiesSet.values();

	// Sort 'uniquePropertiesList' to match how it is laid out in the Property Tree View
	EntityPropertiesTreeModel &propertiesModelRef = pItemData->GetPropertiesModel();
	std::sort(uniquePropertiesList.begin(), uniquePropertiesList.end(), [&](const QPair<QString, QString> &a, const QPair<QString, QString> &b) -> bool
	{
		QModelIndex indexA = propertiesModelRef.FindPropertyModelIndex(a.first, a.second);
		QModelIndex indexB = propertiesModelRef.FindPropertyModelIndex(b.first, b.second);

		if(indexA.parent().row() == indexB.parent().row())
			return indexA.row() < indexA.row();
		return indexA.parent().row() < indexB.parent().row();
	});

	return uniquePropertiesList;
}

QJsonArray EntityDopeSheetScene::SerializeAllKeyFrames(EntityTreeItemData *pItemData) const
{
	if(m_KeyFramesMap.contains(pItemData) == false)
		return QJsonArray();

	const QMap<int, QJsonObject> &itemKeyFrameMapRef = m_KeyFramesMap[pItemData];

	QJsonArray serializedKeyFramesArray;
	for(QMap<int, QJsonObject>::const_iterator iter = itemKeyFrameMapRef.begin(); iter != itemKeyFrameMapRef.end(); ++iter)
	{
		if(iter.value().isEmpty())
			continue;

		QJsonObject serializedKeyFramesObj;
		serializedKeyFramesObj["frame"] = iter.key();
		serializedKeyFramesObj["props"] = iter.value();
		serializedKeyFramesArray.append(serializedKeyFramesObj);
	}

	return serializedKeyFramesArray;
}

QJsonObject EntityDopeSheetScene::GetKeyFrameProperties(EntityTreeItemData *pItemData) const
{
	if(m_KeyFramesMap.contains(pItemData) == false)
		return QJsonObject();

	const QMap<int, QJsonObject> &itemKeyFrameMapRef = m_KeyFramesMap[pItemData];
	QMap<int, QJsonObject>::const_iterator iter = itemKeyFrameMapRef.find(m_iCurrentFrame);
	if(iter == itemKeyFrameMapRef.end())
		return QJsonObject();

	return iter.value();
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
		// lowerBound() - Returns an iterator pointing to the first item with key 'm_iCurrentFrame' in the map.
		//                If the map contains no item with key 'm_iCurrentFrame', the function returns an iterator
		//                to the nearest item with a greater key.
		iter = itemKeyFrameMapRef.lowerBound(m_iCurrentFrame);
		if(iter != itemKeyFrameMapRef.begin())
			iter--; // Don't want an iterator with a greater key, so go back one
	}

	// Starting with this key frame and going backwards in time, combine any properties from key frames that *haven't been set yet*
	// This creates an 'extrapolatedPropObj' that contains all the properties that have been set from the beginning of the timeline, up to 'm_iCurrentFrame'
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

	if(itemKeyFrameMapRef[iFrameIndex].contains(sCategoryName) == false)
		return QJsonValue();

	if(itemKeyFrameMapRef[iFrameIndex][sCategoryName].toObject().contains(sPropName) == false)
		return QJsonValue();

	return itemKeyFrameMapRef[iFrameIndex][sCategoryName].toObject()[sPropName];
}

QJsonValue EntityDopeSheetScene::ExtrapolateKeyFrameProperty(EntityTreeItemData *pItemData, QString sCategoryName, QString sPropName) const
{
	if(m_KeyFramesMap.contains(pItemData) == false)
		return QJsonValue();

	const QMap<int, QJsonObject> &itemKeyFrameMapRef = m_KeyFramesMap[pItemData];

	// Get the closest key frame that is less than or equal to 'm_iCurrentFrame'
	QMap<int, QJsonObject>::const_iterator iter = itemKeyFrameMapRef.find(m_iCurrentFrame);
	if(iter == itemKeyFrameMapRef.end())
	{
		// lowerBound() - Returns an iterator pointing to the first item with key 'm_iCurrentFrame' in the map.
		//                If the map contains no item with key 'm_iCurrentFrame', the function returns an iterator
		//                to the nearest item with a greater key.
		iter = itemKeyFrameMapRef.lowerBound(m_iCurrentFrame);
		if(iter != itemKeyFrameMapRef.begin())
			iter--; // Don't want an iterator with a greater key, so go back one
	}

	if(iter == itemKeyFrameMapRef.end())
		return QJsonValue();

	// Starting with this key frame and going backwards in time, search for the property 'sCategoryName/sPropName' and return the value
	while(true)
	{
		QJsonObject curKeyFrameObj = iter.value();
		if(curKeyFrameObj.contains(sCategoryName) && curKeyFrameObj[sCategoryName].toObject().contains(sPropName))
			return curKeyFrameObj[sCategoryName].toObject()[sPropName];

		if(iter == itemKeyFrameMapRef.begin())
			break;
		iter--;
	}

	return QJsonValue();
}

void EntityDopeSheetScene::SetKeyFrameProperties(EntityTreeItemData *pItemData, int iFrameIndex, QJsonObject propsObj)
{
	QJsonObject curPropsObj = m_KeyFramesMap[pItemData][iFrameIndex];

	if(curPropsObj.empty() && propsObj.empty())
	{
		m_KeyFramesMap[pItemData].remove(iFrameIndex);
		return;
	}

	// Merge 'propsObj' into 'curPropsObj', overwrite any properties that are already set, but preserve any properties in that aren't
	for(auto iterCategory = propsObj.begin(); iterCategory != propsObj.end(); ++iterCategory)
	{
		// Check if category exists
		if(curPropsObj.contains(iterCategory.key()))
		{
			// Category already exists, take existing category ('curPropsObj') object and merge it with passed-in 'iterCategory'
			QJsonObject curCategoryObj = curPropsObj[iterCategory.key()].toObject();
			QJsonObject newCategoryObj = iterCategory.value().toObject();
			for(auto iterProp = newCategoryObj.begin(); iterProp != newCategoryObj.end(); ++iterProp)
				curCategoryObj.insert(iterProp.key(), iterProp.value()); // Update the category with new properties
			
			curPropsObj.insert(iterCategory.key(), curCategoryObj);
		}
		else
			curPropsObj.insert(iterCategory.key(), iterCategory.value());
	}

	if(curPropsObj.empty())
		HyGuiLog("EntityDopeSheetScene::SetKeyFrameProperties() - curPropsObj is empty", LOGTYPE_Error);

	m_KeyFramesMap[pItemData][iFrameIndex] = curPropsObj;

	RefreshAllGfxItems();
}

bool EntityDopeSheetScene::SetKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName, QJsonValue jsonValue, bool bRefreshGfxItems)
{
	bool bIsNewKeyFrame = false;

	if(m_KeyFramesMap.contains(pItemData) == false)
		m_KeyFramesMap.insert(pItemData, QMap<int, QJsonObject>());

	QMap<int, QJsonObject> &itemKeyFrameMapRef = m_KeyFramesMap[pItemData];
	if(itemKeyFrameMapRef.contains(iFrameIndex) == false)
		itemKeyFrameMapRef.insert(iFrameIndex, QJsonObject());

	QJsonObject &keyFrameObjRef = itemKeyFrameMapRef[iFrameIndex];
	if(keyFrameObjRef.contains(sCategoryName) == false)
	{
		keyFrameObjRef.insert(sCategoryName, QJsonObject());
		bIsNewKeyFrame = true;
	}
	else if(keyFrameObjRef[sCategoryName].toObject().contains(sPropName) == false)
		bIsNewKeyFrame = true;

	QJsonObject categoryObj = keyFrameObjRef[sCategoryName].toObject();
	categoryObj.insert(sPropName, jsonValue);

	keyFrameObjRef.insert(sCategoryName, categoryObj);

	if(bRefreshGfxItems)
		RefreshAllGfxItems();

	return bIsNewKeyFrame;
}

void EntityDopeSheetScene::RemoveKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName, bool bRefreshGfxItems)
{
	if(m_KeyFramesMap.contains(pItemData) == false || m_KeyFramesMap[pItemData].contains(iFrameIndex) == false)
		return;

	QJsonObject &keyFrameObjRef = m_KeyFramesMap[pItemData][iFrameIndex];
	if(keyFrameObjRef.empty())
	{
		m_KeyFramesMap[pItemData].remove(iFrameIndex);
		return;
	}

	if(keyFrameObjRef.contains(sCategoryName) == false)
		return;

	QJsonObject categoryObj = keyFrameObjRef[sCategoryName].toObject();
	categoryObj.remove(sPropName);

	if(categoryObj.isEmpty())
	{
		keyFrameObjRef.remove(sCategoryName);
		if(keyFrameObjRef.isEmpty())
		{
			m_KeyFramesMap[pItemData].remove(iFrameIndex);
			if(m_KeyFramesMap[pItemData].isEmpty())
				m_KeyFramesMap.remove(pItemData);
		}
	}
	else
		keyFrameObjRef.insert(sCategoryName, categoryObj);

	// Remove the corresponding gfx rect for this property
	auto gfxRectMapKey = std::make_tuple(pItemData, iFrameIndex, sCategoryName % "/" % sPropName);
	if(m_KeyFramesGfxRectMap.contains(gfxRectMapKey))
	{
		removeItem(m_KeyFramesGfxRectMap[gfxRectMapKey]);
		m_KeyFramesGfxRectMap.remove(gfxRectMapKey);
	}

	if(bRefreshGfxItems)
		RefreshAllGfxItems();
}

void EntityDopeSheetScene::NudgeKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName, int iNudgeAmount, bool bRefreshGfxItems)
{
	QJsonValue propValue = GetKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, sPropName);
	RemoveKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, sPropName, false);
	SetKeyFrameProperty(pItemData, HyMath::Max(0, iFrameIndex + iNudgeAmount), sCategoryName, sPropName, propValue, bRefreshGfxItems);
}

void EntityDopeSheetScene::SelectAllItemKeyFrames(EntityTreeItemData *pItemData)
{
	clearSelection();

	const QMap<int, QJsonObject> &itemKeyFrameMapRef = m_KeyFramesMap[pItemData];
	for(QMap<int, QJsonObject>::const_iterator iter = itemKeyFrameMapRef.begin(); iter != itemKeyFrameMapRef.end(); ++iter)
	{
		int iFrameIndex = iter.key();
		QJsonObject propsObj = iter.value();

		QStringList sCategoryList = propsObj.keys();
		for(QString sCategoryName : sCategoryList)
		{
			QJsonObject categoryObj = propsObj[sCategoryName].toObject();
			QStringList sPropList = categoryObj.keys();
			for(QString sPropName : sPropList)
			{
				auto gfxRectMapKey = std::make_tuple(pItemData, iFrameIndex, sCategoryName % "/" % sPropName);
				if(m_KeyFramesGfxRectMap.contains(gfxRectMapKey))
					m_KeyFramesGfxRectMap[gfxRectMapKey]->setSelected(true);
			}
		}
	}
}

void EntityDopeSheetScene::RefreshAllGfxItems()
{
	// Gather all the entity items (root, children, shapes) into one list 'itemList'
	QList<EntityTreeItemData *> entireItemList, shapeList;
	static_cast<EntityModel &>(m_pEntStateData->GetModel()).GetTreeModel().GetTreeItemData(entireItemList, shapeList);
	entireItemList += shapeList;
	entireItemList.prepend(static_cast<EntityModel &>(m_pEntStateData->GetModel()).GetTreeModel().GetRootTreeItemData());

	qreal fPosY = TIMELINE_HEIGHT + 2.0f;
	for(EntityTreeItemData *pCurItemData : entireItemList)
	{
		// 'uniquePropList' will contain every row of key frames for this item, across all frames
		QList<QPair<QString, QString>> uniquePropList = GetUniquePropertiesList(pCurItemData);

		// - If NOT selected, draw all the key frames in one row (its name row)
		// - If selected, skip the name row and then draw each property in its own row
		if(pCurItemData->IsSelected())
			fPosY += ITEMS_LINE_HEIGHT; // skip the name row

		if(m_KeyFramesMap.contains(pCurItemData))
		{
			const QMap<int, QJsonObject> &keyFrameMapRef = m_KeyFramesMap[pCurItemData];
			for(int iFrameIndex : keyFrameMapRef.keys())
			{
				const float fITEM_START_POSY = fPosY;
				QJsonObject propsObj = keyFrameMapRef[iFrameIndex];

				// Iterate through 'uniquePropList' and draw a QGraphicsRectItem for each property found in 'propsObj'
				for(auto &propPair : uniquePropList)
				{
					if(propsObj.contains(propPair.first) == false || propsObj[propPair.first].toObject().contains(propPair.second) == false)
					{
						if(pCurItemData->IsSelected())
							fPosY += ITEMS_LINE_HEIGHT;
						continue;
					}

					// IMPORTANT NOTE: std::make_tuple() crashes in Release if you try to construct the string directly in the function call
					QString sPropString = propPair.first % "/" % propPair.second;
					auto gfxRectMapKey = std::make_tuple(pCurItemData, iFrameIndex, sPropString);

					qreal fPosX = TIMELINE_LEFT_MARGIN + (iFrameIndex * TIMELINE_NOTCH_SUBLINES_WIDTH) - 2.0f;
					if(m_KeyFramesGfxRectMap.contains(gfxRectMapKey) == false)
					{
						GraphicsKeyFrameItem *pNewGfxRectItem = new GraphicsKeyFrameItem(gfxRectMapKey, 0.0f, 0.0f, KEYFRAME_WIDTH, KEYFRAME_HEIGHT);
						pNewGfxRectItem->setPos(fPosX, fPosY);

						m_KeyFramesGfxRectMap[gfxRectMapKey] = pNewGfxRectItem;
						addItem(pNewGfxRectItem);
					}
					else
						m_KeyFramesGfxRectMap[gfxRectMapKey]->setPos(fPosX, fPosY);

					if(pCurItemData->IsSelected())
						fPosY += ITEMS_LINE_HEIGHT;
				}

				fPosY = fITEM_START_POSY;
			}
		}

		if(pCurItemData->IsSelected() == false)
			fPosY += ITEMS_LINE_HEIGHT; // Move past the name row
		else
			fPosY += uniquePropList.size() * ITEMS_LINE_HEIGHT; // Already accounted for the name row, now move past the property rows

		fPosY += 1.0f; // Account for the space between items
	}

	update();
}

/*virtual*/ void EntityDopeSheetScene::mousePressEvent(QGraphicsSceneMouseEvent *pMouseEvent) /*override*/
{
	if(pMouseEvent->buttons() & Qt::RightButton)
		pMouseEvent->accept(); // Eat right click to avoid deselecting items
	else
		QGraphicsScene::mousePressEvent(pMouseEvent);
}
