/**************************************************************************
 *	HySprite2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Scene/Nodes/Leafs/Draws/HySprite2d.h"
#include "HyEngine.h"
#include "Scene/Nodes/Entities/HyEntity2d.h"
#include "Diagnostics/Console/HyConsole.h"

HySprite2d::HySprite2d(const char *szPrefix, const char *szName, HyEntity2d *pParent /*= nullptr*/) :	IHyLeafDraw2d(HYTYPE_Sprite2d, szPrefix, szName, pParent),
																										m_bIsAnimPaused(false),
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
		m_AnimCtrlAttribList[uiAnimState] &= ~ANIMCTRLATTRIB_Reverse;
		break;
	case HYANIMCTRL_ReversePlay:
		m_bIsAnimPaused = false;
		m_AnimCtrlAttribList[uiAnimState] |= ANIMCTRLATTRIB_Reverse;
		break;
	case HYANIMCTRL_Reset:
		m_AnimCtrlAttribList[uiAnimState] &= ~ANIMCTRLATTRIB_IsBouncing;
		m_AnimCtrlAttribList[uiAnimState] &= ~ANIMCTRLATTRIB_Finished;
		if(m_AnimCtrlAttribList[uiAnimState] & ANIMCTRLATTRIB_Reverse && static_cast<HySprite2dData *>(AcquireData())->GetState(uiAnimState).m_uiNUMFRAMES > 0)
			m_uiCurFrame = static_cast<HySprite2dData *>(AcquireData())->GetState(uiAnimState).m_uiNUMFRAMES - 1;
		else
			m_uiCurFrame = 0;
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

void HySprite2d::AnimSetPause(bool bPause)
{
	m_bIsAnimPaused = bPause;
	m_fElapsedFrameTime = 0.0f;
}

uint32 HySprite2d::AnimGetNumStates()
{
	return static_cast<HySprite2dData *>(AcquireData())->GetNumStates();
}

uint32 HySprite2d::AnimGetState() const
{
	return m_uiCurAnimState;
}

uint32 HySprite2d::AnimGetNumFrames()
{
	return static_cast<HySprite2dData *>(AcquireData())->GetState(m_uiCurAnimState).m_uiNUMFRAMES;
}

uint32 HySprite2d::AnimGetFrame() const
{
	return m_uiCurFrame;
}

void HySprite2d::AnimSetFrame(uint32 uiFrameIndex)
{
	if(uiFrameIndex >= static_cast<HySprite2dData *>(AcquireData())->GetState(m_uiCurAnimState).m_uiNUMFRAMES)
	{
		HyLogWarning("HySprite2d::AnimSetFrame wants to set frame index of '" << uiFrameIndex << "' when total number of frames is '" << static_cast<HySprite2dData *>(AcquireData())->GetState(m_uiCurAnimState).m_uiNUMFRAMES << "'");
		return;
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
	if(uiStateIndex >= static_cast<HySprite2dData *>(AcquireData())->GetNumStates())
	{
		HyLogWarning("HySprite2d::AnimSetState wants to set state index of '" << uiStateIndex << "' when total number of states is '" << static_cast<HySprite2dData *>(AcquireData())->GetNumStates() << "'");
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
}

bool HySprite2d::AnimIsFinished()
{
	AcquireData();
	return (m_AnimCtrlAttribList[m_uiCurAnimState] & ANIMCTRLATTRIB_Finished) != 0;
}

bool HySprite2d::AnimIsPaused()
{
	return m_bIsAnimPaused;
}

float HySprite2d::AnimGetDuration()
{
	return static_cast<HySprite2dData *>(AcquireData())->GetState(m_uiCurAnimState).m_fDURATION;
}

void HySprite2d::AnimSetCallback(uint32 uiStateIndex, HySprite2dAnimFinishedCallback callBack /*= HySprite2d::NullAnimCallback*/, void *pParam /*= nullptr*/)
{
	if(uiStateIndex >= static_cast<HySprite2dData *>(AcquireData())->GetNumStates())
	{
		HyLogWarning("HySprite2d::AnimSetCallback wants to set anim callback on index of '" << uiStateIndex << "' when total number of states is '" << static_cast<HySprite2dData *>(AcquireData())->GetNumStates() << "'");
		return;
	}

	m_AnimCallbackList[uiStateIndex].first = callBack;
	m_AnimCallbackList[uiStateIndex].second = pParam;
}

float HySprite2d::AnimGetCurFrameWidth(bool bIncludeScaling /*= true*/)
{
	const HySprite2dFrame &frameRef = static_cast<HySprite2dData *>(AcquireData())->GetFrame(m_uiCurAnimState, m_uiCurFrame);

	return frameRef.rSRC_RECT.Width() * frameRef.pAtlas->GetWidth() * (bIncludeScaling ? scale.X() : 1.0f);
}

float HySprite2d::AnimGetCurFrameHeight(bool bIncludeScaling /*= true*/)
{
	const HySprite2dFrame &frameRef = static_cast<HySprite2dData *>(AcquireData())->GetFrame(m_uiCurAnimState, m_uiCurFrame);

	return frameRef.rSRC_RECT.Height() * frameRef.pAtlas->GetHeight() * (bIncludeScaling ? scale.Y() : 1.0f);
}

const glm::ivec2 &HySprite2d::AnimGetCurFrameOffset()
{
	const HySprite2dFrame &frameRef = static_cast<HySprite2dData *>(AcquireData())->GetFrame(m_uiCurAnimState, m_uiCurFrame);
	return frameRef.vOFFSET;
}

/*virtual*/ void HySprite2d::DrawUpdate()
{
	if(IsLoaded() == false)
		return;

	if(m_bIsAnimPaused == false)
		m_fElapsedFrameTime += HyUpdateDelta() * m_fAnimPlayRate;

	const HySprite2dFrame &frameRef = static_cast<HySprite2dData *>(UncheckedGetData())->GetFrame(m_uiCurAnimState, m_uiCurFrame);
	uint8 &uiAnimCtrlRef = m_AnimCtrlAttribList[m_uiCurAnimState];
	while(m_fElapsedFrameTime >= frameRef.fDURATION && frameRef.fDURATION > 0.0f)
	{
		int32 iNumFrames = AnimGetNumFrames();
		int32 iNextFrameIndex = static_cast<int32>(m_uiCurFrame);

		if((uiAnimCtrlRef & ANIMCTRLATTRIB_Reverse) == 0)
		{
			(uiAnimCtrlRef & ANIMCTRLATTRIB_IsBouncing) ? iNextFrameIndex-- : iNextFrameIndex++;

			if(iNextFrameIndex < 0)
			{
				if((uiAnimCtrlRef & ANIMCTRLATTRIB_Finished) == 0)
					m_AnimCallbackList[m_uiCurAnimState].first(this, m_AnimCallbackList[m_uiCurAnimState].second);

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
						m_AnimCallbackList[m_uiCurAnimState].first(this, m_AnimCallbackList[m_uiCurAnimState].second);

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
						m_AnimCallbackList[m_uiCurAnimState].first(this, m_AnimCallbackList[m_uiCurAnimState].second);

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
					m_AnimCallbackList[m_uiCurAnimState].first(this, m_AnimCallbackList[m_uiCurAnimState].second);

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

		m_uiCurFrame = iNextFrameIndex;
		m_fElapsedFrameTime -= frameRef.fDURATION;
	}

	const HySprite2dFrame &UpdatedFrameRef = static_cast<HySprite2dData *>(UncheckedGetData())->GetFrame(m_uiCurAnimState, m_uiCurFrame);
	m_RenderState.SetTextureHandle(UpdatedFrameRef.GetGfxApiHandle());
}

/*virtual*/ void HySprite2d::OnDataAcquired()
{
	HySprite2dData *pData = static_cast<HySprite2dData *>(UncheckedGetData());
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
	}
}

/*virtual*/ void HySprite2d::OnCalcBoundingVolume()
{
	uint32 uiNumVerts = m_RenderState.GetNumVerticesPerInstance();
	glm::vec2 vLowerBounds(0.0f);
	glm::vec2 vUpperBounds(0.0f);

	const HySprite2dFrame &frameRef = static_cast<HySprite2dData *>(AcquireData())->GetFrame(m_uiCurAnimState, m_uiCurFrame);
	vLowerBounds.x = static_cast<float>(frameRef.vOFFSET.x);
	vLowerBounds.y = static_cast<float>(frameRef.vOFFSET.y);
	vUpperBounds.x = vLowerBounds.x + AnimGetCurFrameWidth(true);
	vUpperBounds.y = vLowerBounds.y + AnimGetCurFrameHeight(true);

	m_BoundingVolume.SetLocalAABB(vLowerBounds, vUpperBounds);
}

/*virtual*/ void HySprite2d::OnUpdateUniforms()
{
	//m_ShaderUniforms.Set(...);
}

/*virtual*/ void HySprite2d::OnWriteDrawBufferData(char *&pRefDataWritePos)
{
	const HySprite2dFrame &frameRef = static_cast<HySprite2dData *>(UncheckedGetData())->GetFrame(m_uiCurAnimState, m_uiCurFrame);

	glm::vec2 vSize(frameRef.rSRC_RECT.Width() * frameRef.pAtlas->GetWidth(), frameRef.rSRC_RECT.Height() * frameRef.pAtlas->GetHeight());
	*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vSize;
	pRefDataWritePos += sizeof(glm::vec2);

	glm::vec2 vOffset(frameRef.vOFFSET.x, frameRef.vOFFSET.y);
	*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vOffset;
	pRefDataWritePos += sizeof(glm::vec2);

	*reinterpret_cast<glm::vec3 *>(pRefDataWritePos) = CalculateTopTint();
	pRefDataWritePos += sizeof(glm::vec3);
	*reinterpret_cast<float *>(pRefDataWritePos) = CalculateAlpha();
	pRefDataWritePos += sizeof(float);

	*reinterpret_cast<glm::vec3 *>(pRefDataWritePos) = CalculateBotTint();
	pRefDataWritePos += sizeof(glm::vec3);
	*reinterpret_cast<float *>(pRefDataWritePos) = CalculateAlpha();
	pRefDataWritePos += sizeof(float);
	
	//*reinterpret_cast<float *>(pRefDataWritePos) = static_cast<float>(frameRef.GetActualTextureIndex());
	//pRefDataWritePos += sizeof(float);

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

/*static*/ void HySprite2d::NullAnimCallback(HySprite2d *pSelf, void *pParam)
{
}

#ifdef HY_PLATFORM_GUI
/*virtual*/ void HySprite2d::GuiOverrideData(jsonxx::Value &dataValueRef) /*override*/
{
	Unload();

	delete m_pData;
	m_pData = HY_NEW HySprite2dData("GUI", dataValueRef, *sm_pHyAssets);
	OnDataAcquired();
	Load();
}
#endif
