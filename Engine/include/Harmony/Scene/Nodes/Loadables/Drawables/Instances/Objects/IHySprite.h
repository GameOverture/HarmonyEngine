/**************************************************************************
 *	IHySprite.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2021 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IHySprite_h__
#define IHySprite_h__

#include "Afx/HyStdAfx.h"

class IHyNode;
class HyVertexBuffer;

template<typename NODETYPE, typename ENTTYPE>
class IHySprite : public NODETYPE
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

	uint32					m_uiCurFrame;

	glm::ivec2				m_vCustomOffset;					// If set, every frame offsets by this amount (plus any offset it is created with in the Editor)

public:
	IHySprite();
	IHySprite(const IHySprite &copyRef);
	virtual ~IHySprite(void);

	const IHySprite &operator=(const IHySprite &rhs);

	//--------------------------------------------------------------------------------------
	// Set how to playback the animation on the current (or specified) state/animation.
	//--------------------------------------------------------------------------------------
	void SetAnimCtrl(HyAnimCtrl eAnimCtrl);
	void SetAnimCtrl(HyAnimCtrl eAnimCtrl, uint32 uiStateIndex);

	bool IsAnimReverse();
	bool IsAnimReverse(uint32 uiStateIndex);
	
	uint32 GetNumFrames();
	uint32 GetFrame() const;
	void SetFrame(uint32 uiFrameIndex);

	//--------------------------------------------------------------------------------------
	// Returns the time modifier (defaulted to 1.0f) that's applied the animation frame duration 
	// set by the Editor Tool.
	//--------------------------------------------------------------------------------------
	float GetAnimRate() const;

	//--------------------------------------------------------------------------------------
	// Modifies the time modifier that's applied the animation frame duration 
	// set by the Editor Tool. Negative fPlayRate is invalid. 
	//
	// Note: This method will not unpause an sprite. It will just set its play rate for
	//       when it is told to resume.
	//--------------------------------------------------------------------------------------
	void SetAnimRate(float fPlayRate);

	//--------------------------------------------------------------------------------------
	// Change the animation state of the sprite. The new state sets the current frame index
	// to frame [0] or frame [last] if the animation ctrls specify to play in reverse.
	//
	// Note: This does not automatically begin playing the animation. If entity was instructed
	//       to pause prior to SetState(), it will switch to inital frame and continue 
	//       to pause.
	//--------------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------------
	// Returns a boolean value that represents whether this animation has finished playing.
	// A looping animation never will return true.
	//--------------------------------------------------------------------------------------
	bool IsAnimFinished();

	bool IsAnimPaused();

	void SetAnimPause(bool bPause);

	float GetAnimDuration();

	float GetCurFrameWidth(bool bIncludeScaling = true);	// Returns the ALPHA-CROPPED width of the current frame
	float GetCurFrameHeight(bool bIncludScaling = true);	// Returns the ALPHA-CROPPED height of the current frame

	float GetStateMaxWidth(uint32 uiStateIndex, bool bIncludeScaling = true);	// Returns the ALPHA-CROPPED width of the widest frame in this animation state
	float GetStateMaxHeight(uint32 uiStateIndex, bool bIncludeScaling = true);	// Returns the ALPHA-CROPPED height of the tallest frame in this animation state

	void SetUserOffset(int32 iOffsetX, int32 iOffsetY);
	glm::ivec2 GetCurFrameOffset();

	virtual void SetState(uint32 uiStateIndex) override;
	virtual bool IsLoadDataValid() override;

protected:
	virtual void OnInvokeCallback(uint32 uiStateIndex) = 0;
	virtual bool OnIsValidToRender() override;
	virtual void OnDataAcquired() override;
	virtual void OnLoadedUpdate() override;
};

#endif /* IHySprite_h__ */
