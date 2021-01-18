/**************************************************************************
 *	IHySprite.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/Objects/IHySprite.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/IHyInstance2d.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/IHyInstance3d.h"
#include "Assets/Nodes/HySprite2dData.h"
#include "Renderer/Components/HyVertexBuffer.h"
#include "Diagnostics/Console/HyConsole.h"

extern float Hy_UpdateStep();

template<typename NODETYPE, typename ENTTYPE>
IHySprite<NODETYPE, ENTTYPE>::IHySprite(std::string sPrefix /*= ""*/, std::string sName /*= ""*/, ENTTYPE *pParent /*= nullptr*/) :
	NODETYPE(HYTYPE_Sprite, sPrefix, sName, pParent),
	m_bIsAnimPaused(false),
	m_fAnimPlayRate(1.0f),
	m_fElapsedFrameTime(0.0f),
	m_uiCurAnimState(0),
	m_uiCurFrame(0),
	m_vCustomOffset(0, 0)
{
	m_eRenderMode = HYRENDERMODE_TriangleStrip;
}

template<typename NODETYPE, typename ENTTYPE>
IHySprite<NODETYPE, ENTTYPE>::IHySprite(const IHySprite &copyRef) :
	NODETYPE(copyRef),
	m_bIsAnimPaused(copyRef.m_bIsAnimPaused),
	m_fAnimPlayRate(copyRef.m_fAnimPlayRate),
	m_fElapsedFrameTime(copyRef.m_fElapsedFrameTime),
	m_uiCurAnimState(copyRef.m_uiCurAnimState),
	m_uiCurFrame(copyRef.m_uiCurFrame)
{
	for(uint32 i = 0; i < static_cast<uint32>(copyRef.m_AnimCtrlAttribList.size()); ++i)
		m_AnimCtrlAttribList.push_back(copyRef.m_AnimCtrlAttribList[i]);
}

template<typename NODETYPE, typename ENTTYPE>
IHySprite<NODETYPE, ENTTYPE>::~IHySprite(void)
{
}

template<typename NODETYPE, typename ENTTYPE>
const IHySprite<NODETYPE, ENTTYPE> &IHySprite<NODETYPE, ENTTYPE>::operator=(const IHySprite<NODETYPE, ENTTYPE> &rhs)
{
	NODETYPE::operator=(rhs);

	m_bIsAnimPaused = rhs.m_bIsAnimPaused;
	m_fAnimPlayRate = rhs.m_fAnimPlayRate;
	m_fElapsedFrameTime = rhs.m_fElapsedFrameTime;
	m_uiCurAnimState = rhs.m_uiCurAnimState;
	m_uiCurFrame = rhs.m_uiCurFrame;

	m_AnimCtrlAttribList.clear();
	for(uint32 i = 0; i < static_cast<uint32>(rhs.m_AnimCtrlAttribList.size()); ++i)
		m_AnimCtrlAttribList.push_back(rhs.m_AnimCtrlAttribList[i]);

	return *this;
}

template<typename NODETYPE, typename ENTTYPE>
void IHySprite<NODETYPE, ENTTYPE>::AnimCtrl(HyAnimCtrl eAnimCtrl)
{
	AnimCtrl(eAnimCtrl, m_uiCurAnimState);
}

template<typename NODETYPE, typename ENTTYPE>
void IHySprite<NODETYPE, ENTTYPE>::AnimCtrl(HyAnimCtrl eAnimCtrl, uint32 uiAnimState)
{
	while(uiAnimState >= m_AnimCtrlAttribList.size())
		m_AnimCtrlAttribList.push_back(0);

	switch(eAnimCtrl)
	{
	case HYANIMCTRL_Play:
		m_bIsAnimPaused = false;
		m_AnimCtrlAttribList[uiAnimState] &= ~ANIMCTRLATTRIB_Finished;
		break;
	case HYANIMCTRL_Reset:
		m_AnimCtrlAttribList[uiAnimState] &= ~ANIMCTRLATTRIB_IsBouncing;
		m_AnimCtrlAttribList[uiAnimState] &= ~ANIMCTRLATTRIB_Finished;
		if(m_AnimCtrlAttribList[uiAnimState] & ANIMCTRLATTRIB_Reverse && static_cast<const HySprite2dData *>(AcquireData())->GetState(uiAnimState).m_uiNUMFRAMES > 0)
			AnimSetFrame(static_cast<const HySprite2dData *>(AcquireData())->GetState(uiAnimState).m_uiNUMFRAMES - 1);
		else
			AnimSetFrame(0);
		break;

	case HYANIMCTRL_Reverse:
		m_AnimCtrlAttribList[uiAnimState] |= ANIMCTRLATTRIB_Reverse;
		break;
	case HYANIMCTRL_DontReverse:
		m_AnimCtrlAttribList[uiAnimState] &= ~ANIMCTRLATTRIB_Reverse;
		break;
	case HYANIMCTRL_Loop:
		m_AnimCtrlAttribList[uiAnimState] |= ANIMCTRLATTRIB_Loop;
		m_AnimCtrlAttribList[uiAnimState] &= ~ANIMCTRLATTRIB_Finished;
		break;
	case HYANIMCTRL_DontLoop:
		m_AnimCtrlAttribList[uiAnimState] &= ~ANIMCTRLATTRIB_Loop;
		break;
	case HYANIMCTRL_Bounce:
		m_AnimCtrlAttribList[uiAnimState] |= ANIMCTRLATTRIB_Bounce;
		break;
	case HYANIMCTRL_DontBounce:
		m_AnimCtrlAttribList[uiAnimState] &= ~ANIMCTRLATTRIB_Bounce;
		break;
	}
}

template<typename NODETYPE, typename ENTTYPE>
bool IHySprite<NODETYPE, ENTTYPE>::AnimIsReverse()
{
	return AnimIsReverse(m_uiCurAnimState);
}

template<typename NODETYPE, typename ENTTYPE>
bool IHySprite<NODETYPE, ENTTYPE>::AnimIsReverse(uint32 uiAnimState)
{
	if(AcquireData() == nullptr) {
		HyLogWarning("IHySprite<NODETYPE, ENTTYPE>::AnimIsReverse invoked on null data");
		return false;
	}

	return (m_AnimCtrlAttribList[m_uiCurAnimState] & ANIMCTRLATTRIB_Reverse) != 0;
}

template<typename NODETYPE, typename ENTTYPE>
uint32 IHySprite<NODETYPE, ENTTYPE>::AnimGetNumStates()
{
	if(AcquireData() == nullptr) {
		HyLogWarning("IHySprite<NODETYPE, ENTTYPE>::AnimGetNumStates invoked on null data");
		return 0;
	}
	
	return static_cast<const HySprite2dData *>(UncheckedGetData())->GetNumStates();
}

template<typename NODETYPE, typename ENTTYPE>
uint32 IHySprite<NODETYPE, ENTTYPE>::AnimGetState() const
{
	return m_uiCurAnimState;
}

template<typename NODETYPE, typename ENTTYPE>
uint32 IHySprite<NODETYPE, ENTTYPE>::AnimGetNumFrames()
{
	if(AcquireData() == nullptr) {
		HyLogWarning("IHySprite<NODETYPE, ENTTYPE>::AnimGetNumFrames invoked on null data");
		return 0;
	}

	return static_cast<const HySprite2dData *>(UncheckedGetData())->GetState(m_uiCurAnimState).m_uiNUMFRAMES;
}

template<typename NODETYPE, typename ENTTYPE>
uint32 IHySprite<NODETYPE, ENTTYPE>::AnimGetFrame() const
{
	return m_uiCurFrame;
}

template<typename NODETYPE, typename ENTTYPE>
void IHySprite<NODETYPE, ENTTYPE>::AnimSetFrame(uint32 uiFrameIndex)
{
	if(AcquireData() == nullptr || uiFrameIndex >= static_cast<const HySprite2dData *>(UncheckedGetData())->GetState(m_uiCurAnimState).m_uiNUMFRAMES)
	{
		if(UncheckedGetData() == nullptr) {
			HyLogWarning("IHySprite<NODETYPE, ENTTYPE>::AnimSetFrame invoked on null data");
		}
		else {
			HyLogWarning("IHySprite<NODETYPE, ENTTYPE>::AnimSetFrame wants to set frame index of '" << uiFrameIndex << "' when total number of frames is '" << static_cast<const HySprite2dData *>(AcquireData())->GetState(m_uiCurAnimState).m_uiNUMFRAMES << "'");
		}

		return;
	}

	if(m_uiCurFrame == uiFrameIndex)
		return;

	m_uiCurFrame = uiFrameIndex;

	const HySprite2dFrame &UpdatedFrameRef = static_cast<const HySprite2dData *>(UncheckedGetData())->GetFrame(m_uiCurAnimState, m_uiCurFrame);
	m_hTextureHandle = UpdatedFrameRef.GetGfxApiHandle();
	
	SetDirty(DIRTY_BoundingVolume);
}

template<typename NODETYPE, typename ENTTYPE>
float IHySprite<NODETYPE, ENTTYPE>::AnimGetPlayRate() const
{
	return m_fAnimPlayRate;
}

template<typename NODETYPE, typename ENTTYPE>
void IHySprite<NODETYPE, ENTTYPE>::AnimSetPlayRate(float fPlayRate)
{
	if(fPlayRate < 0.0f)
		fPlayRate = 0.0f;

	m_fAnimPlayRate = fPlayRate;
}

template<typename NODETYPE, typename ENTTYPE>
void IHySprite<NODETYPE, ENTTYPE>::AnimSetState(uint32 uiStateIndex)
{
	if(AcquireData() == nullptr || uiStateIndex >= static_cast<const HySprite2dData *>(UncheckedGetData())->GetNumStates())
	{
		if(UncheckedGetData() == nullptr) {
			HyLogWarning("IHySprite<NODETYPE, ENTTYPE>::AnimSetState invoked on null data");
		}
		else if(uiStateIndex >= static_cast<const HySprite2dData *>(UncheckedGetData())->GetNumStates()) {
			HyLogWarning(m_sPrefix << "/" << m_sName << " (HySprite) wants to set state index of '" << uiStateIndex << "' when total number of states is '" << static_cast<const HySprite2dData *>(AcquireData())->GetNumStates() << "'");
		}
		
		return;
	}

	if(m_uiCurAnimState == uiStateIndex)
		return;

	m_uiCurAnimState = uiStateIndex;

	while(m_uiCurAnimState >= m_AnimCtrlAttribList.size())
		m_AnimCtrlAttribList.push_back(0);

	if(0 == (m_AnimCtrlAttribList[m_uiCurAnimState] & ANIMCTRLATTRIB_Reverse) || AnimGetNumFrames() == 0)
		m_uiCurFrame = 0;
	else
		m_uiCurFrame = AnimGetNumFrames() - 1;

	const HySprite2dFrame &UpdatedFrameRef = static_cast<const HySprite2dData *>(UncheckedGetData())->GetFrame(m_uiCurAnimState, m_uiCurFrame);
	m_hTextureHandle = UpdatedFrameRef.GetGfxApiHandle();

	SetDirty(DIRTY_BoundingVolume);
}

template<typename NODETYPE, typename ENTTYPE>
bool IHySprite<NODETYPE, ENTTYPE>::AnimIsFinished()
{
	if(AcquireData() == nullptr) {
		HyLogWarning("IHySprite<NODETYPE, ENTTYPE>::AnimIsFinished invoked on null data");
		return false;
	}

	return (m_AnimCtrlAttribList[m_uiCurAnimState] & ANIMCTRLATTRIB_Finished) != 0;
}

template<typename NODETYPE, typename ENTTYPE>
bool IHySprite<NODETYPE, ENTTYPE>::AnimIsPaused()
{
	return m_bIsAnimPaused;
}

template<typename NODETYPE, typename ENTTYPE>
void IHySprite<NODETYPE, ENTTYPE>::AnimSetPause(bool bPause)
{
	if(AcquireData() == nullptr) {
		HyLogWarning("IHySprite<NODETYPE, ENTTYPE>::AnimSetPause invoked on null data");
		return;
	}

	if(m_bIsAnimPaused == bPause)
		return;

	m_bIsAnimPaused = bPause;
	m_fElapsedFrameTime = 0.0f;

	if(m_bIsAnimPaused == false)
		m_AnimCtrlAttribList[m_uiCurAnimState] &= ~ANIMCTRLATTRIB_Finished;
}

template<typename NODETYPE, typename ENTTYPE>
float IHySprite<NODETYPE, ENTTYPE>::AnimGetDuration()
{
	if(AcquireData() == nullptr) {
		HyLogWarning("IHySprite<NODETYPE, ENTTYPE>::AnimGetDuration invoked on null data");
		return 0.0f;
	}

	return static_cast<const HySprite2dData *>(UncheckedGetData())->GetState(m_uiCurAnimState).m_fDURATION;
}

template<typename NODETYPE, typename ENTTYPE>
float IHySprite<NODETYPE, ENTTYPE>::AnimGetCurFrameWidth(bool bIncludeScaling /*= true*/)
{
	if(AcquireData() == nullptr) {
		HyLogWarning("IHySprite<NODETYPE, ENTTYPE>::AnimGetCurFrameWidth invoked on null data");
		return 0.0f;
	}

	const HySprite2dFrame &frameRef = static_cast<const HySprite2dData *>(UncheckedGetData())->GetFrame(m_uiCurAnimState, m_uiCurFrame);
	if(frameRef.pAtlas == nullptr)
		return 0.0f;

	glm::vec3 vScale(1.0f);
	if(bIncludeScaling)
	{
		glm::quat quatRot;
		glm::vec3 ptTranslation;
		glm::vec3 vSkew;
		glm::vec4 vPerspective;
		glm::decompose(GetWorldTransform(), vScale, quatRot, ptTranslation, vSkew, vPerspective);
	}

	return frameRef.rSRC_RECT.Width() * frameRef.pAtlas->GetWidth() * vScale.x;
}

template<typename NODETYPE, typename ENTTYPE>
float IHySprite<NODETYPE, ENTTYPE>::AnimGetCurFrameHeight(bool bIncludeScaling /*= true*/)
{
	if(AcquireData() == nullptr) {
		HyLogWarning("IHySprite<NODETYPE, ENTTYPE>::AnimGetCurFrameHeight invoked on null data");
		return 0.0f;
	}

	const HySprite2dFrame &frameRef = static_cast<const HySprite2dData *>(UncheckedGetData())->GetFrame(m_uiCurAnimState, m_uiCurFrame);
	if(frameRef.pAtlas == nullptr)
		return 0.0f;

	glm::vec3 vScale(1.0f);
	if(bIncludeScaling)
	{
		glm::quat quatRot;
		glm::vec3 ptTranslation;
		glm::vec3 vSkew;
		glm::vec4 vPerspective;
		glm::decompose(GetWorldTransform(), vScale, quatRot, ptTranslation, vSkew, vPerspective);
	}

	return ((frameRef.rSRC_RECT.Height() * frameRef.pAtlas->GetHeight()) /*+ frameRef.vOFFSET.x*/) * vScale.y;
}

template<typename NODETYPE, typename ENTTYPE>
float IHySprite<NODETYPE, ENTTYPE>::AnimGetMaxWidth(uint32 uiStateIndex, bool bIncludeScaling /*= true*/)
{
	const HySprite2dData *pData = static_cast<const HySprite2dData *>(AcquireData());
	if(pData == nullptr) {
		HyLogWarning("IHySprite<NODETYPE, ENTTYPE>::AnimGetMaxWidth invoked on null data");
		return 0.0f;
	}

	glm::vec3 vScale(1.0f);
	if(bIncludeScaling)
	{
		glm::quat quatRot;
		glm::vec3 ptTranslation;
		glm::vec3 vSkew;
		glm::vec4 vPerspective;
		glm::decompose(GetWorldTransform(), vScale, quatRot, ptTranslation, vSkew, vPerspective);
	}
	
	float fMaxWidth = 0.0f;
	for(uint32 i = 0; i < pData->GetState(uiStateIndex).m_uiNUMFRAMES; ++i)
	{
		const HySprite2dFrame &frameRef = pData->GetFrame(uiStateIndex, i);
		float fFrameWidth = frameRef.rSRC_RECT.Width() * frameRef.pAtlas->GetWidth() * vScale.x;
		if(fMaxWidth < fFrameWidth)
			fMaxWidth = fFrameWidth;
	}

	return fMaxWidth;
}

template<typename NODETYPE, typename ENTTYPE>
float IHySprite<NODETYPE, ENTTYPE>::AnimGetMaxHeight(uint32 uiStateIndex, bool bIncludeScaling /*= true*/)
{
	const HySprite2dData *pData = static_cast<const HySprite2dData *>(AcquireData());
	if(pData == nullptr) {
		HyLogWarning("IHySprite<NODETYPE, ENTTYPE>::AnimGetMaxHeight invoked on null data");
		return 0.0f;
	}

	glm::vec3 vScale(1.0f);
	if(bIncludeScaling)
	{
		glm::quat quatRot;
		glm::vec3 ptTranslation;
		glm::vec3 vSkew;
		glm::vec4 vPerspective;
		glm::decompose(GetWorldTransform(), vScale, quatRot, ptTranslation, vSkew, vPerspective);
	}

	float fMaxHeight = 0.0f;
	for(uint32 i = 0; i < pData->GetState(uiStateIndex).m_uiNUMFRAMES; ++i)
	{
		const HySprite2dFrame &frameRef = pData->GetFrame(uiStateIndex, i);
		float fFrameHeight = frameRef.rSRC_RECT.Height() * frameRef.pAtlas->GetHeight() * vScale.y;
		if(fMaxHeight < fFrameHeight)
			fMaxHeight = fFrameHeight;
	}

	return fMaxHeight;
}

template<typename NODETYPE, typename ENTTYPE>
void IHySprite<NODETYPE, ENTTYPE>::SetUserOffset(int32 iOffsetX, int32 iOffsetY)
{
	m_vCustomOffset.x = iOffsetX;
	m_vCustomOffset.y = iOffsetY;
}

template<typename NODETYPE, typename ENTTYPE>
glm::ivec2 IHySprite<NODETYPE, ENTTYPE>::AnimGetCurFrameOffset()
{
	const HySprite2dFrame &frameRef = static_cast<const HySprite2dData *>(AcquireData())->GetFrame(m_uiCurAnimState, m_uiCurFrame);
	return frameRef.vOFFSET + m_vCustomOffset;
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ bool IHySprite<NODETYPE, ENTTYPE>::IsLoadDataValid() /*override*/
{
	const HySprite2dData *pData = static_cast<const HySprite2dData *>(AcquireData());
	return pData && pData->GetNumStates() != 0;
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ bool IHySprite<NODETYPE, ENTTYPE>::OnIsValidToRender() /*override*/
{
	return ((m_AnimCtrlAttribList[m_uiCurAnimState] & ANIMCTRLATTRIB_Invalid) == 0);
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ void IHySprite<NODETYPE, ENTTYPE>::OnDataAcquired() /*override*/
{
	const HySprite2dData *pData = static_cast<const HySprite2dData *>(UncheckedGetData());
	uint32 uiNumStates = pData->GetNumStates();

	while(m_AnimCtrlAttribList.size() > uiNumStates)
		m_AnimCtrlAttribList.pop_back();

	while(m_AnimCtrlAttribList.size() < uiNumStates)
		m_AnimCtrlAttribList.push_back(0);

	for(uint32 i = 0; i < uiNumStates; ++i)
	{
		const HySprite2dData::AnimState &stateRef = pData->GetState(i);

		if(stateRef.m_bLOOP)
			m_AnimCtrlAttribList[i] |= ANIMCTRLATTRIB_Loop;
		if(stateRef.m_bBOUNCE)
			m_AnimCtrlAttribList[i] |= ANIMCTRLATTRIB_Bounce;
		if(stateRef.m_bREVERSE)
			m_AnimCtrlAttribList[i] |= ANIMCTRLATTRIB_Reverse;

		if(stateRef.m_uiNUMFRAMES == 0 || stateRef.GetFrame(0).IsValid() == false)
			m_AnimCtrlAttribList[i] |= ANIMCTRLATTRIB_Invalid;
		else
			m_AnimCtrlAttribList[i] &= ~ANIMCTRLATTRIB_Invalid;
	}
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ void IHySprite<NODETYPE, ENTTYPE>::OnLoadedUpdate() /*override*/
{
	if(m_bIsAnimPaused == false)
		m_fElapsedFrameTime += Hy_UpdateStep() * m_fAnimPlayRate;

	const HySprite2dFrame &frameRef = static_cast<const HySprite2dData *>(UncheckedGetData())->GetFrame(m_uiCurAnimState, m_uiCurFrame);
	uint8 &uiAnimCtrlRef = m_AnimCtrlAttribList[m_uiCurAnimState];
	while(m_fElapsedFrameTime >= frameRef.fDURATION && frameRef.fDURATION > 0.0f)
	{
		int32 iNumFrames = AnimGetNumFrames();
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
			SetDirty(DIRTY_BoundingVolume);
		}

		m_fElapsedFrameTime -= frameRef.fDURATION;

		if(bInvokeCallback)
			OnInvokeCallback(m_uiCurAnimState);
	}

	const HySprite2dFrame &UpdatedFrameRef = static_cast<const HySprite2dData *>(this->UncheckedGetData())->GetFrame(m_uiCurAnimState, m_uiCurFrame);
	m_hTextureHandle = UpdatedFrameRef.GetGfxApiHandle();
}

template class IHySprite<IHyInstance2d, HyEntity2d>;
template class IHySprite<IHyInstance3d, HyEntity3d>;
