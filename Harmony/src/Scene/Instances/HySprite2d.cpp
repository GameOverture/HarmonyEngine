/**************************************************************************
 *	HySprite2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Scene/Instances/HySprite2d.h"
#include "Diagnostics/HyGuiComms.h"

HySprite2d::HySprite2d(const char *szPrefix, const char *szName) :	IHyInst2d(HYTYPE_Sprite2d, szPrefix, szName),
																	m_fAnimPlayRate(1.0f),
																	m_fElapsedFrameTime(0.0f),
																	m_uiCurAnimState(0),
																	m_uiCurFrame(0)
{
	m_RenderState.Enable(HyRenderState::DRAWMODE_TRIANGLESTRIP | HyRenderState::DRAWINSTANCED);
	m_RenderState.SetShaderId(HYSHADERPROG_QuadBatch);
	m_RenderState.SetNumInstances(1);
	m_RenderState.SetNumVerticesPerInstance(4);
}


HySprite2d::~HySprite2d(void)
{
}

/*virtual*/ void HySprite2d::Unload()
{
	m_AnimCtrlAttribList.clear();
	m_AnimCallbackList.clear();

	m_fAnimPlayRate = 1.0f;
	m_fElapsedFrameTime = 0.0f;
	m_uiCurAnimState = m_uiCurFrame = 0;

	m_RenderState.SetTextureHandle(0);

	IHyInst2d::Unload();
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
		m_AnimCtrlAttribList[uiAnimState] &= ~(ANIMCTRLATTRIB_Paused | ANIMCTRLATTRIB_Reverse);
		m_AnimCtrlAttribList[uiAnimState] |= ANIMCTRLATTRIB_PRELOADAPPLY_DontReverse;
		break;
	case HYANIMCTRL_Pause:
		m_AnimCtrlAttribList[uiAnimState] |= ANIMCTRLATTRIB_Paused;
		break;
	case HYANIMCTRL_ReversePlay:
		m_AnimCtrlAttribList[uiAnimState] &= ~ANIMCTRLATTRIB_Paused;
		m_AnimCtrlAttribList[uiAnimState] |= ANIMCTRLATTRIB_Reverse;
		break;
	case HYANIMCTRL_Reset:
		m_AnimCtrlAttribList[uiAnimState] &= ~ANIMCTRLATTRIB_IsBouncing;
		if(m_AnimCtrlAttribList[uiAnimState] & ANIMCTRLATTRIB_Reverse && m_pData)
			m_uiCurFrame = static_cast<HySprite2dData *>(m_pData)->GetState(uiAnimState).m_uiNUMFRAMES - 1;
		else
			m_uiCurFrame = 0;
		break;
	case HYANIMCTRL_Loop:
		m_AnimCtrlAttribList[uiAnimState] |= ANIMCTRLATTRIB_Loop;
		break;
	case HYANIMCTRL_DontLoop:
		m_AnimCtrlAttribList[uiAnimState] &= ~ANIMCTRLATTRIB_Loop;
		m_AnimCtrlAttribList[uiAnimState] |= ANIMCTRLATTRIB_PRELOADAPPLY_DontLoop;
		break;
	case HYANIMCTRL_Bounce:
		m_AnimCtrlAttribList[uiAnimState] |= ANIMCTRLATTRIB_Bounce;
		break;
	case HYANIMCTRL_DontBounce:
		m_AnimCtrlAttribList[uiAnimState] &= ~ANIMCTRLATTRIB_Bounce;
		m_AnimCtrlAttribList[uiAnimState] |= ANIMCTRLATTRIB_PRELOADAPPLY_DontBounce;
		break;
	}
}

uint32 HySprite2d::AnimGetNumStates() const
{
	if(IsLoaded() == false)
	{
		HyLogWarning("HySprite2d::AnimGetNumStates invoked before sprite instance has loaded. Returning 0");
		return 0;
	}

	return static_cast<HySprite2dData *>(m_pData)->GetNumStates();
}

uint32 HySprite2d::AnimGetCurState() const
{
	return m_uiCurAnimState;
}

uint32 HySprite2d::AnimGetNumFrames() const
{
	if(IsLoaded() == false)
	{
		HyLogWarning("HySprite2d::AnimGetNumFrames invoked before sprite instance has loaded. Returning 0");
		return 0;
	}

	return static_cast<HySprite2dData *>(m_pData)->GetState(m_uiCurAnimState).m_uiNUMFRAMES;
}

uint32 HySprite2d::AnimGetFrame() const
{
	return m_uiCurFrame;
}

void HySprite2d::AnimSetFrame(uint32 uiFrameIndex)
{
	if(IsLoaded())
	{
		if(uiFrameIndex >= static_cast<HySprite2dData *>(m_pData)->GetState(m_uiCurAnimState).m_uiNUMFRAMES)
		{
			HyLogWarning("HySprite2d::AnimSetFrame wants to set frame index of '" << uiFrameIndex << "' when total number of frames is '" << static_cast<HySprite2dData *>(m_pData)->GetState(m_uiCurAnimState).m_uiNUMFRAMES << "'");
			return;
		}
	}

	m_uiCurFrame = uiFrameIndex;
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
	if(IsLoaded())
	{
		if(uiStateIndex >= static_cast<HySprite2dData *>(m_pData)->GetNumStates())
		{
			HyLogWarning("HySprite2d::AnimSetState wants to set state index of '" << uiStateIndex << "' when total number of states is '" << static_cast<HySprite2dData *>(m_pData)->GetNumStates() << "'");
			return;
		}
	}

	if(m_uiCurAnimState == uiStateIndex)
		return;

	m_uiCurAnimState = uiStateIndex;

	while(m_uiCurAnimState >= m_AnimCtrlAttribList.size())
		m_AnimCtrlAttribList.push_back(0);

	if(0 == (m_AnimCtrlAttribList[m_uiCurAnimState] & ANIMCTRLATTRIB_Reverse))
		m_uiCurFrame = 0;
	else
		m_uiCurFrame = AnimGetNumFrames() - 1;
}

bool HySprite2d::AnimIsFinished() const
{
	if(IsLoaded() == false)
	{
		HyLogWarning("HySprite2d::AnimIsFinished invoked before sprite instance has loaded. Returning false");
		return false;
	}

	uint8 uiCtrlAttribs = m_AnimCtrlAttribList[m_uiCurAnimState];
	if(uiCtrlAttribs & ANIMCTRLATTRIB_Loop)
		return false;

	if((uiCtrlAttribs & ANIMCTRLATTRIB_Reverse) == 0)
	{
		// Playing forward
		if(uiCtrlAttribs & ANIMCTRLATTRIB_IsBouncing)
			return m_uiCurFrame == 0;
		else
			return m_uiCurFrame == (AnimGetNumFrames() - 1);
	}
	else
	{
		// Playing reverse
		if(uiCtrlAttribs & ANIMCTRLATTRIB_IsBouncing)
			return m_uiCurFrame == (AnimGetNumFrames() - 1);
		else
			return m_uiCurFrame == 0;
	}
}

bool HySprite2d::AnimIsPaused()
{
	while(m_uiCurAnimState >= m_AnimCtrlAttribList.size())
		m_AnimCtrlAttribList.push_back(0);

	return (m_AnimCtrlAttribList[m_uiCurAnimState] & ANIMCTRLATTRIB_Paused) != 0;
}

void HySprite2d::AnimSetCallback(uint32 uiStateID, void(*fpCallback)(HySprite2d &, void *), void *pParam /*= NULL*/)
{
	while(uiStateID >= m_AnimCallbackList.size())
		m_AnimCallbackList.push_back(std::pair<fpHySprite2dCallback, void *>(NULL, NULL));

	m_AnimCallbackList[uiStateID].first = fpCallback;
	m_AnimCallbackList[uiStateID].second = pParam;
}

float HySprite2d::AnimGetCurFrameWidth(bool bIncludeScaling /*= true*/)
{
	if(IsLoaded() == false)
	{
		HyLogWarning("HySprite2d::AnimGetCurFrameWidth invoked before sprite instance has loaded. Returning 0.0f");
		return 0.0f;
	}

	const HySprite2dFrame &frameRef = static_cast<HySprite2dData *>(m_pData)->GetFrame(m_uiCurAnimState, m_uiCurFrame);

	return frameRef.rSRC_RECT.Width() * frameRef.pAtlasGroup->GetWidth() * (bIncludeScaling ? scale.X() : 1.0f);
}

float HySprite2d::AnimGetCurFrameHeight(bool bIncludeScaling /*= true*/)
{
	if(IsLoaded() == false)
	{
		HyLogWarning("HySprite2d::AnimGetCurFrameHeight invoked before sprite instance has loaded. Returning 0.0f");
		return 0.0f;
	}

	const HySprite2dFrame &frameRef = static_cast<HySprite2dData *>(m_pData)->GetFrame(m_uiCurAnimState, m_uiCurFrame);

	return frameRef.rSRC_RECT.Height() * frameRef.pAtlasGroup->GetHeight() * (bIncludeScaling ? scale.Y() : 1.0f);
}

/*virtual*/ void HySprite2d::OnDataLoaded()
{
	HySprite2dData *pData = static_cast<HySprite2dData *>(m_pData);
	uint32 uiNumStates = pData->GetNumStates();

	while(m_AnimCtrlAttribList.size() > uiNumStates)
		m_AnimCtrlAttribList.pop_back();

	while(m_AnimCtrlAttribList.size() < uiNumStates)
		m_AnimCtrlAttribList.push_back(0);

	for(uint32 i = 0; i < uiNumStates; ++i)
	{
		const HySprite2dData::AnimState &stateRef = pData->GetState(i);

		if(stateRef.m_bLOOP && (m_AnimCtrlAttribList[i] & ANIMCTRLATTRIB_PRELOADAPPLY_DontLoop) == 0)
			m_AnimCtrlAttribList[i] |= ANIMCTRLATTRIB_Loop;
		if(stateRef.m_bBOUNCE && (m_AnimCtrlAttribList[i] & ANIMCTRLATTRIB_PRELOADAPPLY_DontBounce) == 0)
			m_AnimCtrlAttribList[i] |= ANIMCTRLATTRIB_Bounce;
		if(stateRef.m_bREVERSE && (m_AnimCtrlAttribList[i] & ANIMCTRLATTRIB_PRELOADAPPLY_DontReverse) == 0)
			m_AnimCtrlAttribList[i] |= ANIMCTRLATTRIB_Reverse;
	}

	if(m_uiCurAnimState >= uiNumStates)
		m_uiCurAnimState = uiNumStates - 1;

	if(m_uiCurFrame >= AnimGetNumFrames())
		m_uiCurFrame = AnimGetNumFrames() - 1;
}

/*virtual*/ void HySprite2d::OnInstUpdate()
{
	const HySprite2dFrame &frameRef = static_cast<HySprite2dData *>(m_pData)->GetFrame(m_uiCurAnimState, m_uiCurFrame);

	m_RenderState.SetTextureHandle(frameRef.pAtlasGroup->GetGfxApiHandle());

	uint8 &uiAnimCtrlRef = m_AnimCtrlAttribList[m_uiCurAnimState];

	if((uiAnimCtrlRef & ANIMCTRLATTRIB_Paused) == 0)
		m_fElapsedFrameTime += IHyTime::GetUpdateStepSeconds() * m_fAnimPlayRate;

	while(m_fElapsedFrameTime >= frameRef.fDURATION)
	{
		int32 iNumFrames = AnimGetNumFrames();
		int32 iNextFrameIndex = static_cast<int32>(m_uiCurFrame);

		if((uiAnimCtrlRef & ANIMCTRLATTRIB_Reverse) == 0)
		{
			(uiAnimCtrlRef & ANIMCTRLATTRIB_IsBouncing) ? iNextFrameIndex-- : iNextFrameIndex++;

			if(iNextFrameIndex < 0)
			{
				if(uiAnimCtrlRef & ANIMCTRLATTRIB_Loop)
				{
					uiAnimCtrlRef &= ~ANIMCTRLATTRIB_IsBouncing;
					iNextFrameIndex = 1;
				}
				else
					iNextFrameIndex = 0;
			}
			else if(iNextFrameIndex >= iNumFrames)
			{
				if(uiAnimCtrlRef & ANIMCTRLATTRIB_Bounce)
				{
					uiAnimCtrlRef |= ANIMCTRLATTRIB_IsBouncing;
					iNextFrameIndex = iNumFrames - 2;
				}
				else if(uiAnimCtrlRef & ANIMCTRLATTRIB_Loop)
					iNextFrameIndex = 0;
				else
					iNextFrameIndex = iNumFrames - 1;
			}
		}
		else
		{
			(uiAnimCtrlRef & ANIMCTRLATTRIB_IsBouncing) ? iNextFrameIndex++ : iNextFrameIndex--;

			if(iNextFrameIndex < 0)
			{
				if(uiAnimCtrlRef & ANIMCTRLATTRIB_Bounce)
				{
					uiAnimCtrlRef |= ANIMCTRLATTRIB_IsBouncing;
					iNextFrameIndex = 1;
				}
				else if(uiAnimCtrlRef & ANIMCTRLATTRIB_Loop)
					iNextFrameIndex = iNumFrames - 1;
				else
					iNextFrameIndex = 0;
			}
			else if(iNextFrameIndex >= iNumFrames)
			{
				if(uiAnimCtrlRef & ANIMCTRLATTRIB_Loop)
				{
					uiAnimCtrlRef &= ~ANIMCTRLATTRIB_IsBouncing;
					iNextFrameIndex = iNumFrames - 2;
				}
				else
					iNextFrameIndex = iNumFrames - 1;
			}
		}

		m_uiCurFrame = iNextFrameIndex;
		m_fElapsedFrameTime -= frameRef.fDURATION;
	}
}

/*virtual*/ void HySprite2d::OnUpdateUniforms()
{
	//m_ShaderUniforms.Set(...);
}

/*virtual*/ void HySprite2d::OnWriteDrawBufferData(char *&pRefDataWritePos)
{
	HySprite2dData *pData = static_cast<HySprite2dData *>(m_pData);

	const HySprite2dFrame &frameRef = pData->GetFrame(m_uiCurAnimState, m_uiCurFrame);

	glm::vec2 vSize(frameRef.rSRC_RECT.Width() * frameRef.pAtlasGroup->GetWidth(), frameRef.rSRC_RECT.Height() * frameRef.pAtlasGroup->GetHeight());
	*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vSize;
	pRefDataWritePos += sizeof(glm::vec2);

	glm::vec2 vOffset(frameRef.vOFFSET.x, frameRef.vOFFSET.y);
	*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vOffset;
	pRefDataWritePos += sizeof(glm::vec2);

	*reinterpret_cast<glm::vec3 *>(pRefDataWritePos) = topColor.Get();
	pRefDataWritePos += sizeof(glm::vec3);
	*reinterpret_cast<float *>(pRefDataWritePos) = alpha.Get();
	pRefDataWritePos += sizeof(float);

	*reinterpret_cast<glm::vec3 *>(pRefDataWritePos) = botColor.Get();
	pRefDataWritePos += sizeof(glm::vec3);
	*reinterpret_cast<float *>(pRefDataWritePos) = alpha.Get();
	pRefDataWritePos += sizeof(float);

	*reinterpret_cast<float *>(pRefDataWritePos) = static_cast<float>(frameRef.uiTEXTUREINDEX);
	pRefDataWritePos += sizeof(float);

	glm::vec2 vUV;

	vUV.x = frameRef.rSRC_RECT.right;//1.0f;
	vUV.y = frameRef.rSRC_RECT.top;//1.0f;
	*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
	pRefDataWritePos += sizeof(glm::vec2);

	vUV.x = frameRef.rSRC_RECT.left;//0.0f;
	vUV.y = frameRef.rSRC_RECT.top;//1.0f;
	*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
	pRefDataWritePos += sizeof(glm::vec2);

	vUV.x = frameRef.rSRC_RECT.right;//1.0f;
	vUV.y = frameRef.rSRC_RECT.bottom;//0.0f;
	*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
	pRefDataWritePos += sizeof(glm::vec2);

	vUV.x = frameRef.rSRC_RECT.left;//0.0f;
	vUV.y = frameRef.rSRC_RECT.bottom;//0.0f;
	*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
	pRefDataWritePos += sizeof(glm::vec2);

	GetWorldTransform(*reinterpret_cast<glm::mat4 *>(pRefDataWritePos));

	pRefDataWritePos += sizeof(glm::mat4);
}
