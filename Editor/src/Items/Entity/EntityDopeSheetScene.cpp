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


#define TIMELINE_HEIGHT 38.0f
#define TIMELINE_NOTCH_START_XPOS 250.0f
#define TIMELINE_NOTCH_WIDTH 88.0f
#define TIMELINE_NOTCH_MAINLINE_HEIGHT 15.0f
#define TIMELINE_NOTCH_SUBLINES_HEIGHT 9.0f
#define TIMELINE_NOTCH_SUBLINES_WIDTH 18.0f
#define TIMELINE_NOTCH_TEXT_YPOS 10.0f

EntityDopeSheetScene::EntityDopeSheetScene(EntityStateData *pStateData, QJsonObject metaFileObj) :
	QGraphicsScene(),
	m_pEntStateData(pStateData),
	m_iFramesPerSecond(metaFileObj["framesPerSecond"].toInt()),
	m_iCurrentFrame(0),
	m_ViewportSize(1000.0f, 500.0f),
	m_fZoom(1.0f),
	m_TimeLine(this)
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

	setBackgroundBrush(Qt::blue);
	setSceneRect(0, 0, m_ViewportSize.width(), m_ViewportSize.height());

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

void EntityDopeSheetScene::SetViewportSize(QSizeF size)
{
	if(m_ViewportSize == size)
		return;

	m_ViewportSize = size;
	setSceneRect(0, 0, m_ViewportSize.width(), m_ViewportSize.height());
	
	// Update Scene Time line
	m_TimeLine.Update(m_ViewportSize, m_fZoom);
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

EntityDopeSheetScene::TimeLine::TimeLine(QGraphicsScene *pGfxSceneRef) :
	m_fCurWidth(0.0f),
	m_fCurZoom(1.0f)
{
	m_pRectBackground = pGfxSceneRef->addRect(QRectF(), Qt::NoPen, HyGlobal::CovertHyColor(HyColor::ContainerPanel));
	
	QPen pen(Qt::SolidLine);
	pen.setColor(HyGlobal::CovertHyColor(HyColor::WidgetFrame));
	m_pLineSeparator = pGfxSceneRef->addLine(QLineF(), pen);
}

EntityDopeSheetScene::TimeLine::~TimeLine()
{
}

void EntityDopeSheetScene::TimeLine::Update(QSizeF viewportSize, float fZoom)
{
	if(m_fCurWidth == viewportSize.width() && m_fCurZoom == fZoom)
		return;

	m_fCurWidth = viewportSize.width();
	m_fCurZoom = fZoom;

	m_pRectBackground->setRect(0.0f, 0.0f, m_fCurWidth, TIMELINE_HEIGHT);
	m_pLineSeparator->setLine(0, TIMELINE_HEIGHT, m_fCurWidth, TIMELINE_HEIGHT);

	int iNotchIndex = 0;
	int iFrameIndex = 0;
	float fRemainingWidth = m_fCurWidth;
	fRemainingWidth -= TIMELINE_NOTCH_START_XPOS;
	while(fRemainingWidth > 0.0f)
	{
		while(iNotchIndex >= m_NotchLineList.size())
			m_NotchLineList.push_back(new Notch(Root()));

		float fPosX = TIMELINE_NOTCH_START_XPOS + ((TIMELINE_NOTCH_WIDTH * m_fCurZoom) * iNotchIndex);
		float fSubLineSpacing = TIMELINE_NOTCH_SUBLINES_WIDTH;
		int iNumSubLines = 4;
		m_NotchLineList[iNotchIndex]->Update(fPosX, fSubLineSpacing, iNumSubLines, QString::number(iFrameIndex));
		m_NotchLineList[iNotchIndex]->Root()->show();

		iNotchIndex++;
		iFrameIndex += (iNumSubLines + 1);
		fRemainingWidth -= TIMELINE_NOTCH_WIDTH * m_fCurZoom;
	}

	for(; iNotchIndex < m_NotchLineList.size(); ++iNotchIndex)
		m_NotchLineList[iNotchIndex]->Root()->hide();
}

EntityDopeSheetScene::TimeLine::Notch::Notch(QGraphicsItem *pParent)
{
	QPen pen(Qt::SolidLine);
	pen.setColor(HyGlobal::CovertHyColor(HyColor::WidgetFrame));

	m_pMainLine = new QGraphicsLineItem(pParent);
	m_pMainLine->setPen(pen);
	m_pMainLine->setLine(0.0f, TIMELINE_HEIGHT - TIMELINE_NOTCH_MAINLINE_HEIGHT, 0.0f, TIMELINE_HEIGHT);

	m_pTextFrameShadow = new QGraphicsSimpleTextItem(m_pMainLine);
	m_pTextFrameShadow->setBrush(HyGlobal::CovertHyColor(HyColor::Black));
	m_pTextFrame = new QGraphicsSimpleTextItem(m_pMainLine);
	m_pTextFrame->setBrush(HyGlobal::CovertHyColor(HyColor::WidgetFrame));

	for(int i = 0; i < 4; ++i)
	{
		m_pSubLine[i] = new QGraphicsLineItem(m_pMainLine);
		m_pSubLine[i]->setPen(pen);
		m_pSubLine[i]->setLine(0.0f, TIMELINE_HEIGHT - TIMELINE_NOTCH_SUBLINES_HEIGHT, 0.0f, TIMELINE_HEIGHT);
	}
}

EntityDopeSheetScene::TimeLine::Notch::~Notch()
{
	for(int i = 0; i < 4; ++i)
		delete m_pSubLine[i];
	delete m_pTextFrame;
	delete m_pTextFrameShadow;
	delete m_pMainLine;
}

void EntityDopeSheetScene::TimeLine::Notch::Update(float fPosX, float fSubLineSpacing, int iNumSubLines, QString sFrameText)
{
	m_pMainLine->setPos(fPosX, 0.0f);
	
	m_pTextFrame->setText(sFrameText);
	m_pTextFrame->setPos(m_pTextFrame->boundingRect().width() * -0.5f, TIMELINE_NOTCH_TEXT_YPOS);
	m_pTextFrameShadow->setText(sFrameText);
	m_pTextFrameShadow->setPos(m_pTextFrame->pos().x() + 1.0f, m_pTextFrame->pos().y() + 1.0f);
	
	for(int i = 0; i < 4; ++i)
	{
		m_pSubLine[i]->setPos(fSubLineSpacing * (i+1), 0.0f);
		if(i < iNumSubLines)
			m_pSubLine[i]->show();
		else
			m_pSubLine[i]->hide();
	}
}

void EntityDopeSheetScene::UpdateSceneItems()
{
	QRectF rect = sceneRect();
}
