/**************************************************************************
*	EntityTreeItemData.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef EntityTreeItemData_H
#define EntityTreeItemData_H

#include "ProjectItemData.h"
#include "PropertiesTreeMultiModel.h"
#include "EditModeModel.h"

#include <QJsonArray>

class EntityModel;
class PropertiesUndoCmd;

class EntityPropertiesTreeModel : public PropertiesTreeModel
{
	Q_OBJECT

public:
	explicit EntityPropertiesTreeModel(ProjectItemData &projItemRef, int iStateIndex, QVariant subState, QObject *pParent = nullptr);
	virtual ~EntityPropertiesTreeModel();

	virtual PropertiesUndoCmd *AllocateUndoCmd(const QModelIndex &index, const QVariant &newData) override;
};

class EntityPropertiesTreeMultiModel : public PropertiesTreeMultiModel
{
	Q_OBJECT

public:
	explicit EntityPropertiesTreeMultiModel(ProjectItemData &projItemRef, int iStateIndex, QVariant subState, QList<PropertiesTreeModel *> multiModelList, QObject *pParent = nullptr);
	virtual ~EntityPropertiesTreeMultiModel();

	virtual PropertiesUndoCmd *AllocateUndoCmd(const QModelIndex &index, const QVariant &newData) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct TweenInfo
{
	const TweenProperty								m_eTWEEN_PROPERTY;
	int												m_iStartFrame;
	QVariant										m_Start;
	QVariant										m_Destination;
	float											m_fDuration;
	TweenFuncType									m_eTweenFunc;

	TweenInfo(TweenProperty eTweenProp) :
		m_eTWEEN_PROPERTY(eTweenProp)
	{
		Clear();
	}

	bool IsActive() const
	{
		return m_iStartFrame != -2;
	}

	void Clear()
	{
		m_iStartFrame = -2;
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

	void ExtrapolateIntoNode(IHyLoadable2d *pThisHyNode, int iFrameIndex, float fFrameDuration)
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

		// Apply the extrapolated value to pThisHyNode
		switch(m_eTWEEN_PROPERTY)
		{
		case TWEENPROP_Position:
			pThisHyNode->pos.SetX(static_cast<float>(extrapolatedValue.toPointF().x()));
			pThisHyNode->pos.SetY(static_cast<float>(extrapolatedValue.toPointF().y()));
			break;

		case TWEENPROP_Rotation:
			pThisHyNode->rot.Set(extrapolatedValue.toDouble());
			break;

		case TWEENPROP_Scale:
			pThisHyNode->scale.SetX(static_cast<float>(extrapolatedValue.toPointF().x()));
			pThisHyNode->scale.SetY(static_cast<float>(extrapolatedValue.toPointF().y()));
			break;

		case TWEENPROP_Alpha:
			static_cast<IHyBody2d *>(pThisHyNode)->alpha.Set(extrapolatedValue.toDouble());
			break;

		default:
			HyGuiLog("TweenInfo::Extrapolate() - Unhandled tween property", LOGTYPE_Error);
			break;
		}
	}
};

struct EntityPreviewComponent
{
	bool											m_bReallocateDrawItem = false;	// This is set when the draw item needs any of its default values set that doesn't have a corresponding key frame (like when deleting a keyframe)

	// Sprite Special Case:
	// To determine the sprite's animation frame that should be presented, whenever a property that might affect
	// what frame the sprite's animation could be on, calculate 'spriteLastKnownAnimInfo' up to that point.
	// Once all properties have been processed, extrapolate the remaining time up to the Entity's 'iCURRENT_FRAME'
	struct LastKnownSpriteInfo
	{
		int m_iEntityFrame;
		int m_iSpriteFrame;		// Sprite's frame (-1 indicates it hasn't been set, and should be HYANIMCTRL_Reset)
		bool m_bBouncePhase;	// A boolean whether animation is in the "bounce phase"
		bool m_bPaused;			// A boolean whether animation is paused
		LastKnownSpriteInfo() :
			m_iEntityFrame(0),
			m_iSpriteFrame(-1),
			m_bBouncePhase(false),
			m_bPaused(false) { }

		LastKnownSpriteInfo(int iEntityFrame, int iSpriteFrame, bool bBouncePhase, bool bPaused) :
			m_iEntityFrame(iEntityFrame),
			m_iSpriteFrame(iSpriteFrame),
			m_bBouncePhase(bBouncePhase),
			m_bPaused(bPaused) { }
	};
	LastKnownSpriteInfo								m_SpriteInfo;

	// Tween Special Case:
	// To determine the tweens' current values, store the info that kicked it off, and extrapolate based on Entity's 'iCURRENT_FRAME'
	TweenInfo										m_TweenInfo[NUM_TWEENPROPS] = { TWEENPROP_Position, TWEENPROP_Rotation, TWEENPROP_Scale, TWEENPROP_Alpha };

	QUuid											m_CurrentWidgetPanelNodeUuid;		// Only used when this draw item is a UI widget and its panel uses a project node
	QUuid											m_CurrentWidgetTextNodeUuid;		// Only used when this draw item is a UI widget
	QUuid											m_CurrentWidgetBarPanelNodeUuid;	// Only used when this draw item is a UI BarMeter Widget, and its bar panel uses a project node
};

enum EntityItemType
{
	ENTTYPE_Unknown = -1,

	ENTTYPE_Root = 0,
	ENTTYPE_FixtureFolder,
	ENTTYPE_FusedItem,
	ENTTYPE_Item,
	ENTTYPE_SubItem,
	ENTTYPE_ArrayFolder,
	ENTTYPE_ArrayItem,
};
class EntityTreeItemData : public TreeModelItemData
{
	Q_OBJECT

	EntityModel &										m_EntityModelRef;
	EntityItemType										m_eEntType;

	EntityPropertiesTreeModel *							m_pPropertiesModel;
	EditModeModel *										m_pEditModel;			// Only allocated when this item is capable of using Edit Mode

	EntityItemDeclarationType							m_eDeclarationType;

	QUuid												m_ReferencedItemUuid;

	QUuid												m_GuiParentUuid;

	bool												m_bIsLocked;			// Whether this item is locked and cannot be selected
	bool												m_bIsSelected;			// Whether this item is considered selected. Applicable to in all views (Harmony, Node List, Dope Sheet, etc)
	bool												m_bIsDopeExpanded;		// True when this item is expanded and showing all its property keyframes on each row. False shows a collapsed, single row with all its keyframes
	
	EntityPreviewComponent								m_PreviewComponent;		// Extra info for deducing how to extrapolate the item's properties when previewing

public:
	EntityTreeItemData(EntityModel &entityModelRef, EntityItemDeclarationType eDeclarationType, QString sCodeName, ItemType eItemType, EntityItemType eEntType, QUuid uuidOfReferencedItem, QUuid uuidOfThis);
	EntityTreeItemData(EntityModel &entityModelRef, QJsonObject descObj, EntityItemType eEntType);
	virtual ~EntityTreeItemData();

	bool IsSelectable() const;
	bool IsEditable() const;	// This is compatible with Edit Mode
	void SetLocked(bool bIsLocked);

	EntityItemType GetEntType() const;
	QString GetHyNodeTypeName(bool bIncludeNamespace) const;
	QString GetCodeName() const;

	const QUuid &GetThisUuid() const;
	const QUuid &GetReferencedItemUuid() const;
	void SetReferencedItemUuid(QUuid uuidOfReferencedItem);

	const QUuid &GetGuiParentUuid() const;
	void SetGuiParentUuid(QUuid uuidOfGuiParent);

	EntityItemDeclarationType GetDeclarationType() const;

	EntityModel &GetEntityModel() const;
	EntityPropertiesTreeModel &GetPropertiesModel() const;

	EditModeModel *GetEditModel();

	bool IsSelected() const;
	void SetSelected(bool bIsSelected);

	bool IsDopeExpanded() const;
	void SetDopeExpanded(bool bIsDopeExpanded);

	bool IsReallocateDrawItem() const;
	void SetReallocateDrawItem(bool bReallocateDrawItem);

	EntityPreviewComponent &GetPreviewComponent();

	int GetArrayIndex() const; // Returns -1 when not an array item
	int GetNumArrayItems() const; // Returns the number of array items that this item is apart of. -1 when not an array item

	void InsertJsonInfo_Desc(QJsonObject &childObjRef);

protected:
	void InitalizePropertyModel();
};

#endif // EntityTreeItemData_H
