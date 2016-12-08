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

HySprite2d::HySprite2d(const char *szPrefix, const char *szName) :	IHyInst2d(HYTYPE_Sprite2d, szPrefix, szName),
																	m_pAnimCtrlAttribs(NULL),
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
	delete[] m_pAnimCtrlAttribs;
}

void HySprite2d::AnimCtrl(HyAnimCtrl eAnimCtrl)
{
	AnimCtrl(eAnimCtrl, m_uiCurAnimState);
}

void HySprite2d::AnimCtrl(HyAnimCtrl eAnimCtrl, uint32 uiAnimState)
{
	switch(eAnimCtrl)
	{
	case HYANIMCTRL_Play:
		m_pAnimCtrlAttribs[uiAnimState] &= ~(ANIMCTRLATTRIB_Paused | ANIMCTRLATTRIB_Reverse);
		break;
	case HYANIMCTRL_Pause:
		m_pAnimCtrlAttribs[uiAnimState] |= ANIMCTRLATTRIB_Paused;
		break;
	case HYANIMCTRL_ReversePlay:
		m_pAnimCtrlAttribs[uiAnimState] &= ~ANIMCTRLATTRIB_Paused;
		m_pAnimCtrlAttribs[uiAnimState] |= ANIMCTRLATTRIB_Reverse;
		break;
	case HYANIMCTRL_Reset:
		m_pAnimCtrlAttribs[uiAnimState] &= ~ANIMCTRLATTRIB_IsBouncing;
		if(m_pAnimCtrlAttribs[uiAnimState] & ANIMCTRLATTRIB_Reverse)
			m_uiCurFrame = static_cast<HySprite2dData *>(m_pData)->GetState(uiAnimState).m_uiNUMFRAMES - 1;
		else
			m_uiCurFrame = 0;
		break;
	case HYANIMCTRL_Loop:
		m_pAnimCtrlAttribs[uiAnimState] |= ANIMCTRLATTRIB_Loop;
		break;
	case HYANIMCTRL_DontLoop:
		m_pAnimCtrlAttribs[uiAnimState] &= ~ANIMCTRLATTRIB_Loop;
		break;
	case HYANIMCTRL_Bounce:
		m_pAnimCtrlAttribs[uiAnimState] |= ANIMCTRLATTRIB_Bounce;
		break;
	case HYANIMCTRL_DontBounce:
		m_pAnimCtrlAttribs[uiAnimState] &= ~ANIMCTRLATTRIB_Bounce;
		break;
	}
}

uint32 HySprite2d::AnimGetNumStates()
{
	return static_cast<HySprite2dData *>(m_pData)->GetNumStates();
}

uint32 HySprite2d::AnimGetCurState()
{
	return m_uiCurAnimState;
}

uint32 HySprite2d::AnimGetNumFrames()
{
	return static_cast<int32>(static_cast<HySprite2dData *>(m_pData)->GetState(m_uiCurAnimState).m_uiNUMFRAMES);
}

uint32 HySprite2d::AnimGetFrame()
{
	return m_uiCurFrame;
}

void HySprite2d::AnimSetFrame(uint32 uiFrameIndex)
{
	HyAssert(uiFrameIndex >= 0 && uiFrameIndex < AnimGetNumFrames(), "HySprite2d::AnimSetFrame - Invalid frame index specified (" << uiFrameIndex << ")");
	m_uiCurFrame = uiFrameIndex;
}

float HySprite2d::AnimGetPlayRate()
{
	return m_fAnimPlayRate;
}

void HySprite2d::AnimSetPlayRate(float fPlayRate)
{
	HyAssert(fPlayRate >= 0.0f, "HySprite2d::AnimSetPlayRate passed a value that was below zero");

	m_fAnimPlayRate = fPlayRate;
}

void HySprite2d::AnimSetState(uint32 uiStateIndex)
{
	HyAssert(uiStateIndex < static_cast<HySprite2dData *>(m_pData)->GetNumStates(), "HySprite2d::AnimSetState was passed an invalid state index (" << uiStateIndex << ")");
	
	m_uiCurAnimState = uiStateIndex;
}

//void HySprite2d::AnimSetState(std::string sStateName)
//{
//	uint32 uiNumStates = static_cast<HySprite2dData *>(m_pData)->GetNumStates();
//	std::transform(sStateName.begin(), sStateName.end() ::tolower());
//
//	for(int i = 0; i < uiNumStates; ++i)
//	{
//		if( == std::transform(static_cast<HySprite2dData *>(m_pData)->GetState(i)->m_sNAME.begin(), static_cast<HySprite2dData *>(m_pData)->GetState(i)->m_sNAME.end(), ::tolower())
//		{
//			AnimSetState(i);
//			break;
//		}
//	}
//}

bool HySprite2d::AnimIsFinished()
{
	uint8 uiCtrlAttribs = m_pAnimCtrlAttribs[m_uiCurAnimState];
	if(uiCtrlAttribs & ANIMCTRLATTRIB_Loop)
		return false;

	if((uiCtrlAttribs & ANIMCTRLATTRIB_Reverse) == 0)
	{
		if(uiCtrlAttribs & ANIMCTRLATTRIB_IsBouncing)
			return m_uiCurFrame == 0;
		else
			return m_uiCurFrame == (AnimGetNumFrames() - 1);
	}
	else
	{
		if(uiCtrlAttribs & ANIMCTRLATTRIB_IsBouncing)
			return m_uiCurFrame == (AnimGetNumFrames() - 1);
		else
			return m_uiCurFrame == 0;
	}
}

bool HySprite2d::AnimIsPaused()
{
	return (m_pAnimCtrlAttribs[m_uiCurAnimState] & ANIMCTRLATTRIB_Paused) != 0;
}

void HySprite2d::AnimSetCallback(int iStateID, void(*fpCallback)(HySprite2d *, void *), void *pParam /*= NULL*/)
{

}

void HySprite2d::AnimSetFrame(int iFrameIndex)
{
}

float HySprite2d::GetCurFrameWidth(bool bIncludeScaling /*= true*/)
{
	HySprite2dData *pData = static_cast<HySprite2dData *>(m_pData);
	const HySprite2dFrame &frameRef = pData->GetFrame(m_uiCurAnimState, m_uiCurFrame);

	return frameRef.rSRC_RECT.Width() * frameRef.pAtlasGroup->GetWidth() * (bIncludeScaling ? scale.X() : 1.0f);
}

float HySprite2d::GetCurFrameHeight(bool bIncludeScaling /*= true*/)
{
	HySprite2dData *pData = static_cast<HySprite2dData *>(m_pData);
	const HySprite2dFrame &frameRef = pData->GetFrame(m_uiCurAnimState, m_uiCurFrame);

	return frameRef.rSRC_RECT.Height() * frameRef.pAtlasGroup->GetHeight() * (bIncludeScaling ? scale.Y() : 1.0f);
}

/*virtual*/ void HySprite2d::OnDataLoaded()
{
	HySprite2dData *pData = static_cast<HySprite2dData *>(m_pData);

	uint32 uiNumStates = pData->GetNumStates();

	delete[] m_pAnimCtrlAttribs;
	m_pAnimCtrlAttribs = new uint8[uiNumStates];
	memset(m_pAnimCtrlAttribs, 0, sizeof(uint8) * uiNumStates);
	for(uint32 i = 0; i < uiNumStates; ++i)
	{
		const HySprite2dData::AnimState &stateRef = pData->GetState(i);

		if(stateRef.m_bLOOP)
			m_pAnimCtrlAttribs[i] |= ANIMCTRLATTRIB_Loop;
		if(stateRef.m_bBOUNCE)
			m_pAnimCtrlAttribs[i] |= ANIMCTRLATTRIB_Bounce;
		if(stateRef.m_bREVERSE)
			m_pAnimCtrlAttribs[i] |= ANIMCTRLATTRIB_Reverse;
	}

}

/*virtual*/ void HySprite2d::OnInstUpdate()
{
	const HySprite2dFrame &frameRef = static_cast<HySprite2dData *>(m_pData)->GetFrame(m_uiCurAnimState, m_uiCurFrame);

	m_RenderState.SetTextureHandle(frameRef.pAtlasGroup->GetGfxApiHandle());

	uint8 &uiAnimCtrlRef = m_pAnimCtrlAttribs[m_uiCurAnimState];

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

/*virtual*/ void HySprite2d::OnUpdateUniforms(HyShaderUniforms *pShaderUniformsRef)
{
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

	*reinterpret_cast<glm::vec4 *>(pRefDataWritePos) = topColor.Get();
	pRefDataWritePos += sizeof(glm::vec4);

	*reinterpret_cast<glm::vec4 *>(pRefDataWritePos) = botColor.Get();
	pRefDataWritePos += sizeof(glm::vec4);

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
