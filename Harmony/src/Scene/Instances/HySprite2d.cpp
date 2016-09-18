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

HySprite2d::HySprite2d(const char *szPrefix, const char *szName) :	IHyInst2d(HYINST_Sprite2d, szPrefix, szName),
																	m_pAnimStateAttribs(NULL),
																	m_bIsBounced(false),
																	m_fElapsedFrameTime(0.0f),
																	m_uiCurAnimState(0),
																	m_uiCurFrame(0)
{
	m_RenderState.Enable(HyRenderState::DRAWMODE_TRIANGLESTRIP | HyRenderState::DRAWINSTANCED);
	m_RenderState.SetShaderId(HYSHADERPROG_QuadBatch);
	m_RenderState.SetNumInstances(1);
	m_RenderState.SetNumVertices(4);
	
	m_PlayRate.Set(1.0f);
}


HySprite2d::~HySprite2d(void)
{
}

/*virtual*/ void HySprite2d::OnDataLoaded()
{
	HySprite2dData *pData = static_cast<HySprite2dData *>(m_pData);
	//m_RenderState.SetTextureHandle(pData->GetAtlasGroup()->GetGfxApiHandle());

}

/*virtual*/ void HySprite2d::OnUpdate()
{
	HySprite2dData *pData = static_cast<HySprite2dData *>(m_pData);
	const HySprite2dFrame &frameRef = pData->GetFrame(m_uiCurAnimState, m_uiCurFrame);

	m_fElapsedFrameTime += IHyTime::GetUpdateStepSeconds();
	while(m_fElapsedFrameTime >= frameRef.fDURATION)
	{
		const HySprite2dData::AnimState &stateRef = pData->GetState(m_uiCurAnimState);

		uint32 uiNextFrameIndex = m_uiCurFrame;

		if(stateRef.m_bREVERSE == false)
		{
			m_bIsBounced ? uiNextFrameIndex-- : uiNextFrameIndex++;

			if(uiNextFrameIndex < 0)
			{
				m_bIsBounced = false;

				if(stateRef.m_bLOOP)
					uiNextFrameIndex = 1;
				else
					uiNextFrameIndex = m_uiCurFrame;
			}
			else if(uiNextFrameIndex >= stateRef.m_uiNUMFRAMES)
			{
				if(stateRef.m_bBOUNCE)
				{
					uiNextFrameIndex = stateRef.m_uiNUMFRAMES - 2;
					m_bIsBounced = true;
				}
				else if(stateRef.m_bLOOP)
					uiNextFrameIndex = 0;
				else
					uiNextFrameIndex = m_uiCurFrame;
			}
		}
		else
		{
			m_bIsBounced ? uiNextFrameIndex++ : uiNextFrameIndex--;

			if(uiNextFrameIndex < 0)
			{
				if(stateRef.m_bBOUNCE)
				{
					uiNextFrameIndex = 1;
					m_bIsBounced = true;
				}
				else if(stateRef.m_bLOOP)
					uiNextFrameIndex = stateRef.m_uiNUMFRAMES - 1;
				else
					uiNextFrameIndex = m_uiCurFrame;
			}
			else if(uiNextFrameIndex >= stateRef.m_uiNUMFRAMES)
			{
				m_bIsBounced = false;

				if(stateRef.m_bLOOP)
					uiNextFrameIndex = stateRef.m_uiNUMFRAMES - 2;
				else
					uiNextFrameIndex = m_uiCurFrame;
			}
		}

		m_uiCurFrame = uiNextFrameIndex;
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

	*reinterpret_cast<glm::vec4 *>(pRefDataWritePos) = color.Get();
	pRefDataWritePos += sizeof(glm::vec4);

	*reinterpret_cast<float *>(pRefDataWritePos) = static_cast<float>(frameRef.uiTEXTUREINDEX);
	pRefDataWritePos += sizeof(float);

	glm::vec2 vUV;

	vUV.x = frameRef.rSRC_RECT.right;//1.0f;
	vUV.y = frameRef.rSRC_RECT.top;//0.0f;
	*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
	pRefDataWritePos += sizeof(glm::vec2);

	vUV.x = frameRef.rSRC_RECT.left;//0.0f;
	vUV.y = frameRef.rSRC_RECT.top;//0.0f;
	*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
	pRefDataWritePos += sizeof(glm::vec2);

	vUV.x = frameRef.rSRC_RECT.right;//1.0f;
	vUV.y = frameRef.rSRC_RECT.bottom;//1.0f;
	*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
	pRefDataWritePos += sizeof(glm::vec2);

	vUV.x = frameRef.rSRC_RECT.left;//0.0f;
	vUV.y = frameRef.rSRC_RECT.bottom;//1.0f;
	*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
	pRefDataWritePos += sizeof(glm::vec2);

	GetWorldTransform(*reinterpret_cast<glm::mat4 *>(pRefDataWritePos));
	pRefDataWritePos += sizeof(glm::mat4);
}
