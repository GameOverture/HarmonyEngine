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
	HyRect							m_LocalBoundingRect;

#ifdef HY_USE_SPINE
	spine::Skeleton *				m_pSkeleton;
	spine::AnimationState *			m_pAnimationState;
	spine::SkeletonBounds *			m_pSkeletonBounds;

	static spine::SkeletonRenderer	sm_Renderer;			// Used in the HyRenderBuffer::AppendRenderState do/while loop
	spine::RenderCommand *			m_pRenderCmd;			// Used in the HyRenderBuffer::AppendRenderState do/while loop
#endif

public:
	HySpine2d(HyEntity2d *pParent = nullptr);
	HySpine2d(const HyNodePath &nodePath, HyEntity2d *pParent = nullptr);
	HySpine2d(const char *szPrefix, const char *szName, HyEntity2d *pParent = nullptr);
	HySpine2d(const HySpine2d &copyRef);
	virtual ~HySpine2d(void);

	const HySpine2d &operator=(const HySpine2d &rhs);

	virtual void CalcLocalBoundingShape(HyShape2d &shapeOut) override;
	virtual float GetWidth(float fPercent) override;
	virtual float GetHeight(float fPercent) override;

	virtual bool SetState(uint32 uiStateIndex) override;
	virtual bool IsLoadDataValid() override;

#ifdef HY_USE_SPINE
	spine::Animation *GetAnim(int iIndex);
	spine::Animation *GetAnim(std::string sAnimName);

	// @return The track entry for the animation currently playing on the track, or nullptr if no animation is currently playing.
	spine::TrackEntry *GetCurrentTrack(size_t uiTrackIndex);
	
	// Removes all animations from all tracks, leaving skeletons in their previous pose.
	// It may be desired to use SetEmptyAnimations(float) to mix the skeletons back to the setup pose,
	// rather than leaving them in their previous pose.
	void ClearTracks();

	// Removes all animations from the specified track, leaving skeletons in their previous pose.
	// It may be desired to use SetEmptyAnimations(float) to mix the skeletons back to the setup pose,
	// rather than leaving them in their previous pose.
	void ClearTrack(size_t uiTrackIndex);

	// When a track has no current animation and an animation is set, it begins playing right away. To instead mix in the animation from the setup pose, an empty animation can be used.
	// Sets an empty animation for every track, discarding any queued animations, and mixes to it over the specified mix duration.
	void SetEmptyAnimations(float fMixDuration);

	// When a track has no current animation and an animation is set, it begins playing right away. To instead mix in the animation from the setup pose, an empty animation can be used.
	// Sets an empty animation for a track, discarding any queued animations, and mixes to it over the specified mix duration.
	spine::TrackEntry *SetEmptyAnimation(size_t uiTrackIndex, float fMixDuration);

	// When a track is cleared, the track's animations are no longer applied, leaving the skeletons in their current pose. To instead mix out the animation to the setup pose, an empty animation can be used.
	// Adds an empty animation to be played after the current or last queued animation for a track, and mixes to it over the specified mix duration.
	// @return
	// A track entry to allow further customization of animation playback. References to the track entry must not be kept after AnimationState.Dispose.
	// @param trackIndex Track number.
	// @param fMixDuration Mix duration.
	// @param delay Seconds to begin this animation after the start of the previous animation. May be <= 0 to use the animation
	// duration of the previous track minus any mix duration plus the negative delay.
	spine::TrackEntry *AddEmptyAnimation(size_t uiTrackIndex, float fMixDuration, float fDelay);

	// Sets the current animation for a track, discarding any queued animations.
	// 'bLoop' If true, the animation will repeat.
	// If false, it will not, instead its last frame is applied if played beyond its duration.
	// In either case TrackEntry.TrackEnd determines when the track is cleared.
	// @return a track entry to allow further customization of animation playback. References to the track entry must not be kept after AnimationState.Dispose.
	spine::TrackEntry *SetAnimation(size_t uiTrackIndex, spine::Animation *pAnimation, bool bLoop);

	// Adds an animation to be played delay seconds after the current or last queued animation
	// for a track. If the track is empty, it is equivalent to calling setAnimation.
	// @param delay
	// Seconds to begin this animation after the start of the previous animation. May be <= 0 to use the animation
	// duration of the previous track minus any mix duration plus the negative delay.
	// @return a track entry to allow further customization of animation playback. References to the track entry must not be kept after AnimationState.Dispose
	spine::TrackEntry *AddAnimation(size_t uiTrackIndex, spine::Animation *pAnimation, bool bLoop, float fDelay);
#endif

	float GetAnimRate() const;			// Returns the time modifier that scales each animation duration. The default value of '1.0f' will play the animation as presented in the Editor.
	void SetAnimRate(float fPlayRate);	// Scales each frame duration by 'fPlayRate'. The default value of '1.0f' will play the animation as presented in the Editor. Negative 'fPlayRate' is invalid and is clamped to 0.0f

protected:
	virtual void SetDirty(uint32 uiDirtyFlags) override;
	virtual void OnUpdateUniforms(float fExtrapolatePercent) override;
	virtual bool OnIsValidToRender() override;
	virtual void OnDataAcquired() override;
	virtual void OnLoadedUpdate() override;

	virtual void PrepRenderStage(uint32 uiStageIndex, HyRenderMode &eRenderModeOut, HyBlendMode &eBlendModeOut, uint32 &uiNumInstancesOut, uint32 &uiNumVerticesPerInstOut, bool &bIsBatchable) override;
	virtual bool WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef, float fExtrapolatePercent) override;
};

#endif /* HySpine2d_h__ */
