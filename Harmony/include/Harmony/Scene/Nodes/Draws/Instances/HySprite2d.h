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
#include "Scene/Nodes/Draws/Instances/IHyDrawInst2d.h"
#include "Assets/Nodes/HySprite2dData.h"

#include "Utilities/HyMath.h"

class HySprite2d;
typedef std::function<void(HySprite2d *pSelf, void *pParam)> HySprite2dAnimFinishedCallback;

class HySprite2d : public IHyDrawInst2d
{
protected:
	// Array of BYTE's where each BYTE describes how each animation state is supposed to play
	enum AnimCtrlAttribs
	{
		ANIMCTRLATTRIB_Loop						= 1 << 0,
		ANIMCTRLATTRIB_Reverse					= 1 << 1,
		ANIMCTRLATTRIB_Bounce					= 1 << 2,
		ANIMCTRLATTRIB_IsBouncing				= 1 << 3,	// True if anim state is supposed to 'bounce' animation, AND it is currently in the reverse/bounce part of the sequence
		ANIMCTRLATTRIB_Finished					= 1 << 4,	// True if a non-looping animation finishes its full sequence and reset to false whenever any animation is played
		ANIMCTRLATTRIB_Invalid					= 1 << 5

		// Do not exceed '8' attributes, or else increase uint8s
	};
	std::vector<uint8>		m_AnimCtrlAttribList;
	bool					m_bIsAnimPaused;

	float					m_fAnimPlayRate;
	float					m_fElapsedFrameTime;

	uint32					m_uiCurAnimState;
	uint32					m_uiCurFrame;

	// Optional callback invoked upon anim completion/loop
	std::vector<std::pair<HySprite2dAnimFinishedCallback, void *> >	m_AnimCallbackList;

public:
	HySprite2d(const char *szPrefix, const char *szName, HyEntity2d *pParent);
	HySprite2d(const HySprite2d &copyRef);
	virtual ~HySprite2d(void);

	const HySprite2d &operator=(const HySprite2d &rhs);
	virtual HySprite2d *Clone() const;

	//--------------------------------------------------------------------------------------
	// Set how to playback the animation on the current (or specified) state/animation.
	//--------------------------------------------------------------------------------------
	void AnimCtrl(HyAnimCtrl eAnimCtrl);
	void AnimCtrl(HyAnimCtrl eAnimCtrl, uint32 uiAnimState);

	bool AnimIsReverse();
	bool AnimIsReverse(uint32 uiAnimState);
	
	uint32 AnimGetNumStates();
	uint32 AnimGetState() const;
	uint32 AnimGetNumFrames();
	uint32 AnimGetFrame() const;
	void AnimSetFrame(uint32 uiFrameIndex);

	//--------------------------------------------------------------------------------------
	// Returns the time modifier (defaulted to 1.0f) that's applied the animation frame duration 
	// set by the Designer Tool.
	//--------------------------------------------------------------------------------------
	float AnimGetPlayRate() const;

	//--------------------------------------------------------------------------------------
	// Modifies the time modifier that's applied the animation frame duration 
	// set by the Designer Tool. Negative fPlayRate is invalid. 
	//
	// Note: This method will not unpause an sprite. It will just set its play rate for
	//       when it is told to resume.
	//--------------------------------------------------------------------------------------
	void AnimSetPlayRate(float fPlayRate);

	//--------------------------------------------------------------------------------------
	// Change the animation state of the sprite. The new state sets the current frame index
	// to frame [0] or frame [last] if the animation ctrls specify to play in reverse.
	//
	// Note: This does not automatically begin playing the animation. If entity was instructed
	//       to pause prior to AnimSetState(), it will switch to inital frame and continue 
	//       to pause.
	//--------------------------------------------------------------------------------------
	void AnimSetState(uint32 uiStateIndex);

	//--------------------------------------------------------------------------------------
	// Returns a boolean value that represents whether this animation has finished playing.
	// A looping animation never will return true.
	//--------------------------------------------------------------------------------------
	bool AnimIsFinished();

	bool AnimIsPaused();

	void AnimSetPause(bool bPause);

	float AnimGetDuration();
	
	//--------------------------------------------------------------------------------------
	// Invoke a callback function when animation loops or completes. The
	// callback will pass this sprite as a pointer with an optional parameter.
	//
	// Leaving default arguments will clear any currently assigned call back.
	//--------------------------------------------------------------------------------------
	void AnimSetCallback(uint32 uiStateIndex, HySprite2dAnimFinishedCallback callBack = HySprite2d::NullAnimCallback, void *pParam = nullptr);

	float AnimGetCurFrameWidth(bool bIncludeScaling = true);	// Returns the ALPHA-CROPPED width of the current frame
	float AnimGetCurFrameHeight(bool bIncludScaling = true);	// Returns the ALPHA-CROPPED height of the current frame

	const glm::ivec2 &AnimGetCurFrameOffset();

	virtual bool IsLoadDataValid() override;

protected:
	virtual bool OnIsValid() override;
	virtual void CalcBoundingVolume() override;
	virtual void DrawLoadedUpdate() override;
	virtual void OnDataAcquired() override;
	virtual void OnUpdateUniforms() override;
	virtual void OnWriteVertexData(char *&pRefDataWritePos) override;

	static void NullAnimCallback(HySprite2d *pSelf, void *pParam);
};

#endif /* HySprite2d_h__ */
