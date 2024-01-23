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

GraphicsTweenKnobItem::GraphicsTweenKnobItem(KeyFrameKey tupleKey, QGraphicsItem *pParent /*= nullptr*/) :
	QGraphicsEllipseItem(-KEYFRAME_TWEEN_KNOB_RADIUS, -KEYFRAME_TWEEN_KNOB_RADIUS, KEYFRAME_TWEEN_KNOB_RADIUS * 2.0f, KEYFRAME_TWEEN_KNOB_RADIUS * 2.0f, pParent)
{
	setData(GFXDATAKEY_TreeItemData, QVariant::fromValue(std::get<GFXDATAKEY_TreeItemData>(tupleKey)));
	setData(GFXDATAKEY_FrameIndex, std::get<GFXDATAKEY_FrameIndex>(tupleKey));
	setData(GFXDATAKEY_CategoryPropString, std::get<GFXDATAKEY_CategoryPropString>(tupleKey));

	setData(GFXDATAKEY_Type, GFXITEM_TweenKnob);

	setPen(HyGlobal::ConvertHyColor(HyColor::Black));
	setBrush(HyGlobal::ConvertHyColor(HyColor::Green));
	setAcceptHoverEvents(true);
	setAcceptedMouseButtons(Qt::LeftButton);
	setFlags(QGraphicsItem::ItemIsSelectable);
}

/*virtual*/ GraphicsTweenKnobItem::~GraphicsTweenKnobItem()
{
}

KeyFrameKey GraphicsTweenKnobItem::GetKey() const
{
	return std::make_tuple(data(GFXDATAKEY_TreeItemData).value<EntityTreeItemData *>(),
						   data(GFXDATAKEY_FrameIndex).toInt(),
						   data(GFXDATAKEY_CategoryPropString).toString());
}

/*virtual*/ QVariant GraphicsTweenKnobItem::itemChange(GraphicsItemChange eChange, const QVariant &value) /*override*/
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

	return QGraphicsEllipseItem::itemChange(eChange, value);
}

/*virtual*/ void GraphicsTweenKnobItem::hoverEnterEvent(QGraphicsSceneHoverEvent *pEvent) /*override*/
{
	setPen(HyGlobal::ConvertHyColor(HyColor::White));
	scene()->update();
}

/*virtual*/ void GraphicsTweenKnobItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *pEvent) /*override*/
{
	setPen(HyGlobal::ConvertHyColor(HyColor::Black));
	scene()->update();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GraphicsKeyFrameItem::GraphicsKeyFrameItem(KeyFrameKey tupleKey, bool bIsTweenKeyFrame, QGraphicsItem *pParent /*= nullptr*/) :
	QGraphicsRectItem(0.0, 0.0, KEYFRAME_WIDTH, KEYFRAME_HEIGHT, pParent),
	m_pGfxTweenLine(nullptr),
	m_pGfxTweenDurationKnob(nullptr),
	m_iTweenFramesDuration(0)
{
	setData(GFXDATAKEY_TreeItemData, QVariant::fromValue(std::get<GFXDATAKEY_TreeItemData>(tupleKey)));
	setData(GFXDATAKEY_FrameIndex, std::get<GFXDATAKEY_FrameIndex>(tupleKey));
	setData(GFXDATAKEY_CategoryPropString, std::get<GFXDATAKEY_CategoryPropString>(tupleKey));

	setData(GFXDATAKEY_Type, bIsTweenKeyFrame ? GFXITEM_TweenKeyFrame : GFXITEM_PropertyKeyFrame);

	setPen(HyGlobal::ConvertHyColor(HyColor::Black));
	setBrush(HyGlobal::ConvertHyColor(bIsTweenKeyFrame ? HyColor::Green : HyColor::LightGray));
	setAcceptHoverEvents(true);
	setAcceptedMouseButtons(Qt::LeftButton);
	setFlags(QGraphicsItem::ItemIsSelectable); // Can't use 'QGraphicsItem::ItemIsMovable' because key frames are only allowed to move horizontally and snapped to frames

	if(bIsTweenKeyFrame)
	{
		m_pGfxTweenLine = new QGraphicsLineItem(this);
		m_pGfxTweenLine->setFlag(QGraphicsItem::ItemStacksBehindParent);

		QPen dashLinePen;
		dashLinePen.setStyle(Qt::DashLine);
		dashLinePen.setColor(HyGlobal::ConvertHyColor(HyColor::Green));
		m_pGfxTweenLine->setPen(dashLinePen);

		m_pGfxTweenDurationKnob = new GraphicsTweenKnobItem(tupleKey, this);
	}
}

/*virtual*/ GraphicsKeyFrameItem::~GraphicsKeyFrameItem()
{
	delete m_pGfxTweenDurationKnob;
	delete m_pGfxTweenLine;
}

KeyFrameKey GraphicsKeyFrameItem::GetKey() const
{
	return std::make_tuple(data(GFXDATAKEY_TreeItemData).value<EntityTreeItemData *>(),
						   data(GFXDATAKEY_FrameIndex).toInt(),
						   data(GFXDATAKEY_CategoryPropString).toString());
}

bool GraphicsKeyFrameItem::IsTweenKeyFrame() const
{
	return data(GFXDATAKEY_Type).toInt() == GFXITEM_TweenKeyFrame;
}

void GraphicsKeyFrameItem::SetTweenLineLength(qreal fLength, int iFramesDuration)
{
	m_pGfxTweenLine->setLine(0.0f, KEYFRAME_HEIGHT * 0.5f, fLength, KEYFRAME_HEIGHT * 0.5f);
	m_pGfxTweenDurationKnob->setPos(fLength, KEYFRAME_HEIGHT * 0.5f);
	m_iTweenFramesDuration = iFramesDuration;
}

int GraphicsKeyFrameItem::GetTweenFramesDuration() const
{
	return m_iTweenFramesDuration;
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
	m_iCurrentFrame(0)
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
		{
			const QJsonObject &propsObj = keyFramesArray[i].toObject()["props"].toObject();
			if(propsObj.isEmpty() == false)
				m_KeyFramesMap[pItemData][keyFramesArray[i].toObject()["frame"].toInt()] = propsObj;
			else
				HyGuiLog("EntityStateData::EntityStateData - item " % iter.key() % " has an empty keyframe at frame " % QString::number(keyFramesArray[i].toObject()["frame"].toInt()), LOGTYPE_Info);
		}
	}

	QJsonArray callbackArray = metaFileObj["callbacks"].toArray();
	for(int i = 0; i < callbackArray.size(); ++i)
	{
		const QJsonObject &callbackObj = callbackArray[i].toObject();
		CreateCallback(callbackObj["frame"].toInt(), callbackObj["function"].toString());
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

const QMap<int, QString> &EntityDopeSheetScene::GetCallbackMap() const
{
	return m_CallbackMap;
}

bool EntityDopeSheetScene::ContainsKeyFrameProperty(KeyFrameKey tupleKey)
{
	return m_KeyFramesGfxRectMap.contains(tupleKey);
}

bool EntityDopeSheetScene::ContainsKeyFrameTween(KeyFrameKey tupleKey)
{
	return m_TweenGfxRectMap.contains(tupleKey);
}

TweenProperty EntityDopeSheetScene::DetermineIfContextQuickTween(EntityTreeItemData *&pTweenTreeItemDataOut, int &iTweenStartFrameOut, int &iTweenEndFrameOut) const
{
	// Only check if there are 2 items selected
	if(selectedItems().count() != 2)
		return TWEENPROP_None;

	// Make sure both selected items are valid GraphicsKeyFrameItem's
	QGraphicsItem *pItem0 = selectedItems()[0];
	QGraphicsItem *pItem1 = selectedItems()[1];

	bool bAcquiredDataType = false;
	// ITEM 0
	DopeSheetGfxItemType eItemType0 = static_cast<DopeSheetGfxItemType>(pItem0->data(GFXDATAKEY_Type).toInt(&bAcquiredDataType));
	if(bAcquiredDataType == false)
		return TWEENPROP_None;
	if(eItemType0 == GFXITEM_TweenKnob)
	{
		pItem0 = pItem0->parentItem(); // If item 0 is a tween knob, reassign it to its parent (which is a tween keyframe)
		eItemType0 = static_cast<DopeSheetGfxItemType>(pItem1->data(GFXDATAKEY_Type).toInt(&bAcquiredDataType));
	}
	int iFrame0 = pItem0->data(GFXDATAKEY_FrameIndex).toInt();
	if(static_cast<GraphicsKeyFrameItem *>(pItem0)->IsTweenKeyFrame())
		iFrame0 += static_cast<GraphicsKeyFrameItem *>(pItem0)->GetTweenFramesDuration();
	// ITEM 1
	DopeSheetGfxItemType eItemType1 = static_cast<DopeSheetGfxItemType>(pItem1->data(GFXDATAKEY_Type).toInt(&bAcquiredDataType));
	if(bAcquiredDataType == false)
		return TWEENPROP_None;
	if(eItemType1 == GFXITEM_TweenKnob)
	{
		pItem1 = pItem1->parentItem(); // If item 1 is a tween knob, reassign it to its parent (which is a tween keyframe)
		eItemType1 = static_cast<DopeSheetGfxItemType>(pItem1->data(GFXDATAKEY_Type).toInt(&bAcquiredDataType));
	}
	int iFrame1 = pItem1->data(GFXDATAKEY_FrameIndex).toInt();
	if(static_cast<GraphicsKeyFrameItem *>(pItem1)->IsTweenKeyFrame())
		iFrame1 += static_cast<GraphicsKeyFrameItem *>(pItem1)->GetTweenFramesDuration();

	// Sort pItem0 and pItem1 by their frame index
	if(iFrame0 > iFrame1)
	{
		std::swap(pItem0, pItem1);
		std::swap(eItemType0, eItemType1);
		std::swap(iFrame0, iFrame1);
	}

	// The right side item must be a regular property keyframe
	if(eItemType1 != GFXITEM_PropertyKeyFrame)
		return TWEENPROP_None;

	// Collect all the info stored in the FrameKey for item 0
	KeyFrameKey tupleKey0 = static_cast<GraphicsKeyFrameItem *>(pItem0)->GetKey();
	QString sCategoryProp0 = std::get<GFXDATAKEY_CategoryPropString>(tupleKey0);

	// Collect all the info stored in the FrameKey for item 1
	KeyFrameKey tupleKey1 = static_cast<GraphicsKeyFrameItem *>(pItem1)->GetKey();
	QString sCategoryProp1 = std::get<GFXDATAKEY_CategoryPropString>(tupleKey1);

	// Final check to determine if a quick-tween button should be shown, and if so, which one
	if(std::get<GFXDATAKEY_TreeItemData>(tupleKey0) == std::get<GFXDATAKEY_TreeItemData>(tupleKey1) &&
		iFrame0 != iFrame1 &&
		sCategoryProp0 == sCategoryProp1)
	{
		TweenProperty eTweenProp = HyGlobal::GetTweenPropFromString(sCategoryProp0.split('/')[1]);
		if(eTweenProp == TWEENPROP_None)
			return TWEENPROP_None;

		pTweenTreeItemDataOut = std::get<GFXDATAKEY_TreeItemData>(tupleKey0);
		iTweenStartFrameOut = iFrame0;
		iTweenEndFrameOut = iFrame1;
		
		return eTweenProp;
	}
	
	return TWEENPROP_None;
}

QList<QPair<QString, QString>> EntityDopeSheetScene::GetUniquePropertiesList(EntityTreeItemData *pItemData, bool bCollapseTweenProps) const
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
			// If this category is a tween, skip over all the properties and "insert" a single QPair<> to represent it
			if(bCollapseTweenProps && sCategoryName.startsWith("Tween "))
			{
				// Match the tween category to its corresponding property
				QString sTween = sCategoryName.mid(6);
				TweenProperty eTweenProp = HyGlobal::GetTweenPropFromString(sTween);
				uniquePropertiesSet.insert(HyGlobal::ConvertTweenPropToRegularPropPair(eTweenProp));
			}
			else
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

QJsonValue EntityDopeSheetScene::BasicExtrapolateKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName) const
{
	if(m_KeyFramesMap.contains(pItemData) == false)
		return QJsonValue();

	const QMap<int, QJsonObject> &itemKeyFrameMapRef = m_KeyFramesMap[pItemData];

	// Get the closest key frame that is less than or equal to 'iFrameIndex'
	QMap<int, QJsonObject>::const_iterator iter = itemKeyFrameMapRef.find(iFrameIndex);
	if(iter == itemKeyFrameMapRef.end())
	{
		// lowerBound() - Returns an iterator pointing to the first item with key 'iFrameIndex' in the map.
		//                If the map contains no item with key 'iFrameIndex', the function returns an iterator
		//                to the nearest item with a greater key.
		iter = itemKeyFrameMapRef.lowerBound(iFrameIndex);
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

QMap<int, QMap<EntityTreeItemData *, QJsonObject>> EntityDopeSheetScene::GetKeyFrameMapPropertiesByFrame() const
{
	QMap<int, QMap<EntityTreeItemData *, QJsonObject>> keyFrameMapPropertiesByFrame;

	for(auto iterItem = m_KeyFramesMap.begin(); iterItem != m_KeyFramesMap.end(); ++iterItem)
	{
		EntityTreeItemData *pItemData = iterItem.key();
		const QMap<int, QJsonObject> &itemKeyFrameMapRef = iterItem.value();
		for(auto iterKeyFrame = itemKeyFrameMapRef.begin(); iterKeyFrame != itemKeyFrameMapRef.end(); ++iterKeyFrame)
		{
			int iFrameIndex = iterKeyFrame.key();
			const QJsonObject &keyFrameObjRef = iterKeyFrame.value();
			if(keyFrameMapPropertiesByFrame.contains(iFrameIndex) == false)
				keyFrameMapPropertiesByFrame.insert(iFrameIndex, QMap<EntityTreeItemData *, QJsonObject>());
			keyFrameMapPropertiesByFrame[iFrameIndex].insert(pItemData, keyFrameObjRef);
		}
	}

	return keyFrameMapPropertiesByFrame;
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

void EntityDopeSheetScene::RemoveKeyFrameProperties(EntityTreeItemData *pItemData, int iFrameIndex, bool bRefreshGfxItems)
{
	if(m_KeyFramesMap.contains(pItemData) == false || m_KeyFramesMap[pItemData].contains(iFrameIndex) == false)
		return;

	QJsonObject keyFrameObjCopy = m_KeyFramesMap[pItemData][iFrameIndex];
	for(auto iterCategory = keyFrameObjCopy.begin(); iterCategory != keyFrameObjCopy.end(); ++iterCategory)
	{
		QString sCategoryName = iterCategory.key();
		QJsonObject &categoryObjRef = iterCategory.value().toObject();
		QStringList sPropList = categoryObjRef.keys();
		for(int i = 0; i < sPropList.size(); ++i)
			RemoveKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, sPropList[i], bRefreshGfxItems && (i == (sPropList.size()-1)));
	}
}

void EntityDopeSheetScene::RemoveKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName, bool bRefreshGfxItems)
{
	if(m_KeyFramesMap.contains(pItemData) == false || m_KeyFramesMap[pItemData].contains(iFrameIndex) == false)
		return;

	QJsonObject &keyFrameObjRef = m_KeyFramesMap[pItemData][iFrameIndex];
	if(keyFrameObjRef.empty())
	{
		m_KeyFramesMap[pItemData].remove(iFrameIndex);
		pItemData->SetReallocateDrawItem(true);
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

	pItemData->SetReallocateDrawItem(true);

	// Remove the corresponding gfx rect for this property
	KeyFrameKey gfxRectMapKey = std::make_tuple(pItemData, iFrameIndex, sCategoryName % "/" % sPropName);
	if(m_KeyFramesGfxRectMap.contains(gfxRectMapKey))
	{
		removeItem(m_KeyFramesGfxRectMap[gfxRectMapKey]);
		delete m_KeyFramesGfxRectMap[gfxRectMapKey];
		m_KeyFramesGfxRectMap.remove(gfxRectMapKey);
	}

	// Tweens
	if(sCategoryName.startsWith("Tween "))
	{
		QString sTweenName = sCategoryName.mid(6);
		TweenProperty eTweenProp = HyGlobal::GetTweenPropFromString(sTweenName);
		QPair<QString, QString> tweenPair = HyGlobal::ConvertTweenPropToRegularPropPair(eTweenProp);
		gfxRectMapKey = std::make_tuple(pItemData, iFrameIndex, tweenPair.first % "/" % tweenPair.second);
		if(m_TweenGfxRectMap.contains(gfxRectMapKey))
		{
			removeItem(m_TweenGfxRectMap[gfxRectMapKey]);
			delete m_TweenGfxRectMap[gfxRectMapKey];
			m_TweenGfxRectMap.remove(gfxRectMapKey);
		}
	}

	if(bRefreshGfxItems)
		RefreshAllGfxItems();
}

void EntityDopeSheetScene::RemoveKeyFrameTween(EntityTreeItemData *pItemData, int iFrameIndex, TweenProperty eTweenProp, bool bRefreshGfxItems)
{
	QString sCategoryName = "Tween " % HyGlobal::TweenPropName(eTweenProp);

	RemoveKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, "Destination", false);
	RemoveKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, "Duration", false);
	RemoveKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, "Tween Type", bRefreshGfxItems);
}

TweenJsonValues EntityDopeSheetScene::GetTweenJsonValues(EntityTreeItemData *pItemData, int iFrameIndex, TweenProperty eTweenProp) const
{
	QString sCategoryName = "Tween " % HyGlobal::TweenPropName(eTweenProp);

	QJsonValue destinationValue = GetKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, "Destination");
	QJsonValue durationValue = GetKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, "Duration");
	QJsonValue tweenTypeValue = GetKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, "Tween Type");

	return TweenJsonValues(destinationValue, durationValue, tweenTypeValue);
}

void EntityDopeSheetScene::SetKeyFrameTween(EntityTreeItemData *pItemData, int iFrameIndex, TweenProperty eTweenProp, const TweenJsonValues &tweenValues, bool bRefreshGfxItems)
{
	QString sCategoryName = "Tween " % HyGlobal::TweenPropName(eTweenProp);

	SetKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, "Destination", tweenValues.m_Destination, bRefreshGfxItems);
	SetKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, "Duration", tweenValues.m_Duration, bRefreshGfxItems);
	SetKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, "Tween Type", tweenValues.m_TweenFuncType, bRefreshGfxItems);
}

void EntityDopeSheetScene::PopAllKeyFrames(EntityTreeItemData *pItemData, bool bRefreshGfxItems)
{
	if(m_KeyFramesMap.contains(pItemData) == false)
		return;

	// Store the "popped" key frames in case the user wants to readd ("Push") this item back
	m_PoppedKeyFramesMap[pItemData] = m_KeyFramesMap[pItemData];

	QMap<int, QJsonObject> &keyFrameMapRef = m_KeyFramesMap[pItemData];
	QList<int> frameIndexList = keyFrameMapRef.keys();
	for(int i = 0; i < frameIndexList.size(); ++i)
		RemoveKeyFrameProperties(pItemData, frameIndexList[i], false);

	if(bRefreshGfxItems)
		RefreshAllGfxItems();
}

void EntityDopeSheetScene::PushAllKeyFrames(EntityTreeItemData *pItemData, bool bRefreshGfxItems)
{
	if(m_PoppedKeyFramesMap.contains(pItemData) == false)
		return;
	
	for(auto iter = m_PoppedKeyFramesMap[pItemData].begin(); iter != m_PoppedKeyFramesMap[pItemData].end(); ++iter)
		SetKeyFrameProperties(pItemData, iter.key(), iter.value());

	m_PoppedKeyFramesMap.remove(pItemData);

	if(bRefreshGfxItems)
		RefreshAllGfxItems();
}

QJsonArray EntityDopeSheetScene::SerializeCallbacks() const
{
	QJsonArray callbackArray;
	for(auto iter = m_CallbackMap.begin(); iter != m_CallbackMap.end(); ++iter)
	{
		QJsonObject callbackObj;
		callbackObj.insert("frame", iter.key());
		callbackObj.insert("function", iter.value());
		callbackArray.append(callbackObj);
	}
	return callbackArray;
}

QString EntityDopeSheetScene::GetCallback(int iFrameIndex) const
{
	if(m_CallbackMap.contains(iFrameIndex))
		return m_CallbackMap[iFrameIndex];
	
	return QString(); // Indicates that no callback exists for this frame
}

void EntityDopeSheetScene::CreateCallback(int iFrameIndex, QString sCallback)
{
	if(m_CallbackMap.contains(iFrameIndex))
	{
		HyGuiLog("EntityDopeSheetScene::CreateCallback() - Callback already exists for frame index: " % QString::number(iFrameIndex), LOGTYPE_Error);
		return;
	}
	
	m_CallbackMap.insert(iFrameIndex, sCallback);
	update();
}

void EntityDopeSheetScene::RenameCallback(int iFrameIndex, QString sCallback)
{
	if(m_CallbackMap.contains(iFrameIndex))
		m_CallbackMap[iFrameIndex] = sCallback;
	else
		HyGuiLog("EntityDopeSheetScene::RenameCallback() - No callback found for frame index: " % QString::number(iFrameIndex), LOGTYPE_Error);
}

void EntityDopeSheetScene::RemoveCallback(int iFrameIndex)
{
	if(m_CallbackMap.contains(iFrameIndex))
		m_CallbackMap.remove(iFrameIndex);
	else
		HyGuiLog("EntityDopeSheetScene::RemoveCallback() - No callback found for frame index: " % QString::number(iFrameIndex), LOGTYPE_Error);

	update();
}

void EntityDopeSheetScene::NudgeKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName, int iNudgeAmount, bool bRefreshGfxItems)
{
	QJsonValue propValue = GetKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, sPropName);
	RemoveKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, sPropName, false);
	SetKeyFrameProperty(pItemData, HyMath::Max(0, iFrameIndex + iNudgeAmount), sCategoryName, sPropName, propValue, bRefreshGfxItems);
}

void EntityDopeSheetScene::NudgeKeyFrameTween(EntityTreeItemData *pItemData, int iFrameIndex, TweenProperty eTweenProp, int iNudgeAmount, bool bRefreshGfxItems)
{
	QString sCategoryName = "Tween " % HyGlobal::TweenPropName(eTweenProp);

	NudgeKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, "Destination", iNudgeAmount, bRefreshGfxItems);
	NudgeKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, "Duration", iNudgeAmount, bRefreshGfxItems);
	NudgeKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, "Tween Type", iNudgeAmount, bRefreshGfxItems);
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
				KeyFrameKey gfxRectMapKey = std::make_tuple(pItemData, iFrameIndex, sCategoryName % "/" % sPropName);
				if(m_KeyFramesGfxRectMap.contains(gfxRectMapKey))
					m_KeyFramesGfxRectMap[gfxRectMapKey]->setSelected(true);
				if(m_TweenGfxRectMap.contains(gfxRectMapKey))
					m_TweenGfxRectMap[gfxRectMapKey]->setSelected(true);
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

	// Process all the items and draw their key frames
	qreal fPosY = TIMELINE_HEIGHT + 2.0f;
	for(EntityTreeItemData *pCurItemData : entireItemList)
	{
		// 'uniquePropList' will contain every row of key frames for this item, across all frames (tweens are collapsed to their regular category/property)
		QList<QPair<QString, QString>> uniquePropList = GetUniquePropertiesList(pCurItemData, true);

		// - If NOT selected, draw all the key frames in one row (its name row)
		// - If selected, skip the name row and then draw each property in its own row
		if(pCurItemData->IsSelected())
			fPosY += ITEMS_LINE_HEIGHT; // skip the name row

		if(m_KeyFramesMap.contains(pCurItemData))
		{
			QList<TweenProperty> tweenPropList = HyGlobal::GetTweenPropList();
			const QMap<int, QJsonObject> &keyFrameMapRef = m_KeyFramesMap[pCurItemData];
			for(int iFrameIndex : keyFrameMapRef.keys())
			{
				const float fITEM_START_POSY = fPosY;
				QJsonObject propsObj = keyFrameMapRef[iFrameIndex];

				// Iterate through 'uniquePropList' and draw a QGraphicsRectItem for each property found in 'propsObj'
				for(auto &propPair : uniquePropList)
				{
					bool bPropKeyFrame = propsObj.contains(propPair.first) && propsObj[propPair.first].toObject().contains(propPair.second);
					
					bool bTweenKeyFrame = std::any_of(tweenPropList.begin(), tweenPropList.end(), [&](TweenProperty eTweenProp) {
						QPair<QString, QString> tweenPair = HyGlobal::ConvertTweenPropToRegularPropPair(eTweenProp);
						return (propPair == tweenPair && propsObj.contains("Tween " % HyGlobal::TweenPropName(eTweenProp)));
					});

					if(bPropKeyFrame == false && bTweenKeyFrame == false)
					{
						if(pCurItemData->IsSelected())
							fPosY += ITEMS_LINE_HEIGHT;
						continue;
					}

					// IMPORTANT NOTE: std::make_tuple() crashes in Release if you try to construct the string directly in the function call
					QString sPropString = propPair.first % "/" % propPair.second;
					KeyFrameKey gfxRectMapKey = std::make_tuple(pCurItemData, iFrameIndex, sPropString);

					qreal fPosX = TIMELINE_LEFT_MARGIN + (iFrameIndex * TIMELINE_NOTCH_SUBLINES_WIDTH) - 2.0f;
					
					// Add or update the GraphicsKeyFrameItem's
					if(bPropKeyFrame)
					{
						if(m_KeyFramesGfxRectMap.contains(gfxRectMapKey) == false)
						{
							GraphicsKeyFrameItem *pNewGfxRectItem = new GraphicsKeyFrameItem(gfxRectMapKey, false);
							pNewGfxRectItem->setPos(fPosX, fPosY);

							m_KeyFramesGfxRectMap[gfxRectMapKey] = pNewGfxRectItem;
							addItem(pNewGfxRectItem);
						}
						else
							m_KeyFramesGfxRectMap[gfxRectMapKey]->setPos(fPosX, fPosY);
					}
					if(bTweenKeyFrame)
					{
						if(m_TweenGfxRectMap.contains(gfxRectMapKey) == false)
						{
							fPosX += (bPropKeyFrame ? (KEYFRAME_WIDTH + 1.0f) : 0.0f);
							GraphicsKeyFrameItem *pNewGfxRectItem = new GraphicsKeyFrameItem(gfxRectMapKey, true);
							pNewGfxRectItem->setPos(fPosX, fPosY);

							m_TweenGfxRectMap[gfxRectMapKey] = pNewGfxRectItem;
							addItem(pNewGfxRectItem);
						}
						else
							m_TweenGfxRectMap[gfxRectMapKey]->setPos(fPosX + (bPropKeyFrame ? (KEYFRAME_WIDTH + 1.0f) : 0.0f), fPosY);

						// Calculate the dash-line "Duration"
						double dDuration = GetKeyFrameProperty(pCurItemData, iFrameIndex, "Tween " % propPair.second, "Duration").toDouble();
						int iNumFrames = (dDuration * static_cast<EntityModel &>(m_pEntStateData->GetModel()).GetFramesPerSecond());
						qreal fLineLength = iNumFrames * TIMELINE_NOTCH_SUBLINES_WIDTH;
						if(bPropKeyFrame)
							fLineLength -= (KEYFRAME_WIDTH + 1.0f);

						m_TweenGfxRectMap[gfxRectMapKey]->SetTweenLineLength(fLineLength, iNumFrames);
					}

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
