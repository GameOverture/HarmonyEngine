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
	m_pSkeleton(nullptr),
	m_pAnimationState(nullptr),
	m_pSkeletonBounds(nullptr)
{
	m_eRenderMode = HYRENDERMODE_TriangleStrip;
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

	m_pSkeleton->update(HyEngine::DeltaTime() /** m_fAnimPlayRate*/);	// Update the time field used for attachments and such
	m_pAnimationState->apply(*m_pSkeleton);								// Apply the state to the skeleton
	m_pSkeleton->updateWorldTransform();								// Calculate world transforms for rendering
#endif
}

/*virtual*/ bool HySpine2d::WriteVertexData(HyVertexBuffer &vertexBufferRef) /*override*/
{
#ifdef HY_USE_SPINE
	spine::Slot *pCurSlot = nullptr;
	uint32 uiNumSlots = GetNumSlots();
	for(uint32 i = 0; i < uiNumSlots; ++i) // For each slot in the draw order array of the skeleton
	{
		pCurSlot = m_pSkeleton->getDrawOrder()[i];

		// Fetch the currently active attachment
		spine::Attachment *pAttachment = pCurSlot->getAttachment();
		if(!pAttachment)
			continue; // continue with the next slot in the draw order if no attachment is active on the slot

		// Fetch the blend mode from the slot and translate it to the engine blend mode
		spine::BlendMode engineBlendMode;
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

		// Calculate the tinting color based on the skeleton's color and the slot's color. spine::Color is given in the range [0-1] for each color channel
		spine::Color skeletonColor = m_pSkeleton->getColor();
		spine::Color slotColor = pCurSlot->getColor();
		spine::Color slotTint(skeletonColor.r * slotColor.r, skeletonColor.g * slotColor.g, skeletonColor.b * slotColor.b, skeletonColor.a * slotColor.a);

		// Fill the vertices attributes based on the type of attachment
		if(pAttachment->getRTTI().isExactly(spine::RegionAttachment::rtti))
		{
			// Cast to an spRegionAttachment so we can get the rendererObject and compute the world vertices
			spine::RegionAttachment *pRegionAttachment = static_cast<spine::RegionAttachment *>(pAttachment);

			//static_cast<spine::AtlasRegion *>(pRegionAttachment->getRendererObject())->page->name

			// Our engine specific Texture is stored in the AtlasRegion which was
			// assigned to the attachment on load. It represents the texture atlas
			// page that contains the image the region attachment is mapped to.
			//texture = (Texture *)((spine::AtlasRegion *)pRegionAttachment->getRendererObject())->page->getRendererObject();

			// Computed the world vertices positions for the 4 vertices that make up
			// the rectangular region attachment. This assumes the world transform of the
			// bone to which the slot (and hence attachment) is attached has been calculated
			// before rendering via Skeleton::updateWorldTransform(). The vertex positions
			// will be written directory into the vertices array, with a stride of sizeof(Vertex)
			spine::Vector<float> ptWorldPos;
			ptWorldPos.setSize(8, 0.0f);
			pRegionAttachment->computeWorldVertices(pCurSlot->getBone(), ptWorldPos, 0, 2);

			// SIZE
#define GetPosX(index) ptWorldPos[index * 2]
#define GetPosY(index) ptWorldPos[index * 2 + 1]
			glm::vec2 vSize(abs(GetPosX(0) - GetPosX(3)), abs(GetPosY(2) - GetPosY(3)));
			vertexBufferRef.AppendData2d(&vSize, sizeof(glm::vec2));

			// OFFSET
			vertexBufferRef.AppendData2d(&ptWorldPos[0], sizeof(glm::vec2));

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
#define GetUVs(index) &pRegionAttachment->getUVs()[index * 2]
			//vUV.x = frameRef.rSRC_RECT.right;//1.0f;
			//vUV.y = frameRef.rSRC_RECT.top;//1.0f;
			vertexBufferRef.AppendData2d(GetUVs(2), sizeof(glm::vec2));

			//vUV.x = frameRef.rSRC_RECT.left;//0.0f;
			//vUV.y = frameRef.rSRC_RECT.top;//1.0f;
			vertexBufferRef.AppendData2d(GetUVs(1), sizeof(glm::vec2));

			//vUV.x = frameRef.rSRC_RECT.right;//1.0f;
			//vUV.y = frameRef.rSRC_RECT.bottom;//0.0f;
			vertexBufferRef.AppendData2d(GetUVs(3), sizeof(glm::vec2));

			//vUV.x = frameRef.rSRC_RECT.left;//0.0f;
			//vUV.y = frameRef.rSRC_RECT.bottom;//0.0f;
			vertexBufferRef.AppendData2d(GetUVs(0), sizeof(glm::vec2));

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

		//// Draw the mesh we created for the attachment
		//engine_drawMesh(vertices, 0, vertexIndex, texture, engineBlendMode);
	}
#endif

	return true;
}

/*virtual*/ void HySpine2d::OnCalcBoundingVolume() /*override*/
{
}
