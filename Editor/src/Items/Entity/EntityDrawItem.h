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

#include "IDrawExItem.h"

class EntityDraw;
class EntityTreeItemData;
class GfxShapeHyView;
class GfxShapeModel;

// NOTE: this class does not keep its state when removed, it is deleted (should not be passed to UndoCmd's)
class EntityDrawItem : public IDrawExItem
{
	EntityTreeItemData *							m_pEntityTreeItemData;
	IHyBody2d *										m_pChild;
	GfxShapeHyView *								m_pShapeView;

public:
	EntityDrawItem(Project &projectRef, EntityTreeItemData *pModelItemData, EntityDraw *pEntityDraw, HyEntity2d *pParent);
	virtual ~EntityDrawItem();

	EntityDraw &GetEntityDraw();
	EntityTreeItemData *GetEntityTreeItemData() const;
	virtual IHyBody2d *GetHyNode() override;

	virtual bool IsSelectable() const override;
	virtual bool IsSelected() override;

	virtual void RefreshTransform(HyCamera2d *pCamera) override;

	GfxShapeHyView *GetShapeView();

	// This draw visual has all the current extrapolated data set for the current frame
	QJsonValue ExtractPropertyData(QString sCategory, QString sPropertyName);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct EntityPreviewComponent;

// Child entities of the root entity are these SubEntity objects
class SubEntity : public HyEntity2d
{
	Project &										m_ProjectRef;

	struct ChildInfo
	{
		IHyLoadable2d *								m_pChild;
		ItemType									m_eItemType;
		EntityPreviewComponent *					m_pPreviewComponent;
		ChildInfo(IHyLoadable2d *pChild, ItemType eItemType, EntityPreviewComponent *pPreviewComponent) :
			m_pChild(pChild),
			m_eItemType(eItemType),
			m_pPreviewComponent(pPreviewComponent)
		{ }
	};
	QList<ChildInfo>								m_ChildInfoList;
	//QList<QPair<IHyLoadable2d *, ItemType>>			m_ChildTypeList;

	struct StateInfo
	{
		QMap<int, QJsonObject>						m_RootPropertiesMap;
		QMap<IHyNode2d *, QMap<int, QJsonObject>>	m_ChildPropertiesMap;
	};
	QList<StateInfo>								m_StateInfoList;

	bool											m_bSubTimelinePaused;
	int												m_iSubTimelineStartFrame;
	int												m_iSubTimelineRemainingFrames;
	bool											m_bSubTimelineDirty;			// If a timeline property has been modified, this indicates we need to re-extrapolate the properties with new inputs
	
	int												m_iElapsedTimelineFrames;
	int												m_iRemainingTimelineFrames;

	QList<QPair<int, QString>>						m_ConflictingPropsList;	// A list of properties that are both set by the main entity, and this sub-entity at the same time

public:
	SubEntity(Project &projectRef, QUuid subEntityUuid, const QJsonArray &descArray, const QJsonArray &stateArray, HyEntity2d *pParent);
	virtual ~SubEntity();
	void CtorInitJsonObj(QMap<QUuid, IHyLoadable2d *> &uuidChildMapRef, const QJsonObject &childObj);

	void Extrapolate(const QMap<int, QJsonObject> &propMapRef, EntityPreviewComponent &previewComponentRef, bool bIsSelected, float fFrameDuration, int iMainDestinationFrame);

	void MergeRootProperties(QMap<int, QJsonObject> &mergeMapOut);

	bool IsTimelinePaused() const;
	int GetTimelineFrame() const;

	bool TimelineEvent(int iMainTimelineFrame, QJsonObject timelineObj); // Returns true if state changes, invalidating the current timeline

protected:
	void ExtrapolateChildProperties(int iNumFramesDuration, uint32 uiStateIndex);

private:
	using HyEntity2d::SetState;
};


void ExtrapolateProperties(Project &projectRef, IHyLoadable2d *pThisHyNode, GfxShapeModel *pShapeModel, bool bIsSelected, ItemType eItemType, const float fFRAME_DURATION, const int iSTART_FRAME, const int iDESTINATION_FRAME, const QMap<int, QJsonObject> &keyFrameMapRef, EntityPreviewComponent &previewComponentRef);

#endif // ENTITYDRAWITEM_H
