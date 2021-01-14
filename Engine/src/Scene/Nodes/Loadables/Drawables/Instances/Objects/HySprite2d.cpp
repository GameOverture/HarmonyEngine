/**************************************************************************
 *	HySprite2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/Objects/HySprite2d.h"
#include "HyEngine.h"
#include "Scene/Nodes/Loadables/Drawables/Objects/HyEntity2d.h"
#include "Diagnostics/Console/HyConsole.h"
#include "Assets/Nodes/HySprite2dData.h"

extern float Hy_UpdateStep();

HySprite2d::HySprite2d(std::string sPrefix /*= ""*/, std::string sName /*= ""*/, HyEntity2d *pParent /*= nullptr*/) :
	IHyInstance2d(HYTYPE_Sprite, sPrefix, sName, pParent),
	m_bIsAnimPaused(false),
	m_fAnimPlayRate(1.0f),
	m_fElapsedFrameTime(0.0f),
	m_uiCurAnimState(0),
	m_uiCurFrame(0),
	m_vCustomOffset(0, 0)
{
	m_eRenderMode = HYRENDERMODE_TriangleStrip;
}

HySprite2d::HySprite2d(const HySprite2d &copyRef) :
	IHyInstance2d(copyRef),
	m_bIsAnimPaused(copyRef.m_bIsAnimPaused),
	m_fAnimPlayRate(copyRef.m_fAnimPlayRate),
	m_fElapsedFrameTime(copyRef.m_fElapsedFrameTime),
	m_uiCurAnimState(copyRef.m_uiCurAnimState),
	m_uiCurFrame(copyRef.m_uiCurFrame)
{
	for(uint32 i = 0; i < static_cast<uint32>(copyRef.m_AnimCtrlAttribList.size()); ++i)
		m_AnimCtrlAttribList.push_back(copyRef.m_AnimCtrlAttribList[i]);

	for(uint32 i = 0; i < static_cast<uint32>(copyRef.m_AnimCallbackList.size()); ++i)
		m_AnimCallbackList.push_back(std::pair<HySprite2dAnimFinishedCallback, void *>(copyRef.m_AnimCallbackList[i].first, copyRef.m_AnimCallbackList[i].second));
}

HySprite2d::~HySprite2d(void)
{
}

const HySprite2d &HySprite2d::operator=(const HySprite2d &rhs)
{
	IHyInstance2d::operator=(rhs);

	m_bIsAnimPaused = rhs.m_bIsAnimPaused;
	m_fAnimPlayRate = rhs.m_fAnimPlayRate;
	m_fElapsedFrameTime = rhs.m_fElapsedFrameTime;
	m_uiCurAnimState = rhs.m_uiCurAnimState;
	m_uiCurFrame = rhs.m_uiCurFrame;

	m_AnimCtrlAttribList.clear();
	for(uint32 i = 0; i < static_cast<uint32>(rhs.m_AnimCtrlAttribList.size()); ++i)
		m_AnimCtrlAttribList.push_back(rhs.m_AnimCtrlAttribList[i]);

	m_AnimCallbackList.clear();
	for(uint32 i = 0; i < static_cast<uint32>(rhs.m_AnimCallbackList.size()); ++i)
		m_AnimCallbackList.push_back(std::pair<HySprite2dAnimFinishedCallback, void *>(rhs.m_AnimCallbackList[i].first, rhs.m_AnimCallbackList[i].second));

	return *this;
}

void HySprite2d::AnimCtrl(HyAnimCtrl eAnimCtrl)
{
	AnimCtrl(eAnimCtrl, m_uiCurAnimState);
}

void HySprite2d::AnimCtrl(HyAnimCtrl eAnimCtrl, uint32 uiAnimState)
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

bool HySprite2d::AnimIsReverse()
{
	return AnimIsReverse(m_uiCurAnimState);
}

bool HySprite2d::AnimIsReverse(uint32 uiAnimState)
{
	if(AcquireData() == nullptr) {
		HyLogWarning("HySprite2d::AnimIsReverse invoked on null data");
		return false;
	}

	return (m_AnimCtrlAttribList[m_uiCurAnimState] & ANIMCTRLATTRIB_Reverse) != 0;
}

uint32 HySprite2d::AnimGetNumStates()
{
	if(AcquireData() == nullptr) {
		HyLogWarning("HySprite2d::AnimGetNumStates invoked on null data");
		return 0;
	}
	
	return static_cast<const HySprite2dData *>(UncheckedGetData())->GetNumStates();
}

uint32 HySprite2d::AnimGetState() const
{
	return m_uiCurAnimState;
}

uint32 HySprite2d::AnimGetNumFrames()
{
	if(AcquireData() == nullptr) {
		HyLogWarning("HySprite2d::AnimGetNumFrames invoked on null data");
		return 0;
	}

	return static_cast<const HySprite2dData *>(UncheckedGetData())->GetState(m_uiCurAnimState).m_uiNUMFRAMES;
}

uint32 HySprite2d::AnimGetFrame() const
{
	return m_uiCurFrame;
}

void HySprite2d::AnimSetFrame(uint32 uiFrameIndex)
{
	if(AcquireData() == nullptr || uiFrameIndex >= static_cast<const HySprite2dData *>(UncheckedGetData())->GetState(m_uiCurAnimState).m_uiNUMFRAMES)
	{
		if(UncheckedGetData() == nullptr) {
			HyLogWarning("HySprite2d::AnimSetFrame invoked on null data");
		}
		else {
			HyLogWarning("HySprite2d::AnimSetFrame wants to set frame index of '" << uiFrameIndex << "' when total number of frames is '" << static_cast<const HySprite2dData *>(AcquireData())->GetState(m_uiCurAnimState).m_uiNUMFRAMES << "'");
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

float HySprite2d::AnimGetPlayRate() const
{
	return m_fAnimPlayRate;
}

void HySprite2d::AnimSetPlayRate(float fPlayRate)
{
	if(fPlayRate < 0.0f)
		fPlayRate = 0.0f;

	m_fAnimPlayRate = fPlayRate;
}

void HySprite2d::AnimSetState(uint32 uiStateIndex)
{
	if(AcquireData() == nullptr || uiStateIndex >= static_cast<const HySprite2dData *>(UncheckedGetData())->GetNumStates())
	{
		if(UncheckedGetData() == nullptr) {
			HyLogWarning("HySprite2d::AnimSetState invoked on null data");
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

bool HySprite2d::AnimIsFinished()
{
	if(AcquireData() == nullptr) {
		HyLogWarning("HySprite2d::AnimIsFinished invoked on null data");
		return false;
	}

	return (m_AnimCtrlAttribList[m_uiCurAnimState] & ANIMCTRLATTRIB_Finished) != 0;
}

bool HySprite2d::AnimIsPaused()
{
	return m_bIsAnimPaused;
}

void HySprite2d::AnimSetPause(bool bPause)
{
	if(AcquireData() == nullptr) {
		HyLogWarning("HySprite2d::AnimSetPause invoked on null data");
		return;
	}

	if(m_bIsAnimPaused == bPause)
		return;

	m_bIsAnimPaused = bPause;
	m_fElapsedFrameTime = 0.0f;

	if(m_bIsAnimPaused == false)
		m_AnimCtrlAttribList[m_uiCurAnimState] &= ~ANIMCTRLATTRIB_Finished;
}

float HySprite2d::AnimGetDuration()
{
	if(AcquireData() == nullptr) {
		HyLogWarning("HySprite2d::AnimGetDuration invoked on null data");
		return 0.0f;
	}

	return static_cast<const HySprite2dData *>(UncheckedGetData())->GetState(m_uiCurAnimState).m_fDURATION;
}

void HySprite2d::AnimSetCallback(uint32 uiStateIndex, HySprite2dAnimFinishedCallback callBack /*= HySprite2d::NullAnimCallback*/, void *pParam /*= nullptr*/)
{
	if(AcquireData() == nullptr || uiStateIndex >= static_cast<const HySprite2dData *>(UncheckedGetData())->GetNumStates())
	{
		if(UncheckedGetData() == nullptr) {
			HyLogWarning("HySprite2d::AnimSetCallback invoked on null data");
		}
		else {
			HyLogWarning("HySprite2d::AnimSetCallback wants to set anim callback on index of '" << uiStateIndex << "' when total number of states is '" << static_cast<const HySprite2dData *>(AcquireData())->GetNumStates() << "'");
		}

		return;
	}

	m_AnimCallbackList[uiStateIndex].first = callBack;
	m_AnimCallbackList[uiStateIndex].second = pParam;
}

float HySprite2d::AnimGetCurFrameWidth(bool bIncludeScaling /*= true*/)
{
	if(AcquireData() == nullptr) {
		HyLogWarning("HySprite2d::AnimGetCurFrameWidth invoked on null data");
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

float HySprite2d::AnimGetCurFrameHeight(bool bIncludeScaling /*= true*/)
{
	if(AcquireData() == nullptr) {
		HyLogWarning("HySprite2d::AnimGetCurFrameHeight invoked on null data");
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

float HySprite2d::AnimGetMaxWidth(uint32 uiStateIndex, bool bIncludeScaling /*= true*/)
{
	const HySprite2dData *pData = static_cast<const HySprite2dData *>(AcquireData());
	if(pData == nullptr) {
		HyLogWarning("HySprite2d::AnimGetMaxWidth invoked on null data");
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

float HySprite2d::AnimGetMaxHeight(uint32 uiStateIndex, bool bIncludeScaling /*= true*/)
{
	const HySprite2dData *pData = static_cast<const HySprite2dData *>(AcquireData());
	if(pData == nullptr) {
		HyLogWarning("HySprite2d::AnimGetMaxHeight invoked on null data");
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

void HySprite2d::SetUserOffset(int32 iOffsetX, int32 iOffsetY)
{
	m_vCustomOffset.x = iOffsetX;
	m_vCustomOffset.y = iOffsetY;
}

glm::ivec2 HySprite2d::AnimGetCurFrameOffset()
{
	const HySprite2dFrame &frameRef = static_cast<const HySprite2dData *>(AcquireData())->GetFrame(m_uiCurAnimState, m_uiCurFrame);
	return frameRef.vOFFSET + m_vCustomOffset;
}

/*virtual*/ bool HySprite2d::IsLoadDataValid() /*override*/
{
	const HySprite2dData *pData = static_cast<const HySprite2dData *>(AcquireData());
	return pData && pData->GetNumStates() != 0;
}

/*virtual*/ bool HySprite2d::OnIsValidToRender() /*override*/
{
	return ((m_AnimCtrlAttribList[m_uiCurAnimState] & ANIMCTRLATTRIB_Invalid) == 0);
}

/*virtual*/ void HySprite2d::OnCalcBoundingVolume() /*override*/
{
	glm::vec2 vFrameOffset = static_cast<const HySprite2dData *>(AcquireData())->GetFrame(m_uiCurAnimState, m_uiCurFrame).vOFFSET + m_vCustomOffset;
	vFrameOffset *= scale.Get();

	float fHalfWidth = AnimGetCurFrameWidth(true) * 0.5f;
	float fHalfHeight = AnimGetCurFrameHeight(true) * 0.5f;

	m_LocalBoundingVolume.SetAsBox(fHalfWidth, fHalfHeight, glm::vec2(vFrameOffset.x + fHalfWidth, vFrameOffset.y + fHalfHeight), rot.Get());
}

/*virtual*/ void HySprite2d::OnDataAcquired() /*override*/
{
	const HySprite2dData *pData = static_cast<const HySprite2dData *>(UncheckedGetData());
	uint32 uiNumStates = pData->GetNumStates();

	while(m_AnimCtrlAttribList.size() > uiNumStates)
		m_AnimCtrlAttribList.pop_back();

	while(m_AnimCtrlAttribList.size() < uiNumStates)
		m_AnimCtrlAttribList.push_back(0);

	while(m_AnimCallbackList.size() < uiNumStates)
		m_AnimCallbackList.push_back(std::pair<HySprite2dAnimFinishedCallback, void *>(NullAnimCallback, nullptr));

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

/*virtual*/ void HySprite2d::OnLoadedUpdate() /*override*/
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
			m_AnimCallbackList[m_uiCurAnimState].first(this, m_AnimCallbackList[m_uiCurAnimState].second);
	}

	const HySprite2dFrame &UpdatedFrameRef = static_cast<const HySprite2dData *>(UncheckedGetData())->GetFrame(m_uiCurAnimState, m_uiCurFrame);
	m_hTextureHandle = UpdatedFrameRef.GetGfxApiHandle();
}

/*virtual*/ void HySprite2d::OnWriteVertexData(HyVertexBuffer &vertexBufferRef) /*override*/
{
	const HySprite2dFrame &frameRef = static_cast<const HySprite2dData *>(UncheckedGetData())->GetFrame(m_uiCurAnimState, m_uiCurFrame);

	glm::vec2 vSize(frameRef.rSRC_RECT.Width() * frameRef.pAtlas->GetWidth(), frameRef.rSRC_RECT.Height() * frameRef.pAtlas->GetHeight());
	vertexBufferRef.AppendData2d(&vSize, sizeof(glm::vec2));

	glm::vec2 vOffset(frameRef.vOFFSET.x + m_vCustomOffset.x, frameRef.vOFFSET.y + m_vCustomOffset.y);
	vertexBufferRef.AppendData2d(&vOffset, sizeof(glm::vec2));

	vertexBufferRef.AppendData2d(&CalculateTopTint(), sizeof(glm::vec3));

	float fAlpha = CalculateAlpha();
	vertexBufferRef.AppendData2d(&fAlpha, sizeof(float));

	vertexBufferRef.AppendData2d(&CalculateBotTint(), sizeof(glm::vec3));

	vertexBufferRef.AppendData2d(&fAlpha, sizeof(float));

	glm::vec2 vUV;

	vUV.x = frameRef.rSRC_RECT.right;//1.0f;
	vUV.y = frameRef.rSRC_RECT.top;//1.0f;
	vertexBufferRef.AppendData2d(&vUV, sizeof(glm::vec2));

	vUV.x = frameRef.rSRC_RECT.left;//0.0f;
	vUV.y = frameRef.rSRC_RECT.top;//1.0f;
	vertexBufferRef.AppendData2d(&vUV, sizeof(glm::vec2));

	vUV.x = frameRef.rSRC_RECT.right;//1.0f;
	vUV.y = frameRef.rSRC_RECT.bottom;//0.0f;
	vertexBufferRef.AppendData2d(&vUV, sizeof(glm::vec2));

	vUV.x = frameRef.rSRC_RECT.left;//0.0f;
	vUV.y = frameRef.rSRC_RECT.bottom;//0.0f;
	vertexBufferRef.AppendData2d(&vUV, sizeof(glm::vec2));

	vertexBufferRef.AppendData2d(&GetWorldTransform(), sizeof(glm::mat4));
}

/*static*/ void HySprite2d::NullAnimCallback(HySprite2d *pSelf, void *pParam)
{
}
