/**************************************************************************
*	EntityDrawItem.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2023 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef ENTITYDRAWITEM_H
#define ENTITYDRAWITEM_H

#include "IDraw.h"

class EntityDraw;
class EntityTreeItemData;
class EntityDopeSheetScene;

struct TweenInfo
{
	const TweenProperty m_eTWEEN_PROPERTY;
	int m_iStartFrame;
	QVariant m_Start;
	QVariant m_Destination;
	float m_fDuration;
	TweenFuncType m_eTweenFunc;

	TweenInfo(TweenProperty eTweenProp) :
		m_eTWEEN_PROPERTY(eTweenProp)
	{
		Clear();
	}

	bool IsActive() const
	{
		return m_iStartFrame != -1;
	}

	void Clear()
	{
		m_iStartFrame = -1;
		m_Start.clear();
		m_Destination.clear();
		m_fDuration = 0.0f;
		m_eTweenFunc = TWEENFUNC_Unknown;
	}

	void Set(int iStartFrame, const QJsonObject &tweenObj, QVariant startValue)
	{
		m_iStartFrame = iStartFrame;
		m_fDuration = tweenObj["Duration"].toDouble();
		m_eTweenFunc = HyGlobal::GetTweenFuncFromString(tweenObj["Tween Type"].toString());
		m_Start = startValue;
		switch(m_eTWEEN_PROPERTY)
		{
		case TWEENPROP_Position:
		case TWEENPROP_Scale: {
			QJsonArray destinationArray = tweenObj["Destination"].toArray();
			m_Destination = QPointF(destinationArray[0].toDouble(), destinationArray[1].toDouble());
			break; }

		case TWEENPROP_Rotation:
		case TWEENPROP_Alpha:
			m_Destination = tweenObj["Destination"].toDouble();
			break;

		default:
			HyGuiLog("TweenInfo::Set() - Unhandled tween property", LOGTYPE_Error);
			break;
		}
	}

	QVariant Extrapolate(int iFrameIndex, float fFrameDuration)
	{
		float fElapsedTime = (iFrameIndex - m_iStartFrame) * fFrameDuration;
		fElapsedTime = HyMath::Clamp(fElapsedTime, 0.0f, m_fDuration);
		HyTweenFunc fpTweenFunc = HyGlobal::GetTweenFunc(m_eTweenFunc);
		float fRatio = (m_fDuration > 0.0f) ? fpTweenFunc(fElapsedTime / m_fDuration) : 1.0f;

		QVariant extrapolatedValue;
		switch(m_eTWEEN_PROPERTY)
		{
		case TWEENPROP_Position:
		case TWEENPROP_Scale: {
			QPointF ptStart = m_Start.toPointF();
			QPointF ptDest = m_Destination.toPointF();
			extrapolatedValue = QPointF(static_cast<float>(ptStart.x() + (ptDest.x() - ptStart.x()) * fRatio),
										static_cast<float>(ptStart.y() + (ptDest.y() - ptStart.y()) * fRatio));
			break; }

		case TWEENPROP_Rotation:
		case TWEENPROP_Alpha:
			extrapolatedValue = m_Start.toDouble() + (m_Destination.toDouble() - m_Start.toDouble()) * fRatio;
			break;

		default:
			HyGuiLog("TweenInfo::Extrapolate() - Unhandled tween property", LOGTYPE_Error);
			break;
		}

		return extrapolatedValue;
	}
};

// NOTE: this class does not keep its state when removed, it is deleted (should not be passed to UndoCmd's)
class EntityDrawItem
{
	EntityTreeItemData *					m_pEntityTreeItemData;

	IHyLoadable2d *							m_pChild;

	TransformCtrl							m_Transform;
	ShapeCtrl								m_ShapeCtrl;

public:
	EntityDrawItem(Project &projectRef, EntityTreeItemData *pModelItemData, EntityDraw *pEntityDraw, HyEntity2d *pParent);
	virtual ~EntityDrawItem();

	EntityTreeItemData *GetEntityTreeItemData() const;

	IHyLoadable2d *GetHyNode();

	ShapeCtrl &GetShapeCtrl();
	TransformCtrl &GetTransformCtrl();

	bool IsMouseInBounds();

	void RefreshTransform(HyCamera2d *pCamera);

	void ShowTransformCtrl(bool bShowGrabPoints);
	void HideTransformCtrl();
	void ExtractTransform(HyShape2d &boundingShapeOut, glm::mat4 &transformMtxOut);

	// This draw visual has all the current extrapolated data set for the current frame
	QJsonValue ExtractPropertyData(QString sCategory, QString sPropertyName);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SubEntity : public HyEntity2d
{
	int												m_iFramesPerSecond;
	QList<QPair<IHyLoadable2d *, ItemType>>			m_ChildTypeList;

	struct StateInfo
	{
		QMap<IHyNode2d *, QMap<int, QJsonObject>>	m_PropertiesMap;
		QMap<int, QStringList>						m_EventMap;
	};
	QList<StateInfo>								m_StateInfoList;

	bool											m_bTimelinePaused;
	float											m_fTimelinePausedAt;

public:
	SubEntity(Project &projectRef, int iFps, QUuid subEntityUuid, const QJsonArray &descArray, const QJsonArray &stateArray, HyEntity2d *pParent);
	virtual ~SubEntity();

	bool IsTimelinePaused() const;
	void SetTimelinePaused(float fElapsedTime, bool bPaused);

	void CtorInitJsonObj(Project &projectRef, QMap<QUuid, IHyLoadable2d *> &uuidChildMapRef, const QJsonObject &childObj);

	void ExtrapolateChildProperties(float fElapsedTime, const QJsonObject &additionalChildPropObj, HyCamera2d *pCamera);
};

void ExtrapolateProperties(IHyLoadable2d *pThisHyNode, ShapeCtrl *pShapeCtrl, bool bIsSelected, ItemType eItemType, const float fFRAME_DURATION, const int iCURRENT_FRAME, const QMap<int, QJsonObject> &keyFrameMapRef, const QMap<int, QStringList> &eventMap, HyCamera2d *pCamera);

#endif // ENTITYDRAWITEM_H
