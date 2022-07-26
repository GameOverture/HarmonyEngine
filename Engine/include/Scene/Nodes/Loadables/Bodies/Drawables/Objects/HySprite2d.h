/**************************************************************************
 *	HySprite2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HySprite2d_h__
#define HySprite2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/IHySprite.h"

class HySprite2d;
typedef std::function<void(HySprite2d *pSelf, void *pParam)> HySprite2dAnimFinishedCallback;

class HySprite2d : public IHySprite<IHyDrawable2d, HyEntity2d>
{
	// Optional callback invoked upon anim completion/loop
	std::vector<std::pair<HySprite2dAnimFinishedCallback, void *> >	m_AnimCallbackList;

public:
	HySprite2d(std::string sPrefix = "", std::string sName = "", HyEntity2d *pParent = nullptr);
	HySprite2d(const HySprite2d &copyRef);
	virtual ~HySprite2d(void);

	const HySprite2d &operator=(const HySprite2d &rhs);

	//--------------------------------------------------------------------------------------
	// Invoke a callback function when animation loops or completes. The
	// callback will pass this sprite as a pointer with an optional parameter.
	//
	// Leaving default arguments will clear any currently assigned call back.
	//--------------------------------------------------------------------------------------
	void SetAnimCallback(uint32 uiStateIndex, HySprite2dAnimFinishedCallback callBack = HySprite2d::NullAnimCallback, void *pParam = nullptr);

protected:
	virtual void OnInvokeCallback(uint32 uiStateIndex) override;
	virtual void OnCalcBoundingVolume() override;
	virtual void OnDataAcquired() override;

	virtual void PrepRenderStage(uint32 uiStageIndex, HyRenderMode &eRenderModeOut, uint32 &uiNumInstancesOut, uint32 &uiNumVerticesPerInstOut, bool &bIsBatchable) override;
	virtual bool WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef) override;
	
	static void NullAnimCallback(HySprite2d *pSelf, void *pParam) { }
};

#endif /* HySprite2d_h__ */
