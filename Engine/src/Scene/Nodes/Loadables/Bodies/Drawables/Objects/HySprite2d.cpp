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
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySprite2d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "Diagnostics/Console/HyConsole.h"
#include "Assets/Nodes/HySprite2dData.h"

HySprite2d::HySprite2d(std::string sPrefix /*= ""*/, std::string sName /*= ""*/, HyEntity2d *pParent /*= nullptr*/) :
	IHySprite<IHyDrawable2d, HyEntity2d>(sPrefix, sName, pParent)
{
}

HySprite2d::HySprite2d(const HySprite2d &copyRef) :
	IHySprite<IHyDrawable2d, HyEntity2d>(copyRef)
{
	for(uint32 i = 0; i < static_cast<uint32>(copyRef.m_AnimCallbackList.size()); ++i)
		m_AnimCallbackList.push_back(std::pair<HySprite2dAnimFinishedCallback, void *>(copyRef.m_AnimCallbackList[i].first, copyRef.m_AnimCallbackList[i].second));
}

HySprite2d::~HySprite2d(void)
{
}

const HySprite2d &HySprite2d::operator=(const HySprite2d &rhs)
{
	IHySprite<IHyDrawable2d, HyEntity2d>::operator=(rhs);

	m_AnimCallbackList.clear();
	for(uint32 i = 0; i < static_cast<uint32>(rhs.m_AnimCallbackList.size()); ++i)
		m_AnimCallbackList.push_back(std::pair<HySprite2dAnimFinishedCallback, void *>(rhs.m_AnimCallbackList[i].first, rhs.m_AnimCallbackList[i].second));

	return *this;
}

void HySprite2d::SetAnimCallback(uint32 uiStateIndex, HySprite2dAnimFinishedCallback callBack /*= HySprite2d::NullAnimCallback*/, void *pParam /*= nullptr*/)
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

/*virtual*/ void HySprite2d::OnInvokeCallback(uint32 uiStateIndex) /*override*/
{
	m_AnimCallbackList[uiStateIndex].first(this, m_AnimCallbackList[uiStateIndex].second);
}

/*virtual*/ void HySprite2d::OnCalcBoundingVolume() /*override*/
{
	if(AcquireData() == nullptr)
	{
		HyLogWarning("HySprite2d::OnCalcBoundingVolume invoked on null data");
		return;
	}

	glm::vec2 vFrameOffset = static_cast<const HySprite2dData *>(UncheckedGetData())->GetFrame(m_uiState, m_uiCurFrame).vOFFSET + m_vCustomOffset;
	vFrameOffset.x *= std::fabs(scale.Get().x);
	vFrameOffset.y *= std::fabs(scale.Get().y);

	float fHalfWidth = GetCurFrameWidth(true) * 0.5f;
	float fHalfHeight = GetCurFrameHeight(true) * 0.5f;

	m_LocalBoundingVolume.SetAsBox(fHalfWidth, fHalfHeight, glm::vec2(vFrameOffset.x + fHalfWidth, vFrameOffset.y + fHalfHeight), 0.0f);
}

/*virtual*/ void HySprite2d::OnDataAcquired() /*override*/
{
	IHySprite<IHyDrawable2d, HyEntity2d>::OnDataAcquired();

	const HySprite2dData *pData = static_cast<const HySprite2dData *>(UncheckedGetData());
	uint32 uiNumStates = pData->GetNumStates();

	while(m_AnimCallbackList.size() < uiNumStates)
		m_AnimCallbackList.push_back(std::pair<HySprite2dAnimFinishedCallback, void *>(NullAnimCallback, nullptr));
}

/*virtual*/ void HySprite2d::OnWriteVertexData(HyVertexBuffer &vertexBufferRef) /*override*/
{
	const HySprite2dFrame &frameRef = static_cast<const HySprite2dData *>(UncheckedGetData())->GetFrame(m_uiState, m_uiCurFrame);

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
