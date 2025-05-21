/**************************************************************************
 *	HySpine2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySpine2d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "Diagnostics/Console/IHyConsole.h"
#include "Assets/Nodes/Objects/HySpineData.h"
#include "HyEngine.h"

#ifdef HY_USE_SPINE
spine::SkeletonRenderer	HySpine2d::sm_Renderer;
#endif

HySpine2d::HySpine2d(HyEntity2d *pParent /*= nullptr*/) :
	IHyDrawable2d(HYTYPE_Spine, HyNodePath(), pParent)
#ifdef HY_USE_SPINE
	, m_pSkeleton(nullptr),
	m_pAnimationState(nullptr),
	m_pSkeletonBounds(nullptr),
	m_pRenderCmd(nullptr)
#endif
{
	m_ShaderUniforms.SetNumTexUnits(1);
}

HySpine2d::HySpine2d(const HyNodePath &nodePath, HyEntity2d *pParent /*= nullptr*/) :
	IHyDrawable2d(HYTYPE_Spine, nodePath, pParent)
#ifdef HY_USE_SPINE
	, m_pSkeleton(nullptr),
	m_pAnimationState(nullptr),
	m_pSkeletonBounds(nullptr),
	m_pRenderCmd(nullptr)
#endif
{
	m_ShaderUniforms.SetNumTexUnits(1);
}

HySpine2d::HySpine2d(const char *szPrefix, const char *szName, HyEntity2d *pParent /*= nullptr*/) :
	IHyDrawable2d(HYTYPE_Spine, HyNodePath(szPrefix, szName), pParent)
#ifdef HY_USE_SPINE
	, m_pSkeleton(nullptr),
	m_pAnimationState(nullptr),
	m_pSkeletonBounds(nullptr),
	m_pRenderCmd(nullptr)
#endif
{
	m_ShaderUniforms.SetNumTexUnits(1);
}

HySpine2d::HySpine2d(const HySpine2d &copyRef) :
	IHyDrawable2d(copyRef)
#ifdef HY_USE_SPINE
	, m_pSkeleton(nullptr),
	m_pAnimationState(nullptr),
	m_pSkeletonBounds(nullptr),
	m_pRenderCmd(nullptr)
#endif
{
	m_ShaderUniforms.SetNumTexUnits(1);
}

/*virtual*/ HySpine2d::~HySpine2d(void)
{
#ifdef HY_USE_SPINE
	delete m_pSkeleton;
	delete m_pAnimationState;
	delete m_pSkeletonBounds;
#endif
}

const HySpine2d &HySpine2d::operator=(const HySpine2d &rhs)
{
	IHyDrawable2d::operator=(rhs);
	return *this;
}

/*virtual*/ void HySpine2d::CalcLocalBoundingShape(HyShape2d &shapeOut) /*override*/
{
#ifdef HY_USE_SPINE
	if(m_pSkeleton == nullptr)
	{
		shapeOut.SetAsNothing();
		return;
	}
	
	float fX, fY, fWidth, fHeight;
	spine::Vector<float> vertexList;
	m_pSkeleton->getBounds(fX, fY, fWidth, fHeight, vertexList);
	m_LocalBoundingRect.Set(fX, fY, fWidth, fHeight);
	
	shapeOut.SetAsBox(m_LocalBoundingRect);
#else
	shapeOut.SetAsNothing();
#endif
}

/*virtual*/ float HySpine2d::GetWidth(float fPercent) /*override*/
{
	return m_LocalBoundingRect.GetWidth(fPercent);
}

/*virtual*/ float HySpine2d::GetHeight(float fPercent) /*override*/
{
	return m_LocalBoundingRect.GetHeight(fPercent);
}

/*virtual*/ bool HySpine2d::SetState(uint32 uiStateIndex) /*override*/
{
	if(this->m_uiState == uiStateIndex || IHyLoadable::SetState(uiStateIndex) == false)
		return this->m_uiState == uiStateIndex; // Return true if the state is already set, otherwise return false because IHyLoadable::SetState() failed

#ifdef HY_USE_SPINE
	// Setup Animations
	if(m_pAnimationState)
	{
		const HySpineData *pData = static_cast<const HySpineData *>(this->AcquireData());
		if(m_pAnimationState && pData)
		{
			
			pData->GetSkeletonData()->getAnimations();

			m_pAnimationState->addAnimation(0, pData->GetSkeletonData()->getAnimations()[uiStateIndex], true, 0.0f);

			


		}
	}

	// Setup Skins
#endif

	return true;
}

/*virtual*/ bool HySpine2d::IsLoadDataValid() /*override*/
{
	const HySpineData *pData = static_cast<const HySpineData *>(this->AcquireData());
	return pData != nullptr;
}

#ifdef HY_USE_SPINE
spine::Animation *HySpine2d::GetAnim(int iIndex)
{
	const HySpineData *pData = static_cast<const HySpineData *>(this->AcquireData());
	if(pData == nullptr)
		return nullptr;

	spine::Vector<spine::Animation *> &animListRef = pData->GetSkeletonData()->getAnimations();
	if(iIndex >= animListRef.size() || iIndex < 0)
		return nullptr;

	return animListRef[iIndex];
}

spine::Animation *HySpine2d::GetAnim(std::string sAnimName)
{
	const HySpineData *pData = static_cast<const HySpineData *>(this->AcquireData());
	if(pData == nullptr)
		return nullptr;

	return pData->GetSkeletonData()->findAnimation(sAnimName.c_str());
}

void HySpine2d::ClearTracks()
{
	if(m_pAnimationState == nullptr)
	{
		HyLogWarning("HySpine2d::ClearTracks() - Animation state is null");
		return;
	}
	m_pAnimationState->clearTracks();
}

void HySpine2d::ClearTrack(size_t uiTrackIndex)
{
	if(m_pAnimationState == nullptr)
	{
		HyLogWarning("HySpine2d::ClearTrack() - Animation state is null");
		return;
	}
	m_pAnimationState->clearTrack(uiTrackIndex);
}

spine::TrackEntry *HySpine2d::SetAnimation(size_t uiTrackIndex, spine::Animation *pAnimation, bool bLoop)
{
	if(m_pAnimationState == nullptr)
	{
		HyLogWarning("HySpine2d::SetAnimation() - Animation state is null");
		return nullptr;
	}
	return m_pAnimationState->setAnimation(uiTrackIndex, pAnimation, bLoop);
}

spine::TrackEntry *HySpine2d::AddAnimation(size_t uiTrackIndex, spine::Animation *pAnimation, bool bLoop, float fDelay)
{
	if(m_pAnimationState == nullptr)
	{
		HyLogWarning("HySpine2d::AddAnimation() - Animation state is null");
		return nullptr;
	}
	return m_pAnimationState->addAnimation(uiTrackIndex, pAnimation, bLoop, fDelay);
}

spine::TrackEntry *HySpine2d::SetEmptyAnimation(size_t uiTrackIndex, float fMixDuration)
{
	if(m_pAnimationState == nullptr)
	{
		HyLogWarning("HySpine2d::SetEmptyAnimation() - Animation state is null");
		return nullptr;
	}
	return m_pAnimationState->setEmptyAnimation(uiTrackIndex, fMixDuration);
}

spine::TrackEntry *HySpine2d::AddEmptyAnimation(size_t uiTrackIndex, float fMixDuration, float fDelay)
{
	if(m_pAnimationState == nullptr)
	{
		HyLogWarning("HySpine2d::AddEmptyAnimation() - Animation state is null");
		return nullptr;
	}
	return m_pAnimationState->addEmptyAnimation(uiTrackIndex, fMixDuration, fDelay);
}

void HySpine2d::SetEmptyAnimations(float fMixDuration)
{
	if(m_pAnimationState == nullptr)
	{
		HyLogWarning("HySpine2d::SetEmptyAnimations() - Animation state is null");
		return;
	}
	m_pAnimationState->setEmptyAnimations(fMixDuration);
}

spine::TrackEntry *HySpine2d::GetCurrentTrack(size_t uiTrackIndex)
{
	if(m_pAnimationState == nullptr)
	{
		HyLogWarning("HySpine2d::GetCurrentTrack() - Animation state is null");
		return nullptr;
	}
	return m_pAnimationState->getCurrent(uiTrackIndex);
}
#endif // #ifdef HY_USE_SPINE


/*virtual*/ void HySpine2d::SetDirty(uint32 uiDirtyFlags) /*override*/
{
#ifdef HY_USE_SPINE
	if(m_pSkeleton)
	{
		if(uiDirtyFlags & DIRTY_Transform)
		{
			m_pSkeleton->setPosition(pos.GetX(), pos.GetY());
			m_pSkeleton->setScaleX(scale.GetX());
			m_pSkeleton->setScaleY(scale.GetY());
			m_pSkeleton->getRootBone()->setRotation(rot.Get());
		}
	}
#endif

	IHyDrawable2d::SetDirty(uiDirtyFlags);
}

/*virtual*/ void HySpine2d::OnUpdateUniforms(float fExtrapolatePercent) /*override*/
{
	glm::mat4 mtx = GetSceneTransform(fExtrapolatePercent);
	m_ShaderUniforms.Set("u_mtxTransform", mtx);
}

/*virtual*/ bool HySpine2d::OnIsValidToRender() /*override*/
{
	return true;
}

/*virtual*/ void HySpine2d::OnDataAcquired() /*override*/
{
	const HySpineData *pData = static_cast<const HySpineData *>(this->UncheckedGetData());

#ifdef HY_USE_SPINE
	m_pSkeleton = HY_NEW spine::Skeleton(pData->GetSkeletonData());
	m_pAnimationState = HY_NEW spine::AnimationState(pData->GetAnimationStateData());
	//m_pAnimationState->setListener(
	//	[](spine::AnimationState *pState, spine::EventType eType, spine::TrackEntry *pEntry, spine::Event *pEvent)
	//	{

	//	});
	m_pSkeletonBounds = HY_NEW spine::SkeletonBounds();
#endif
	m_ShaderUniforms.SetNumTexUnits(1);
}

/*virtual*/ void HySpine2d::OnLoadedUpdate() /*override*/
{
#ifdef HY_USE_SPINE
	m_pAnimationState->update(HyEngine::DeltaTime());
	m_pAnimationState->apply(*m_pSkeleton);						// Apply the state to the skeleton

	m_pSkeleton->updateWorldTransform(spine::Physics_Update);	// Calculate world transforms for rendering

	if(m_pSkeletonBounds)
		m_pSkeletonBounds->update(*m_pSkeleton, true);
#endif
}

/*virtual*/ void HySpine2d::PrepRenderStage(uint32 uiStageIndex, HyRenderMode &eRenderModeOut, HyBlendMode &eBlendModeOut, uint32 &uiNumInstancesOut, uint32 &uiNumVerticesPerInstOut, bool &bIsBatchable) /*override*/
{
	eRenderModeOut = HYRENDERMODE_Triangles;

#ifdef HY_USE_SPINE
	if(m_pRenderCmd == nullptr)
	{
		HyAssert(uiStageIndex == 0, "m_pRenderCmd shouldn't be null if uiStageIndex != 0");
		HyAssert(m_pSkeleton != nullptr, "HySpine2d m_pSkeleton shouldn't be null if invoking PrepRenderStage()");
		m_pRenderCmd = sm_Renderer.render(*m_pSkeleton); // This 'm_pRenderCmd/sm_Renderer' will be in scope for the whole HyRenderBuffer::AppendRenderState do/while loop
	}

	if(m_pRenderCmd == nullptr)
		return;

	switch(m_pRenderCmd->blendMode)
	{
	case spine::BlendMode_Normal:	eBlendModeOut = HYBLENDMODE_Normal; break;
	case spine::BlendMode_Additive:	eBlendModeOut = HYBLENDMODE_Additive; break;
	case spine::BlendMode_Multiply:	eBlendModeOut = HYBLENDMODE_Multiply; break;
	case spine::BlendMode_Screen:	eBlendModeOut = HYBLENDMODE_Screen; break;
	default:
		HyLogError("HySpine2d::PrepRenderStage() - Unknown blend mode:" << m_pRenderCmd->blendMode);
		break;
	}
	
	m_ShaderUniforms.SetTexHandle(0, static_cast<HySpineAtlas *>(m_pRenderCmd->texture)->GetTexHandle());
	
	uiNumVerticesPerInstOut = m_pRenderCmd->numIndices;
#endif
	uiNumInstancesOut = 0;
	bIsBatchable = false; // TODO: Maybe it is batchable?
}

/*virtual*/ bool HySpine2d::WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef, float fExtrapolatePercent) /*override*/
{
#ifdef HY_USE_SPINE
	float *positions = m_pRenderCmd->positions;
	float *uvs = m_pRenderCmd->uvs;
	uint32_t *colors = m_pRenderCmd->colors;
	uint32_t *darkColors = m_pRenderCmd->darkColors;
	for(int i = 0, j = 0; i < m_pRenderCmd->numVertices; i++, j += 2)
	{
		//vertex_t *vertex = &renderer->vertex_buffer[i];
		glm::vec2 ptPos(positions[j], positions[j + 1]);
		vertexBufferRef.AppendVertexData(&ptPos, sizeof(glm::vec2));

		glm::vec2 vUv(uvs[j], uvs[j + 1]);
		vUv = static_cast<HySpineAtlas *>(m_pRenderCmd->texture)->ConvertUVs(vUv);
		vertexBufferRef.AppendVertexData(&vUv, sizeof(glm::vec2));

		uint32_t uiColor = colors[i]; // 0xAARRGGBB is how spine render command stores colors
		// Swap the color components R and B so the byte order is RGBA
		uiColor = (uiColor & 0xFF00FF00) | ((uiColor & 0x00FF0000) >> 16) | ((uiColor & 0x000000FF) << 16);
		vertexBufferRef.AppendVertexData(&uiColor, sizeof(uint32_t));

		//uint32_t uiDarkColor = darkColors[i]; // 0xAARRGGBB is how spine render command stores colors
		//// Swap the color components R and B so the byte order is RGBA
		//uiDarkColor = (uiDarkColor & 0xFF00FF00) | ((uiDarkColor & 0x00FF0000) >> 16) | ((uiDarkColor & 0x000000FF) << 16);
		//vertexBufferRef.AppendVertexData(&uiDarkColor, sizeof(uint32_t));
	}

	vertexBufferRef.AppendIndicesData(m_pRenderCmd->indices, m_pRenderCmd->numIndices);

	m_pRenderCmd = m_pRenderCmd->next;
	return m_pRenderCmd == nullptr; // return true if we are done
#else
	return true;
#endif
}
