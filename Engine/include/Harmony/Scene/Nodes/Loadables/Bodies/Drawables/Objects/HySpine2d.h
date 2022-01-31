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
#include "Assets/Nodes/HySpine2dData.h"

class HySpine2d : public IHyDrawable2d
{
//	spine::Skeleton *		m_pSpineSkeleton;
//	spine::Animation **		m_ppSpineAnims;
//	uint32					m_uiNumAnims;
//
//	spAnimationStateData *	m_pAnimStateData;
//	spAnimationState **		m_ppAnimStates;
//	uint32					m_uiNumAnimStates;
//	bool *					m_pIsAnimStateEnabled;
//	float					m_fAnimPlayRate;
//
//	spSkeletonBounds *		m_spSkeletonBounds;
//	uint32					m_uiCurAnimState;
//	bool					m_bLooping;
//
public:
	HySpine2d(std::string sPrefix = "", std::string sName = "", HyEntity2d *pParent = nullptr);
	HySpine2d(const HySpine2d &copyRef);
	virtual ~HySpine2d(void);

	const HySpine2d &operator=(const HySpine2d &rhs);

	uint32 GetNumSlots() const;

protected:
	virtual void OnWriteVertexData(HyVertexBuffer &vertexBufferRef) override;
	virtual bool OnIsValidToRender() override;
	virtual void OnCalcBoundingVolume() override;

//
//	/************************************************************************************************
//											ANIMATION CALLS
//	/************************************************************************************************/
//
//	void FlipX(bool bFlip)			{ m_pSpineSkeleton->flipX = bFlip; }
//	void FlipY(bool bFlip)			{ m_pSpineSkeleton->flipY = bFlip; }
//	
//	inline int AnimGetNumStates()	{ return m_uiNumAnims; }
//	inline uint32 AnimGetState()	{ return m_uiCurAnimState; }
//
//	// Sets every combination of animation blend to this amount
//	void AnimInitBlend(float fInterpDur);
//
//	void AnimInitBlend(const char *szAnimFrom, const char *szAnimTo, float fInterpDur);
//	void AnimInitBlend(uint32 uiAnimIdFrom, uint32 uiAnimIdTo, float fInterpDur);
//	void AnimInitState(uint32 uiNumStates);
//
//	//inline bool AnimGetIsReverse() { return ; }
//	//inline float AnimGetPlayRate() { return m_fPlayRate; }
//
//	void SetState(uint32 uiAnimId, bool bLoop, uint32 uiIndex = 0);
//	void SetState(const char *szAnimName, bool bLoop, uint32 uiIndex = 0);
//	void AnimChainState(uint32 uiAnimId, bool bLoop, float fDelay, uint32 uiIndex = 0);
//
//	void AnimSetStateEnabled(bool bEnable, uint32 uiIndex);
//
//	//--------------------------------------------------------------------------------------
//	// Returns a boolean value that represents whether this animation has finished playing.
//	// A looping animation never will return true.
//	//--------------------------------------------------------------------------------------
//	//bool AnimIsFinished()	{ spAnimationState_addAnimatio
//
//	//bool AnimIsPaused() { return m_bIsPaused; }
//
//	void AnimSetListener(void (*fpCallback)(spAnimationState* state, int trackIndex, spEventType type, spEvent* event, int loopCount), void *pParam = NULL, int uiIndex = 0);
//
//	//--------------------------------------------------------------------------------------
//	// Instantly sets the specified frame of the current animation. If supplied frame index
//	// exceeds the bounds of the current animation, it will be clamped.
//	//--------------------------------------------------------------------------------------
//	void AnimSetFrame(int iFrameIndex);
//
//	//--------------------------------------------------------------------------------------
//	// Specify an effect on the current state/animation of the Spine2d. The same effects can
//	// be achieved by using AnimSetRate(), however this may be a cleaner, and more explict 
//	// interface.
//	// 
//	// Note: The play rate value is preserved when using this function. Its +/- sign may be
//	//       switched however if told to reverse from playing forward and vice versa.	
//	//--------------------------------------------------------------------------------------
//	//void SetAnimCtrl(eAnimControl eAnimCtrl);
//
//	//--------------------------------------------------------------------------------------
//	// Modifies how fast the Spine2d's animation will play. Supplying a negative number 
//	// will set the reverse animation attribute. Supplying '0.0f' will just set the PAUSE
//	// flag and will preserve the current play rate. (1.0f = default speed)
//	//
//	// Note: This method will not unpause an Spine2d. It will just set its play rate for
//	//       when it is told to resume.
//	//--------------------------------------------------------------------------------------
//	void AnimSetPlayRate(float fPlayRate)	{ m_fAnimPlayRate = fPlayRate; }
//
//
//	//// For internal engine use
//	//void _setFactoryData(IFactoryBase *pParentFactory)
//	//{
//	//	m_pParentFactory = pParentFactory;
//	//}
//
//private:
//	//virtual void OnDataLoaded() override;
//
//	virtual void OnLoadedUpdate() override;
//	
//	virtual void OnWriteVertexData(HyVertexBuffer &vertexBufferRef) override;
};

#endif /* HySpine2d_h__ */
