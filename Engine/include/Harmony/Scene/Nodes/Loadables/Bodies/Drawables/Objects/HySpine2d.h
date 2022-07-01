/**************************************************************************
 *	HySpine2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HySpine2d_h__
#define HySpine2d_h__

#include "Afx/HyInteropAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable2d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
//#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/IHySpine.h"

class HySpine2d : public IHyDrawable2d
{
	spine::Skeleton *				m_pSkeleton;
	spine::AnimationState *			m_pAnimationState;
	spine::SkeletonBounds *			m_pSkeletonBounds;

	uint32							m_uiStartingSlotIndex;

public:
	HySpine2d(std::string sPrefix = "", std::string sName = "", HyEntity2d *pParent = nullptr);
	HySpine2d(const HySpine2d &copyRef);
	virtual ~HySpine2d(void);

	const HySpine2d &operator=(const HySpine2d &rhs);

	uint32 GetNumSlots();

	virtual bool IsLoadDataValid() override;

protected:
	virtual bool OnIsValidToRender() override;
	virtual void OnDataAcquired() override;
	virtual void OnLoadedUpdate() override;

	virtual void PrepRenderStage(uint32 uiStageIndex, HyRenderMode &eRenderModeOut, uint32 &uiNumInstancesOut, uint32 &uiNumVerticesPerInstOut, bool &bIsBatchable) override;
	virtual bool WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef) override;
	virtual void OnCalcBoundingVolume() override;
};

#endif /* HySpine2d_h__ */
