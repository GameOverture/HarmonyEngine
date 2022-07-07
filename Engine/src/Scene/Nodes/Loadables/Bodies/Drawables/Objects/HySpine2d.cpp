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
#include "Assets/Nodes/HySpineData.h"
#include "HyEngine.h"

HySpine2d::HySpine2d(std::string sPrefix /*= ""*/, std::string sName /*= ""*/, HyEntity2d *pParent /*= nullptr*/) :
	IHyDrawable2d(HYTYPE_Spine, sPrefix, sName, pParent),
#ifdef HY_USE_SPINE
	m_pSkeleton(nullptr),
	m_pAnimationState(nullptr),
	m_pSkeletonBounds(nullptr),
#endif
	m_uiStartingSlotIndex(0)
{
	m_ShaderUniforms.SetNumTexUnits(1);
}

HySpine2d::HySpine2d(const HySpine2d &copyRef) :
	IHyDrawable2d(copyRef)
{
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

/*virtual*/ bool HySpine2d::SetState(uint32 uiStateIndex) /*override*/
{
	if(this->m_uiState == uiStateIndex || IHyLoadable::SetState(uiStateIndex) == false)
		return false;

#ifdef HY_USE_SPINE
	const HySpineData *pData = static_cast<const HySpineData *>(this->AcquireData());
	if(m_pAnimationState && pData && uiStateIndex < pData->GetSkeletonData()->getAnimations().size())
		m_pAnimationState->addAnimation(0, pData->GetSkeletonData()->getAnimations()[uiStateIndex], true, 0.0f);
#endif

	return true;
}

uint32 HySpine2d::GetNumSlots()
{
#ifdef HY_USE_SPINE
	if(AcquireData())
		return static_cast<uint32>(m_pSkeleton->getSlots().size());
#endif
	return 0;
}

/*virtual*/ bool HySpine2d::IsLoadDataValid() /*override*/
{
	const HySpineData *pData = static_cast<const HySpineData *>(this->AcquireData());
	return pData;
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
	m_pSkeletonBounds = HY_NEW spine::SkeletonBounds();
#endif
}

/*virtual*/ void HySpine2d::OnLoadedUpdate() /*override*/
{
#ifdef HY_USE_SPINE
	//m_pSkeleton->setX(pos.X());
	//m_pSkeleton->setY(pos.Y());
	//m_pSkeleton->setScaleX(scale.X());
	//m_pSkeleton->setScaleY(scale.Y());

	m_pAnimationState->update(HyEngine::DeltaTime());
	m_pAnimationState->apply(*m_pSkeleton);								// Apply the state to the skeleton
	m_pSkeleton->updateWorldTransform();								// Calculate world transforms for rendering
#endif
}

/*virtual*/ void HySpine2d::PrepRenderStage(uint32 uiStageIndex, HyRenderMode &eRenderModeOut, uint32 &uiNumInstancesOut, uint32 &uiNumVerticesPerInstOut, bool &bIsBatchable) /*override*/
{
#ifdef HY_USE_SPINE
	eRenderModeOut = HYRENDERMODE_TriangleStrip;
	uiNumVerticesPerInstOut = 4;
	bIsBatchable = true;

	if(uiStageIndex == 0)
		m_uiStartingSlotIndex = 0;

	// Used to determine when the render stage ends
	HyTextureHandle hCurTexture = HY_UNUSED_HANDLE;

	uiNumInstancesOut = 0;
	uint32 uiSlotIndex = m_uiStartingSlotIndex;
	for(; uiSlotIndex < m_pSkeleton->getDrawOrder().size(); ++uiSlotIndex)
	{
		spine::Slot *pCurSlot = m_pSkeleton->getDrawOrder()[uiSlotIndex];
		spine::Attachment *pAttachment = pCurSlot->getAttachment(); // Fetch the currently active attachment
		if(!pAttachment)
			continue; // continue with the next slot in the draw order if no attachment is active on the slot

		// TODO: Add 'blending' to the render state

		// Fetch the blend mode from the slot and translate it to the engine blend mode
		//spine::BlendMode engineBlendMode;
		//switch(pCurSlot->getData().getBlendMode())
		//{
		//case spine::BlendMode_Normal:		//		  int source, int sourcePMA, int destColor, int sourceAlpha
		//	engineBlendMode = BLEND_NORMAL; // normal(GL_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE)
		//	break;
		//case spine::BlendMode_Additive:
		//	engineBlendMode = BLEND_ADDITIVE; // additive(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE)
		//	break;
		//case spine::BlendMode_Multiply:
		//	engineBlendMode = BLEND_MULTIPLY; // multiply(GL_DST_COLOR, GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
		//	break;
		//case spine::BlendMode_Screen:
		//	engineBlendMode = BLEND_SCREEN; // screen(GL_ONE, GL_ONE, GL_ONE_MINUS_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR)
		//	break;
		//default:
		//	engineBlendMode = BLEND_NORMAL;
		//}

		if(pAttachment->getRTTI().isExactly(spine::RegionAttachment::rtti))
		{
			// Cast to an spine::RegionAttachment so we can get its AtlasRegion's, page's rendererObject, which is the index into *this subatlas list
			spine::RegionAttachment *pRegionAttachment = static_cast<spine::RegionAttachment *>(pAttachment);
			spine::AtlasRegion *pSpineAtlasRegion = static_cast<spine::AtlasRegion *>(pRegionAttachment->getRendererObject());
			HySpineAtlas *pSpineSubAtlas = reinterpret_cast<HySpineAtlas *>(pSpineAtlasRegion->page->getRendererObject());

			// Check if this texture handle ends this render stage
			HyTextureHandle hTex = pSpineSubAtlas->GetTexHandle();
			if(hCurTexture == HY_UNUSED_HANDLE)
				hCurTexture = hTex;
			else if(hCurTexture != hTex)
				break; // Break
		}
		else
			HyError("Spine attachment type not yet supported: " << std::string(pAttachment->getName().buffer()));

		// All passed above, increment number of instances to be rendered this stage
		uiNumInstancesOut++;
	}

	m_ShaderUniforms.SetTexHandle(0, hCurTexture);
#endif
}

/*virtual*/ bool HySpine2d::WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef) /*override*/
{
#ifdef HY_USE_SPINE
	uint32 uiNumSlots = static_cast<uint32>(m_pSkeleton->getDrawOrder().size());
	uint32 uiSlotIndex = m_uiStartingSlotIndex;

	for(; uiNumInstances > 0 && uiSlotIndex < uiNumSlots; ++uiSlotIndex) // For each slot in the draw order array of the skeleton
	{
		spine::Slot *pCurSlot = m_pSkeleton->getDrawOrder()[uiSlotIndex];

		// Fetch the currently active attachment
		spine::Attachment *pAttachment = pCurSlot->getAttachment();
		if(!pAttachment)
			continue; // continue with the next slot in the draw order if no attachment is active on the slot

		// Calculate the tinting color based on the skeleton's color and the slot's color. spine::Color is given in the range [0-1] for each color channel
		spine::Color skeletonColor = m_pSkeleton->getColor();
		spine::Color slotColor = pCurSlot->getColor();
		spine::Color slotTint(skeletonColor.r * slotColor.r, skeletonColor.g * slotColor.g, skeletonColor.b * slotColor.b, skeletonColor.a * slotColor.a);

		// Fill the vertices attributes based on the type of attachment
		if(pAttachment->getRTTI().isExactly(spine::RegionAttachment::rtti))
		{
			// Cast to an spRegionAttachment so we can get the rendererObject and compute the world vertices
			spine::RegionAttachment *pRegionAttachment = static_cast<spine::RegionAttachment *>(pAttachment);

			// Computed the world vertices positions for the 4 vertices that make up
			// the rectangular region attachment. This assumes the world transform of the
			// bone to which the slot (and hence attachment) is attached has been calculated
			// before rendering via Skeleton::updateWorldTransform(). The vertex positions
			// will be written directory into the vertices array, with a stride of sizeof(Vertex)
			spine::Vector<float> ptWorldPos;
			ptWorldPos.setSize(8, 0.0f);
			pRegionAttachment->computeWorldVertices(*pCurSlot, ptWorldPos, 0, 2);

			// These macros convert ptWorldPos[x] to Harmony's winding order
#define Vert2Index 0
#define Vert3Index 2
#define Vert1Index 4
#define Vert0Index 6

			// SIZE
			glm::vec2 vSize(abs(ptWorldPos[Vert2Index] - ptWorldPos[Vert3Index]), abs(ptWorldPos[Vert0Index + 1] - ptWorldPos[Vert2Index + 1]));
			vertexBufferRef.AppendData2d(&vSize, sizeof(glm::vec2));

			// OFFSET
			vertexBufferRef.AppendData2d(&ptWorldPos[Vert3Index], sizeof(glm::vec2));

			glm::vec4 vColor;
			// TOP COLOR
			HyCopyVec(vColor, CalculateTopTint());
			vColor.a = CalculateAlpha();
			vColor.r *= slotTint.r;
			vColor.g *= slotTint.g;
			vColor.b *= slotTint.b;
			vColor.a *= slotTint.a;
			vertexBufferRef.AppendData2d(&vColor, sizeof(glm::vec4));
			// BOT COLOR
			HyCopyVec(vColor, CalculateBotTint());
			vColor.a = CalculateAlpha();
			vColor.r *= slotTint.r;
			vColor.g *= slotTint.g;
			vColor.b *= slotTint.b;
			vColor.a *= slotTint.a;
			vertexBufferRef.AppendData2d(&vColor, sizeof(glm::vec4));
			
			// UV's
			//vUV.x = frameRef.rSRC_RECT.right;//1.0f;
			//vUV.y = frameRef.rSRC_RECT.top;//1.0f;
			vertexBufferRef.AppendData2d(&pRegionAttachment->getUVs()[Vert0Index], sizeof(glm::vec2));

			//vUV.x = frameRef.rSRC_RECT.left;//0.0f;
			//vUV.y = frameRef.rSRC_RECT.top;//1.0f;
			vertexBufferRef.AppendData2d(&pRegionAttachment->getUVs()[Vert1Index], sizeof(glm::vec2));

			//vUV.x = frameRef.rSRC_RECT.right;//1.0f;
			//vUV.y = frameRef.rSRC_RECT.bottom;//0.0f;
			vertexBufferRef.AppendData2d(&pRegionAttachment->getUVs()[Vert2Index], sizeof(glm::vec2));

			//vUV.x = frameRef.rSRC_RECT.left;//0.0f;
			//vUV.y = frameRef.rSRC_RECT.bottom;//0.0f;
			vertexBufferRef.AppendData2d(&pRegionAttachment->getUVs()[Vert3Index], sizeof(glm::vec2));

			// TRANSFORM MTX
			vertexBufferRef.AppendData2d(&GetSceneTransform(), sizeof(glm::mat4));
		}
		else if(pAttachment->getRTTI().isExactly(spine::MeshAttachment::rtti))
		{
			// Cast to an MeshAttachment so we can get the rendererObject
			// and compute the world vertices
			spine::MeshAttachment *pMeshAttachment = static_cast<spine::MeshAttachment *>(pAttachment);

			// Ensure there is enough room for vertices
			//vertices.setSize(pMeshAttachment->getWorldVerticesLength() / 2, Vertex());

			// Our engine specific Texture is stored in the AtlasRegion which was
			// assigned to the attachment on load. It represents the texture atlas
			// page that contains the image the region attachment is mapped to.
			//texture = (Texture *)((AtlasRegion *)mesh->getRendererObject())->page->getRendererObject();

			// Computed the world vertices positions for the vertices that make up
			// the mesh attachment. This assumes the world transform of the
			// bone to which the slot (and hence attachment) is attached has been calculated
			// before rendering via Skeleton::updateWorldTransform(). The vertex positions will
			// be written directly into the vertices array, with a stride of sizeof(Vertex)
			size_t numVertices = pMeshAttachment->getWorldVerticesLength() / 2;
			std::vector<glm::vec2> ptWorldPosList(numVertices);
			//pMeshAttachment->computeWorldVertices(pCurSlot, 0, numVertices, &ptWorldPosList.data()->x, 0, sizeof(glm::vec2));

			HyError("Mesh Attachments are not yet supported");
		}

		uiNumInstances--;
	} // Slot for-loop

	if(uiSlotIndex == uiNumSlots)
		return true;
	else
		m_uiStartingSlotIndex = uiSlotIndex;

	return false;
#else
	return true;
#endif
}

/*virtual*/ void HySpine2d::OnCalcBoundingVolume() /*override*/
{
}
