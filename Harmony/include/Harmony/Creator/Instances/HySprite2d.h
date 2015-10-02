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

#include "Creator/Instances/IHyInst2d.h"
#include "FileIO/Data/HySprite2dData.h"

#include "Utilities/HyMath.h"

class HySprite2d : public IHyInst2d
{
	// Array of BYTE's where each BYTE describes how each animation state is supposed to play
	enum eStateAttribs
	{
		STATEATTRIB_Loop = 1 << 0,
		STATEATTRIB_Reverse = 1 << 1,
		STATEATTRIB_Bounce = 1 << 2,
		// Do not exceed '8' attributes, or else increase uint8
	};
	uint8 *		m_pAnimStateAttribs;
	float		m_fPlayRate;
	bool		m_bIsPaused;
	float		m_fTime;			// Used to determine when to advance a frame in current animation
	int32		m_iCurAnimState;
	int32		m_iCurFrame;
	bool		m_bIsBouncing;		// True if anim state is supposed to 'bounce' animation, AND it is currently in the reverse/bounce part of the sequence

public:
	HySprite2d(const char *szPrefix, const char *szName);
	virtual ~HySprite2d(void);
	
	inline int32 GetNumStates()				;//{ return reinterpret_cast<HySprite2dData *>(m_pDataPtr)->m_iNumStates; }
	inline int32 GetState()					;//{ return m_iCurAnimState; }
	inline int32 GetState(int32 iIndex)		;//{ return ; }
	inline int32 GetNumFrames()				;//{ return GetCurAnimState().m_iNumFrames; }
	inline int32 GetFrame()					;//{ return m_iCurFrame; }

	//--------------------------------------------------------------------------------------
	// 
	//--------------------------------------------------------------------------------------
	inline float AnimGetPlayRate() { return m_fPlayRate; }

	//--------------------------------------------------------------------------------------
	// Change the state of the entity, in other words, essentially swaps the animation and
	// goes to frame [0] or frame [last] if playing in reverse.
	//
	// Note: This does not automatically begin playing the animation. If entity was instructed
	//       to pause prior to AnimSetState(), it will switch to inital frame and continue 
	//       to pause.
	//--------------------------------------------------------------------------------------
	void AnimSetState(int iStateID);

	//--------------------------------------------------------------------------------------
	// Returns a boolean value that represents whether this animation has finished playing.
	// A looping animation never will return true.
	//--------------------------------------------------------------------------------------
	bool AnimIsFinished();

	bool AnimIsPaused() { return m_bIsPaused; }
	
	//--------------------------------------------------------------------------------------
	// Client may specify whether to invoke a callback function when animation completes. The
	// callback will pass this entity as a ptr and return void.
	//
	// Passing 'NULL' will clear any callback that may be set.
	//
	// Note: if the animation is set to loop and a callback is specified, the callback will
	//       be invoked on every loop.
	//--------------------------------------------------------------------------------------
	void AnimSetCallback(int iStateID, void (*fpCallback)(HySprite2d *, void *), void *pParam = NULL);

	//--------------------------------------------------------------------------------------
	// Instantly sets the specified frame of the current animation. If supplied frame index
	// exceeds the bounds of the current animation, it will be clamped.
	//--------------------------------------------------------------------------------------
	void AnimSetFrame(int iFrameIndex);

	//--------------------------------------------------------------------------------------
	// Specify an effect on the current state/animation of the entity. The same effects can
	// be achieved by using AnimSetRate(), however this may be a cleaner, and more explict 
	// interface.
	// 
	// Note: The play rate value is preserved when using this function. Its +/- sign may be
	//       switched however if told to reverse from playing forward and vice versa.	
	//--------------------------------------------------------------------------------------
	void AnimCtrl(/*eAnimControl eAnimCtrl*/);

	//--------------------------------------------------------------------------------------
	// Modifies how fast the entity's animation will play. Supplying a negative number 
	// will set the reverse animation attribute. Supplying '0.0f' will just set the PAUSE
	// flag and will preserve the current play rate. (1.0f = default speed)
	//
	// Note: This method will not unpause an entity. It will just set its play rate for
	//       when it is told to resume.
	//--------------------------------------------------------------------------------------
	void AnimSetPlayRate(float fPlayRate);

	// Returns a const RECT pointer to the internal drawing texture source rectangle. This method
	// can sometimes be useful to determine the dimensions of an entity in its current state 
	// and frame. 
	// 
	// NOTE: This rectangle may be misleading in the case where the source rectangle includes
	//       alpha'ed out pixels around the source art.
	const RECT *GetCurRect();

	int GetCurFrameWidth(bool bCalcProceduralScale = false);
	int GetCurFrameHeight(bool bCalcProceduralScale = false);

private:
	virtual void OnDataLoaded();
	virtual void Update();

	virtual void WriteDrawBufferData(char *&pRefDataWritePos);
};

#endif /* __HySprite2d_h__ */
