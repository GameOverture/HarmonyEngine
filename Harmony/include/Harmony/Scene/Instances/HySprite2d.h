/**************************************************************************
 *	HySprite2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HySprite2d_h__
#define __HySprite2d_h__

#include "Afx/HyStdAfx.h"

#include "Scene/Instances/IHyInst2d.h"
#include "Assets/Data/HySprite2dData.h"

#include "Utilities/HyMath.h"

class HySprite2d : public IHyInst2d
{
protected:
	// Array of BYTE's where each BYTE describes how each animation state is supposed to play
	enum eAnimCtrlAttribs
	{
		ANIMCTRLATTRIB_Loop						= 1 << 0,
		ANIMCTRLATTRIB_Reverse					= 1 << 1,
		ANIMCTRLATTRIB_Bounce					= 1 << 2,
		ANIMCTRLATTRIB_IsBouncing				= 1 << 3,	// True if anim state is supposed to 'bounce' animation, AND it is currently in the reverse/bounce part of the sequence

		// Below are only used when this instance isn't loaded yet. They are the "off" values to be applied once the instance actually is loaded
		ANIMCTRLATTRIB_PRELOADAPPLY_DontLoop	= 1 << 4,
		ANIMCTRLATTRIB_PRELOADAPPLY_DontBounce	= 1 << 5,
		ANIMCTRLATTRIB_PRELOADAPPLY_DontReverse	= 1 << 6

		// Do not exceed '8' attributes, or else increase uint8s
	};
	std::vector<uint8>		m_AnimCtrlAttribList;
	bool					m_bIsAnimPaused;

	float					m_fAnimPlayRate;
	float					m_fElapsedFrameTime;

	uint32					m_uiCurAnimState;
	uint32					m_uiCurFrame;

	// Optional callback invoked upon anim completion/loop
	typedef void(*fpHySprite2dCallback)(HySprite2d &selfRef, void *pParam);
	std::vector<std::pair<fpHySprite2dCallback, void *> >	m_AnimCallbackList;

public:
	HySprite2d(const char *szPrefix, const char *szName);
	virtual ~HySprite2d(void);

	virtual void Unload();

	//--------------------------------------------------------------------------------------
	// Set how to playback the animation on the current (or specified) state/animation.
	//--------------------------------------------------------------------------------------
	void AnimCtrl(HyAnimCtrl eAnimCtrl);
	void AnimCtrl(HyAnimCtrl eAnimCtrl, uint32 uiAnimState);

	void AnimSetPause(bool bPause);
	
	uint32 AnimGetNumStates() const;
	uint32 AnimGetCurState() const;
	uint32 AnimGetNumFrames() const;
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
	bool AnimIsFinished() const;

	bool AnimIsPaused();
	
	//--------------------------------------------------------------------------------------
	// Client may specify whether to invoke a callback function when animation completes. The
	// callback will pass this entity as a ptr and return void.
	//
	// Passing 'NULL' will clear any callback that may be set.
	//
	// Note: if the animation is set to loop and a callback is specified, the callback will
	//       be invoked on every loop.
	//--------------------------------------------------------------------------------------
	void AnimSetCallback(uint32 uiStateID, void(*fpCallback)(HySprite2d &, void *), void *pParam = NULL);

	//--------------------------------------------------------------------------------------
	// Instantly sets the specified frame of the current animation. If supplied frame index
	// exceeds the bounds of the current animation, it will be clamped.
	//--------------------------------------------------------------------------------------
	void AnimSetFrame(int iFrameIndex);

	float AnimGetCurFrameWidth(bool bIncludeScaling = true);
	float AnimGetCurFrameHeight(bool bIncludScaling = true);

protected:
	virtual void OnDataLoaded();

	virtual void OnUpdate();

	virtual void OnCalcBoundingVolume();

	virtual void OnUpdateUniforms();
	virtual void OnWriteDrawBufferData(char *&pRefDataWritePos);
};

#endif /* __HySprite2d_h__ */
