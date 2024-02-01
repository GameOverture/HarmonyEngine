/**************************************************************************
 *	IHySprite.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2021 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "HyEngine.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/IHySprite.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable3d.h"
#include "Assets/Nodes/Objects/HySpriteData.h"
#include "Assets/Files/HyFileAtlas.h"
#include "Renderer/Components/HyVertexBuffer.h"
#include "Diagnostics/Console/IHyConsole.h"

template<typename NODETYPE, typename ENTTYPE>
IHySprite<NODETYPE, ENTTYPE>::IHySprite(const HyNodePath &nodePath, ENTTYPE *pParent) :
	NODETYPE(HYTYPE_Sprite, nodePath, pParent),
	m_bIsAnimPaused(false),
	m_fAnimPlayRate(1.0f),
	m_fElapsedFrameTime(0.0f),
	m_uiCurFrame(0)
{
	this->m_ShaderUniforms.SetNumTexUnits(1);
}

template<typename NODETYPE, typename ENTTYPE>
IHySprite<NODETYPE, ENTTYPE>::IHySprite(const IHySprite &copyRef) :
	NODETYPE(copyRef),
	m_bIsAnimPaused(copyRef.m_bIsAnimPaused),
	m_fAnimPlayRate(copyRef.m_fAnimPlayRate),
	m_fElapsedFrameTime(copyRef.m_fElapsedFrameTime),
	m_uiCurFrame(copyRef.m_uiCurFrame)
{
	for(uint32 i = 0; i < static_cast<uint32>(copyRef.m_AnimCtrlAttribList.size()); ++i)
		m_AnimCtrlAttribList.push_back(copyRef.m_AnimCtrlAttribList[i]);
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ IHySprite<NODETYPE, ENTTYPE>::~IHySprite(void)
{
}

template<typename NODETYPE, typename ENTTYPE>
const IHySprite<NODETYPE, ENTTYPE> &IHySprite<NODETYPE, ENTTYPE>::operator=(const IHySprite<NODETYPE, ENTTYPE> &rhs)
{
	NODETYPE::operator=(rhs);

	m_bIsAnimPaused = rhs.m_bIsAnimPaused;
	m_fAnimPlayRate = rhs.m_fAnimPlayRate;
	m_fElapsedFrameTime = rhs.m_fElapsedFrameTime;
	m_uiCurFrame = rhs.m_uiCurFrame;

	m_AnimCtrlAttribList.clear();
	for(uint32 i = 0; i < static_cast<uint32>(rhs.m_AnimCtrlAttribList.size()); ++i)
		m_AnimCtrlAttribList.push_back(rhs.m_AnimCtrlAttribList[i]);

	return *this;
}

template<typename NODETYPE, typename ENTTYPE>
void IHySprite<NODETYPE, ENTTYPE>::SetAnimCtrl(HyAnimCtrl eAnimCtrl)
{
	SetAnimCtrl(eAnimCtrl, this->m_uiState);
}

template<typename NODETYPE, typename ENTTYPE>
void IHySprite<NODETYPE, ENTTYPE>::SetAnimCtrl(HyAnimCtrl eAnimCtrl, uint32 uiStateIndex)
{
	while(uiStateIndex >= m_AnimCtrlAttribList.size())
		m_AnimCtrlAttribList.push_back(0);

	switch(eAnimCtrl)
	{
	case HYANIMCTRL_Reset:
	case HYANIMCTRL_ResetAndPlay:
	case HYANIMCTRL_ResetAndPause:
		m_AnimCtrlAttribList[uiStateIndex] &= ~ANIMCTRLATTRIB_IsBouncing;
		m_AnimCtrlAttribList[uiStateIndex] &= ~ANIMCTRLATTRIB_Finished;
		if(m_AnimCtrlAttribList[uiStateIndex] & ANIMCTRLATTRIB_Reverse && static_cast<const HySpriteData *>(this->AcquireData())->GetState(uiStateIndex).m_uiNUMFRAMES > 0)
			SetFrame(static_cast<const HySpriteData *>(this->AcquireData())->GetState(uiStateIndex).m_uiNUMFRAMES - 1);
		else
			SetFrame(0);
		
		if(eAnimCtrl == HYANIMCTRL_ResetAndPlay)
			m_bIsAnimPaused = false;
		else if(eAnimCtrl == HYANIMCTRL_ResetAndPause)
			m_bIsAnimPaused = true;
		break;

	case HYANIMCTRL_Reverse:
		m_AnimCtrlAttribList[uiStateIndex] |= ANIMCTRLATTRIB_Reverse;
		break;
	case HYANIMCTRL_DontReverse:
		m_AnimCtrlAttribList[uiStateIndex] &= ~ANIMCTRLATTRIB_Reverse;
		break;
	case HYANIMCTRL_Loop:
		m_AnimCtrlAttribList[uiStateIndex] |= ANIMCTRLATTRIB_Loop;
		m_AnimCtrlAttribList[uiStateIndex] &= ~ANIMCTRLATTRIB_Finished;
		break;
	case HYANIMCTRL_DontLoop:
		m_AnimCtrlAttribList[uiStateIndex] &= ~ANIMCTRLATTRIB_Loop;
		break;
	case HYANIMCTRL_Bounce:
		m_AnimCtrlAttribList[uiStateIndex] |= ANIMCTRLATTRIB_Bounce;
		break;
	case HYANIMCTRL_DontBounce:
		m_AnimCtrlAttribList[uiStateIndex] &= ~ANIMCTRLATTRIB_Bounce;
		break;
	}
}

template<typename NODETYPE, typename ENTTYPE>
bool IHySprite<NODETYPE, ENTTYPE>::IsAnimLoop()
{
	return IsAnimLoop(this->m_uiState);
}

template<typename NODETYPE, typename ENTTYPE>
bool IHySprite<NODETYPE, ENTTYPE>::IsAnimLoop(uint32 uiStateIndex)
{
	if(this->AcquireData() == nullptr) {
		HyLogDebug("IHySprite<NODETYPE, ENTTYPE>::IsAnimLoop invoked on null data");
		return false;
	}
	return (m_AnimCtrlAttribList[uiStateIndex] & ANIMCTRLATTRIB_Loop) != 0;
}

template<typename NODETYPE, typename ENTTYPE>
bool IHySprite<NODETYPE, ENTTYPE>::IsAnimReverse()
{
	return IsAnimReverse(this->m_uiState);
}

template<typename NODETYPE, typename ENTTYPE>
bool IHySprite<NODETYPE, ENTTYPE>::IsAnimReverse(uint32 uiStateIndex)
{
	if(this->AcquireData() == nullptr) {
		HyLogDebug("IHySprite<NODETYPE, ENTTYPE>::IsAnimReverse invoked on null data");
		return false;
	}

	return (m_AnimCtrlAttribList[uiStateIndex] & ANIMCTRLATTRIB_Reverse) != 0;
}

template<typename NODETYPE, typename ENTTYPE>
bool IHySprite<NODETYPE, ENTTYPE>::IsAnimBounce()
{
	return IsAnimBounce(this->m_uiState);
}

template<typename NODETYPE, typename ENTTYPE>
bool IHySprite<NODETYPE, ENTTYPE>::IsAnimBounce(uint32 uiStateIndex)
{
	if(this->AcquireData() == nullptr) {
		HyLogDebug("IHySprite<NODETYPE, ENTTYPE>::IsAnimBounce invoked on null data");
		return false;
	}
	return (m_AnimCtrlAttribList[uiStateIndex] & ANIMCTRLATTRIB_Bounce) != 0;
}

template<typename NODETYPE, typename ENTTYPE>
bool IHySprite<NODETYPE, ENTTYPE>::IsAnimInBouncePhase()
{
	if(this->AcquireData() == nullptr) {
		HyLogDebug("IHySprite<NODETYPE, ENTTYPE>::IsAnimInBouncePhase invoked on null data");
		return false;
	}

	return (m_AnimCtrlAttribList[this->m_uiState] & ANIMCTRLATTRIB_IsBouncing) != 0;
}

template<typename NODETYPE, typename ENTTYPE>
void IHySprite<NODETYPE, ENTTYPE>::SetAnimInBouncePhase(bool bSetBouncingFlag)
{
	if(IsAnimBounce(this->m_uiState) == false)
		return;

	if(bSetBouncingFlag)
		m_AnimCtrlAttribList[this->m_uiState] |= ANIMCTRLATTRIB_IsBouncing;
	else
		m_AnimCtrlAttribList[this->m_uiState] &= ~ANIMCTRLATTRIB_IsBouncing;
}

template<typename NODETYPE, typename ENTTYPE>
bool IHySprite<NODETYPE, ENTTYPE>::IsAnimPaused()
{
	return m_bIsAnimPaused;
}

template<typename NODETYPE, typename ENTTYPE>
void IHySprite<NODETYPE, ENTTYPE>::SetAnimPause(bool bPause)
{
	if(m_bIsAnimPaused == bPause)
		return;

	m_bIsAnimPaused = bPause;

	if(this->AcquireData() == nullptr)
		HyLogDebug("IHySprite<NODETYPE, ENTTYPE>::SetAnimPause invoked on null data");
	else if(m_bIsAnimPaused == false)
		m_AnimCtrlAttribList[this->m_uiState] &= ~ANIMCTRLATTRIB_Finished;
}

template<typename NODETYPE, typename ENTTYPE>
uint32 IHySprite<NODETYPE, ENTTYPE>::GetNumFrames()
{
	if(this->AcquireData() == nullptr) {
		HyLogDebug("IHySprite<NODETYPE, ENTTYPE>::GetNumFrames invoked on null data");
		return 0;
	}

	return static_cast<const HySpriteData *>(this->UncheckedGetData())->GetState(this->m_uiState).m_uiNUMFRAMES;
}

template<typename NODETYPE, typename ENTTYPE>
uint32 IHySprite<NODETYPE, ENTTYPE>::GetFrame() const
{
	return m_uiCurFrame;
}

template<typename NODETYPE, typename ENTTYPE>
void IHySprite<NODETYPE, ENTTYPE>::SetFrame(uint32 uiFrameIndex)
{
	if(this->AcquireData() == nullptr || uiFrameIndex >= static_cast<const HySpriteData *>(this->UncheckedGetData())->GetState(this->m_uiState).m_uiNUMFRAMES)
	{
		if(this->UncheckedGetData() == nullptr) {
			HyLogDebug("IHySprite<NODETYPE, ENTTYPE>::AnimSetFrame invoked on null data");
		}
		else {
			HyLogWarning("IHySprite<NODETYPE, ENTTYPE>::AnimSetFrame wants to set frame index of '" << uiFrameIndex << "' when total number of frames is '" << static_cast<const HySpriteData *>(this->AcquireData())->GetState(this->m_uiState).m_uiNUMFRAMES << "'");
		}

		return;
	}

	m_fElapsedFrameTime = 0.0f;

	if(m_uiCurFrame == uiFrameIndex)
		return;

	m_uiCurFrame = uiFrameIndex;

	const HySpriteFrame &UpdatedFrameRef = static_cast<const HySpriteData *>(this->UncheckedGetData())->GetFrame(this->m_uiState, m_uiCurFrame);
	this->m_ShaderUniforms.SetTexHandle(0, UpdatedFrameRef.GetGfxApiHandle());
	
	this->SetDirty(this->DIRTY_SceneAABB);
}

template<typename NODETYPE, typename ENTTYPE>
float IHySprite<NODETYPE, ENTTYPE>::GetAnimRate() const
{
	return m_fAnimPlayRate;
}

template<typename NODETYPE, typename ENTTYPE>
void IHySprite<NODETYPE, ENTTYPE>::SetAnimRate(float fPlayRate)
{
	m_fAnimPlayRate = HyMath::Max(0.0f, fPlayRate);
}

template<typename NODETYPE, typename ENTTYPE>
bool IHySprite<NODETYPE, ENTTYPE>::IsAnimFinished()
{
	if(this->AcquireData() == nullptr) {
		HyLogDebug("IHySprite<NODETYPE, ENTTYPE>::IsAnimFinished invoked on null data");
		return false;
	}

	return (m_AnimCtrlAttribList[this->m_uiState] & ANIMCTRLATTRIB_Finished) != 0;
}

template<typename NODETYPE, typename ENTTYPE>
float IHySprite<NODETYPE, ENTTYPE>::GetAnimDuration()
{
	if(this->AcquireData() == nullptr ||
	   this->m_uiState >= this->UncheckedGetData()->GetNumStates())
	{
		HyLogDebug("IHySprite<NODETYPE, ENTTYPE>::GetAnimDuration invoked on null data");
		return 0.0f;
	}

	return static_cast<const HySpriteData *>(this->UncheckedGetData())->GetState(this->m_uiState).m_fDURATION;
}

template<typename NODETYPE, typename ENTTYPE>
void IHySprite<NODETYPE, ENTTYPE>::AdvanceAnim(float fDeltaTime)
{
	m_fElapsedFrameTime += (fDeltaTime * m_fAnimPlayRate);

	const HySpriteFrame &frameRef = static_cast<const HySpriteData *>(this->UncheckedGetData())->GetFrame(this->m_uiState, m_uiCurFrame);
	uint8 &uiAnimCtrlRef = m_AnimCtrlAttribList[this->m_uiState];
	while(m_fElapsedFrameTime >= frameRef.fDURATION && frameRef.fDURATION > 0.0f)
	{
		int32 iNumFrames = GetNumFrames();
		int32 iNextFrameIndex = static_cast<int32>(m_uiCurFrame);
		bool bInvokeCallback = false;

		if((uiAnimCtrlRef & ANIMCTRLATTRIB_Reverse) == 0)
		{
			(uiAnimCtrlRef & ANIMCTRLATTRIB_IsBouncing) ? iNextFrameIndex-- : iNextFrameIndex++;

			if(iNextFrameIndex < 0)
			{
				if((uiAnimCtrlRef & ANIMCTRLATTRIB_Finished) == 0)
					bInvokeCallback = true;

				if(uiAnimCtrlRef & ANIMCTRLATTRIB_Loop)
				{
					uiAnimCtrlRef &= ~ANIMCTRLATTRIB_IsBouncing;
					iNextFrameIndex = 1;
				}
				else
				{
					uiAnimCtrlRef |= ANIMCTRLATTRIB_Finished;
					iNextFrameIndex = 0;
				}
			}
			else if(iNextFrameIndex >= iNumFrames)
			{
				if(uiAnimCtrlRef & ANIMCTRLATTRIB_Bounce)
				{
					uiAnimCtrlRef |= ANIMCTRLATTRIB_IsBouncing;
					iNextFrameIndex = iNumFrames - 2;
				}
				else
				{
					if((uiAnimCtrlRef & ANIMCTRLATTRIB_Finished) == 0)
						bInvokeCallback = true;

					if(uiAnimCtrlRef & ANIMCTRLATTRIB_Loop)
						iNextFrameIndex = 0;
					else
					{
						uiAnimCtrlRef |= ANIMCTRLATTRIB_Finished;
						iNextFrameIndex = iNumFrames - 1;
					}
				}
			}
		}
		else // Playing in Reverse
		{
			(uiAnimCtrlRef & ANIMCTRLATTRIB_IsBouncing) ? iNextFrameIndex++ : iNextFrameIndex--;

			if(iNextFrameIndex < 0)
			{
				if(uiAnimCtrlRef & ANIMCTRLATTRIB_Bounce)
				{
					uiAnimCtrlRef |= ANIMCTRLATTRIB_IsBouncing;
					iNextFrameIndex = 1;
				}
				else
				{
					if((uiAnimCtrlRef & ANIMCTRLATTRIB_Finished) == 0)
						bInvokeCallback = true;

					if(uiAnimCtrlRef & ANIMCTRLATTRIB_Loop)
						iNextFrameIndex = iNumFrames - 1;
					else
					{
						uiAnimCtrlRef |= ANIMCTRLATTRIB_Finished;
						iNextFrameIndex = 0;
					}
				}
			}
			else if(iNextFrameIndex >= iNumFrames)
			{
				if((uiAnimCtrlRef & ANIMCTRLATTRIB_Finished) == 0)
					bInvokeCallback = true;

				if(uiAnimCtrlRef & ANIMCTRLATTRIB_Loop)
				{
					uiAnimCtrlRef &= ~ANIMCTRLATTRIB_IsBouncing;
					iNextFrameIndex = iNumFrames - 2;
				}
				else
				{
					uiAnimCtrlRef |= ANIMCTRLATTRIB_Finished;
					iNextFrameIndex = iNumFrames - 1;
				}
			}
		}

		if(iNextFrameIndex < 0)
			iNextFrameIndex = 0;

		if(m_uiCurFrame != iNextFrameIndex)
		{
			m_uiCurFrame = iNextFrameIndex;
			this->SetDirty(this->DIRTY_SceneAABB);
		}

		m_fElapsedFrameTime -= frameRef.fDURATION;

		if(bInvokeCallback)
			OnInvokeCallback(this->m_uiState);
	}

	const HySpriteFrame &UpdatedFrameRef = static_cast<const HySpriteData *>(this->UncheckedGetData())->GetFrame(this->m_uiState, m_uiCurFrame);
	this->m_ShaderUniforms.SetTexHandle(0, UpdatedFrameRef.GetGfxApiHandle());
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ float IHySprite<NODETYPE, ENTTYPE>::GetWidth(float fPercent /*= 1.0f*/) /*override*/
{
	return GetFrameWidth(this->m_uiState, m_uiCurFrame, fPercent);
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ float IHySprite<NODETYPE, ENTTYPE>::GetHeight(float fPercent /*= 1.0f*/) /*override*/
{
	return GetFrameHeight(this->m_uiState, m_uiCurFrame, fPercent);
}

template<typename NODETYPE, typename ENTTYPE>
float IHySprite<NODETYPE, ENTTYPE>::GetFrameWidth(float fPercent /*= 1.0f*/)
{
	return GetFrameWidth(this->m_uiState, m_uiCurFrame, fPercent);
}

template<typename NODETYPE, typename ENTTYPE>
float IHySprite<NODETYPE, ENTTYPE>::GetFrameWidth(uint32 uiStateIndex, uint32 uiFrameIndex, float fPercent /*= 1.0f*/)
{
	if(this->AcquireData() == nullptr ||
		uiStateIndex >= this->UncheckedGetData()->GetNumStates() ||
		uiFrameIndex >= static_cast<const HySpriteData *>(this->UncheckedGetData())->GetState(uiStateIndex).m_uiNUMFRAMES)
	{
		HyLogDebug("IHySprite<NODETYPE, ENTTYPE>::GetFrameWidth invoked on invalid frame [State: " << uiStateIndex << ", Frame: " << uiFrameIndex << "]");
		return 0.0f;
	}

	const HySpriteFrame &frameRef = static_cast<const HySpriteData *>(this->UncheckedGetData())->GetFrame(uiStateIndex, uiFrameIndex);
	if(frameRef.pAtlas == nullptr)
		return 0.0f;

	return (frameRef.rSRC_RECT.Width() * frameRef.pAtlas->GetWidth()) * fPercent;
}

template<typename NODETYPE, typename ENTTYPE>
float IHySprite<NODETYPE, ENTTYPE>::GetFrameHeight(float fPercent /*= 1.0f*/)
{
	return GetFrameHeight(this->m_uiState, m_uiCurFrame, fPercent);
}

template<typename NODETYPE, typename ENTTYPE>
float IHySprite<NODETYPE, ENTTYPE>::GetFrameHeight(uint32 uiStateIndex, uint32 uiFrameIndex, float fPercent /*= 1.0f*/)
{
	if(this->AcquireData() == nullptr ||
		uiStateIndex >= this->UncheckedGetData()->GetNumStates() ||
		uiFrameIndex >= static_cast<const HySpriteData *>(this->UncheckedGetData())->GetState(uiStateIndex).m_uiNUMFRAMES)
	{
		HyLogDebug("IHySprite<NODETYPE, ENTTYPE>::GetHeight invoked on invalid frame [State: " << uiStateIndex << ", Frame: " << uiFrameIndex << "]");
		return 0.0f;
	}

	const HySpriteFrame &frameRef = static_cast<const HySpriteData *>(this->UncheckedGetData())->GetFrame(uiStateIndex, uiFrameIndex);
	if(frameRef.pAtlas == nullptr)
		return 0.0f;

	return (frameRef.rSRC_RECT.Height() * frameRef.pAtlas->GetHeight()) * fPercent;
}

template<typename NODETYPE, typename ENTTYPE>
float IHySprite<NODETYPE, ENTTYPE>::GetStateWidth(uint32 uiStateIndex, float fPercent /*= 1.0f*/)
{
	const HySpriteData *pData = static_cast<const HySpriteData *>(this->AcquireData());
	if(pData == nullptr ||
	   uiStateIndex >= pData->GetNumStates() ||
	   pData->GetFrame(uiStateIndex, 0).pAtlas == nullptr)
	{
		HyLogDebug("IHySprite<NODETYPE, ENTTYPE>::GetStateMaxWidth invoked on invalid data");
		return 0.0f;
	}
	
	float fMaxWidth = 0.0f;
	for(uint32 i = 0; i < pData->GetState(uiStateIndex).m_uiNUMFRAMES; ++i)
	{
		const HySpriteFrame &frameRef = pData->GetFrame(uiStateIndex, i);
		float fFrameWidth = (frameRef.rSRC_RECT.Width() * frameRef.pAtlas->GetWidth()) * fPercent;
		if(fMaxWidth < fFrameWidth)
			fMaxWidth = fFrameWidth;
	}

	return fMaxWidth;
}

template<typename NODETYPE, typename ENTTYPE>
float IHySprite<NODETYPE, ENTTYPE>::GetStateHeight(uint32 uiStateIndex, float fPercent /*= 1.0f*/)
{
	const HySpriteData *pData = static_cast<const HySpriteData *>(this->AcquireData());
	if(pData == nullptr ||
	   uiStateIndex >= pData->GetNumStates() ||
	   pData->GetFrame(uiStateIndex, 0).pAtlas == nullptr)
	{
		HyLogDebug("IHySprite<NODETYPE, ENTTYPE>::GetStateMaxHeight invoked on invalid data");
		return 0.0f;
	}

	float fMaxHeight = 0.0f;
	for(uint32 i = 0; i < pData->GetState(uiStateIndex).m_uiNUMFRAMES; ++i)
	{
		const HySpriteFrame &frameRef = pData->GetFrame(uiStateIndex, i);
		float fFrameHeight = (frameRef.rSRC_RECT.Height() * frameRef.pAtlas->GetHeight()) * fPercent;
		if(fMaxHeight < fFrameHeight)
			fMaxHeight = fFrameHeight;
	}

	return fMaxHeight;
}

template<typename NODETYPE, typename ENTTYPE>
glm::ivec2 IHySprite<NODETYPE, ENTTYPE>::GetCurFrameOffset()
{
	if(this->AcquireData() == nullptr ||
	   this->m_uiState >= this->UncheckedGetData()->GetNumStates() ||
	   m_uiCurFrame >= static_cast<const HySpriteData *>(this->UncheckedGetData())->GetState(this->m_uiState).m_uiNUMFRAMES)
	{
		HyLogDebug("IHySprite<NODETYPE, ENTTYPE>::GetCurFrameOffset invoked on invalid data");
		return glm::ivec2(0);
	}

	const HySpriteFrame &frameRef = static_cast<const HySpriteData *>(this->UncheckedGetData())->GetFrame(this->m_uiState, m_uiCurFrame);
	return frameRef.vOFFSET;
}

template<typename NODETYPE, typename ENTTYPE>
glm::ivec2 IHySprite<NODETYPE, ENTTYPE>::GetStateOffset(uint32 uiStateIndex)
{
	const HySpriteData *pData = static_cast<const HySpriteData *>(this->AcquireData());
	if(pData == nullptr ||
	   uiStateIndex >= this->UncheckedGetData()->GetNumStates())
	{
		HyLogDebug("IHySprite<NODETYPE, ENTTYPE>::GetCurFrameOffset invoked on invalid data");
		return glm::vec2(0);
	}

	glm::ivec2 vMaxOffset(0, 0);
	for(uint32 i = 0; i < pData->GetState(uiStateIndex).m_uiNUMFRAMES; ++i)
	{
		const HySpriteFrame &frameRef = pData->GetFrame(uiStateIndex, i);
		if(abs(vMaxOffset.x) < abs(frameRef.vOFFSET.x))
			vMaxOffset.x = frameRef.vOFFSET.x;
		if(abs(vMaxOffset.y) < abs(frameRef.vOFFSET.y))
			vMaxOffset.y = frameRef.vOFFSET.y;
	}
	
	return vMaxOffset;
}

template<typename NODETYPE, typename ENTTYPE>
bool IHySprite<NODETYPE, ENTTYPE>::IsBoundsIncludeAlphaCrop()
{
	return IsBoundsIncludeAlphaCrop(this->m_uiState);
}

template<typename NODETYPE, typename ENTTYPE>
bool IHySprite<NODETYPE, ENTTYPE>::IsBoundsIncludeAlphaCrop(uint32 uiStateIndex)
{
	if(this->AcquireData() == nullptr) {
		HyLogDebug("IHySprite<NODETYPE, ENTTYPE>::IsBoundsIncludeAlphaCrop invoked on null data");
		return false;
	}
	return (m_AnimCtrlAttribList[uiStateIndex] & ANIMCTRLATTRIB_BoundsIncludeAlphaCrop) != 0;
}

template<typename NODETYPE, typename ENTTYPE>
void IHySprite<NODETYPE, ENTTYPE>::SetBoundsIncludeAlphaCrop(bool bIncludeAlphaCrop, uint32 uiStateIndex)
{
	while(uiStateIndex >= m_AnimCtrlAttribList.size())
		m_AnimCtrlAttribList.push_back(0);

	if(bIncludeAlphaCrop)
		m_AnimCtrlAttribList[uiStateIndex] |= ANIMCTRLATTRIB_BoundsIncludeAlphaCrop;
	else
		m_AnimCtrlAttribList[uiStateIndex] &= ~ANIMCTRLATTRIB_BoundsIncludeAlphaCrop;
}

template<typename NODETYPE, typename ENTTYPE>
void IHySprite<NODETYPE, ENTTYPE>::SetAllBoundsIncludeAlphaCrop(bool bIncludeAlphaCrop)
{
	if(this->AcquireData() == nullptr) {
		HyLogWarning("IHySprite<NODETYPE, ENTTYPE>::SetAllBoundsIncludeAlphaCrop invoked on null data. This call will have no effect!");
		return;
	}

	for(uint32 i = 0; i < this->UncheckedGetData()->GetNumStates(); ++i)
		SetBoundsIncludeAlphaCrop(bIncludeAlphaCrop, i);
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ bool IHySprite<NODETYPE, ENTTYPE>::SetState(uint32 uiStateIndex) /*override*/
{
	if(this->m_uiState == uiStateIndex || IHyLoadable::SetState(uiStateIndex) == false)
		return false;

	while(this->m_uiState >= m_AnimCtrlAttribList.size())
		m_AnimCtrlAttribList.push_back(0);

	if(0 == (m_AnimCtrlAttribList[this->m_uiState] & ANIMCTRLATTRIB_Reverse) || GetNumFrames() == 0)
		m_uiCurFrame = 0;
	else
		m_uiCurFrame = GetNumFrames() - 1;

	const HySpriteFrame &UpdatedFrameRef = static_cast<const HySpriteData *>(this->UncheckedGetData())->GetFrame(this->m_uiState, m_uiCurFrame);
	this->m_ShaderUniforms.SetTexHandle(0, UpdatedFrameRef.GetGfxApiHandle());

	this->SetDirty(this->DIRTY_SceneAABB);
	return true;
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ bool IHySprite<NODETYPE, ENTTYPE>::IsLoadDataValid() /*override*/
{
	const HySpriteData *pData = static_cast<const HySpriteData *>(this->AcquireData());
	return pData && pData->GetNumStates() != 0;
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ bool IHySprite<NODETYPE, ENTTYPE>::OnIsValidToRender() /*override*/
{
	return (m_AnimCtrlAttribList[this->m_uiState] & ANIMCTRLATTRIB_Invalid) == 0;
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ void IHySprite<NODETYPE, ENTTYPE>::OnDataAcquired() /*override*/
{
	const HySpriteData *pData = static_cast<const HySpriteData *>(this->UncheckedGetData());
	uint32 uiNumStates = pData->GetNumStates();

	while(m_AnimCtrlAttribList.size() > uiNumStates)
		m_AnimCtrlAttribList.pop_back();

	while(m_AnimCtrlAttribList.size() < uiNumStates)
		m_AnimCtrlAttribList.push_back(0);

	for(uint32 i = 0; i < uiNumStates; ++i)
	{
		const HySpriteData::AnimState &stateRef = pData->GetState(i);

		if(stateRef.m_bLOOP)
			m_AnimCtrlAttribList[i] |= ANIMCTRLATTRIB_Loop;
		if(stateRef.m_bBOUNCE)
			m_AnimCtrlAttribList[i] |= ANIMCTRLATTRIB_Bounce;
		if(stateRef.m_bREVERSE)
			m_AnimCtrlAttribList[i] |= ANIMCTRLATTRIB_Reverse;

		if(stateRef.m_uiNUMFRAMES == 0 || stateRef.GetFrame(0).IsAtlasValid() == false)
			m_AnimCtrlAttribList[i] |= ANIMCTRLATTRIB_Invalid;
		else
			m_AnimCtrlAttribList[i] &= ~ANIMCTRLATTRIB_Invalid;
	}
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ void IHySprite<NODETYPE, ENTTYPE>::OnLoadedUpdate() /*override*/
{
	if(m_bIsAnimPaused == false)
		AdvanceAnim(HyEngine::DeltaTime());
	else
		AdvanceAnim(0.0f); // AdvanceAnim() should still be called to update the shader uniform
}

template class IHySprite<IHyDrawable2d, HyEntity2d>;
template class IHySprite<IHyDrawable3d, HyEntity3d>;
