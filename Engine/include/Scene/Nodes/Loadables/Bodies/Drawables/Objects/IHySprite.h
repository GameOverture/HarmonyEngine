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

public:
	IHySprite(std::string sPrefix, std::string sName, ENTTYPE *pParent);
	IHySprite(const IHySprite &copyRef);
	virtual ~IHySprite(void);

	const IHySprite &operator=(const IHySprite &rhs);

	void SetAnimCtrl(HyAnimCtrl eAnimCtrl);							// Set how to playback the animation on the current state
	void SetAnimCtrl(HyAnimCtrl eAnimCtrl, uint32 uiStateIndex);	// Set how to playback the animation on the specified state

	bool IsAnimLoop();
	bool IsAnimLoop(uint32 uiStateIndex);

	bool IsAnimReverse();
	bool IsAnimReverse(uint32 uiStateIndex);

	bool IsAnimBounce();
	bool IsAnimBounce(uint32 uiStateIndex);
	bool IsAnimInBouncePhase();							// True if the current anim state is supposed to 'bounce' animation, AND it is currently in the reverse/bounce part of the sequence
	void SetAnimInBouncePhase(bool bSetBouncingFlag);	// If the current anim state is set to 'bounce', this will set or unset the bounce flag, affecting the remaining of animation's playback. False means the animation has not bounced yet and is "playing forward", True means the bounce has already occurred and is "playing in reverse"

	bool IsAnimPaused();
	void SetAnimPause(bool bPause);
	
	uint32 GetNumFrames();
	uint32 GetFrame() const;
	void SetFrame(uint32 uiFrameIndex);

	float GetAnimRate() const;			// Returns the time modifier that scales each frame duration in the animation. The default value of '1.0f' will play the animation as presented in the Editor.
	void SetAnimRate(float fPlayRate);	// Scales each frame duration by 'fPlayRate'. The default value of '1.0f' will play the animation as presented in the Editor. Negative 'fPlayRate' is invalid and is clamped to 0.0f, use SetAnimCtrl(HYANIMCTRL_Reverse) instead.

	bool IsAnimFinished();				// Returns a boolean value that represents whether this animation has finished playing. A looping animation will not return true.

	float GetAnimDuration();
	void AdvanceAnim(float fDeltaTime);				// Must have valid data to use this function! - Advances the animation by fDeltaTime seconds using all the current anim attributes, ignoring whether it's paused or not. You are not required to call this function yourself.

	float GetFrameWidth(float fPercent = 1.0f);		// Returns the ALPHA-CROPPED width of the current frame (ignores any scaling)
	float GetFrameHeight(float fPercent = 1.0f);	// Returns the ALPHA-CROPPED height of the current frame (ignores any scaling)

	float GetStateWidth(uint32 uiStateIndex, float fPercent = 1.0f);	// Returns the ALPHA-CROPPED width of the widest frame in this animation state (ignores any scaling)
	float GetStateHeight(uint32 uiStateIndex, float fPercent = 1.0f);	// Returns the ALPHA-CROPPED height of the tallest frame in this animation state (ignores any scaling)

	glm::ivec2 GetCurFrameOffset();
	glm::ivec2 GetStateOffset(uint32 uiStateIndex);

	virtual bool SetState(uint32 uiStateIndex) override;
	virtual bool IsLoadDataValid() override;

protected:
	virtual void OnInvokeCallback(uint32 uiStateIndex) = 0;
	virtual bool OnIsValidToRender() override;
	virtual void OnDataAcquired() override;
	virtual void OnLoadedUpdate() override;
};

#endif /* IHySprite_h__ */
