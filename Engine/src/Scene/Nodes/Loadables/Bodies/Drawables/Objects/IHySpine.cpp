///**************************************************************************
// *	IHySpine.cpp
// *	
// *	Harmony Engine
// *	Copyright (c) 2022 Jason Knobler
// *
// *	Harmony License:
// *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
// *************************************************************************/
//#include "Afx/HyStdAfx.h"
//#include "HyEngine.h"
//#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/IHySpine.h"
//#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable2d.h"
//#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable3d.h"
//#include "Assets/Nodes/HySpineData.h"
//#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
//
//template<typename NODETYPE, typename ENTTYPE>
//IHySpine<NODETYPE, ENTTYPE>::IHySpine(std::string sPrefix, std::string sName, ENTTYPE *pParent) :
//	NODETYPE(HYTYPE_Spine, sPrefix, sName, pParent),
//	m_pSkeleton(nullptr),
//	m_pAnimationState(nullptr),
//	m_pSkeletonBounds(nullptr)
//{
//	this->m_eRenderMode = HYRENDERMODE_TriangleStrip;
//}
//
//template<typename NODETYPE, typename ENTTYPE>
//IHySpine<NODETYPE, ENTTYPE>::IHySpine(const IHySpine &copyRef) :
//	NODETYPE(copyRef)
//{
//}
//
//template<typename NODETYPE, typename ENTTYPE>
///*virtual*/ IHySpine<NODETYPE, ENTTYPE>::~IHySpine(void)
//{
//#ifdef HY_USE_SPINE
//	delete m_pSkeleton;
//	delete m_pAnimationState;
//	delete m_pSkeletonBounds;
//#endif
//}
//
//template<typename NODETYPE, typename ENTTYPE>
//const IHySpine<NODETYPE, ENTTYPE> &IHySpine<NODETYPE, ENTTYPE>::operator=(const IHySpine<NODETYPE, ENTTYPE> &rhs)
//{
//	NODETYPE::operator=(rhs);
//	return *this;
//}
//
//template<typename NODETYPE, typename ENTTYPE>
//uint32 IHySpine<NODETYPE, ENTTYPE>::GetNumSlots() const
//{
//	return 0;
//}
//
//template<typename NODETYPE, typename ENTTYPE>
///*virtual*/ bool IHySpine<NODETYPE, ENTTYPE>::IsLoadDataValid() /*override*/
//{
//	const HySpineData *pData = static_cast<const HySpineData *>(this->AcquireData());
//	return pData;
//}
//
//template<typename NODETYPE, typename ENTTYPE>
///*virtual*/ bool IHySpine<NODETYPE, ENTTYPE>::OnIsValidToRender() /*override*/
//{
//	return true;
//}
//
//template<typename NODETYPE, typename ENTTYPE>
///*virtual*/ void IHySpine<NODETYPE, ENTTYPE>::OnDataAcquired() /*override*/
//{
//	const HySpineData *pData = static_cast<const HySpineData *>(this->UncheckedGetData());
//	
//#ifdef HY_USE_SPINE
//	m_pSkeleton = HY_NEW spine::Skeleton(pData->GetSkeletonData());
//	m_pAnimationState = HY_NEW spine::AnimationState(pData->GetAnimationStateData());
//	m_pSkeletonBounds = HY_NEW spine::SkeletonBounds();
//#endif
//}
//
//template<typename NODETYPE, typename ENTTYPE>
///*virtual*/ void IHySpine<NODETYPE, ENTTYPE>::OnLoadedUpdate() /*override*/
//{
//	// Update the time field used for attachments and such
////	spSkeleton_update(m_pSpineSkeleton, HyEngine::DeltaTime());
////
////	// Calculate the animation state
////	for(uint32 i = 0; i < m_uiNumAnimStates; ++i)
////	{
////		if(m_pIsAnimStateEnabled[i])
////		{
////			spAnimationState_update(m_ppAnimStates[i], HyEngine::DeltaTime() * m_fAnimPlayRate);
////			spAnimationState_apply(m_ppAnimStates[i], m_pSpineSkeleton);
////		}
////	}
////	
////	// Update the transform
////	//float fCoordModifier = m_eCoordType == HYCOORD_Meter ? HyScene::PixelsPerMeter() : 1.0f;
////	//m_pSpineSkeleton->x = m_ptPosition.Get().x * fCoordModifier;
////	//m_pSpineSkeleton->y = m_ptPosition.Get().y * fCoordModifier;
////	
////	// TODO: Use botColor as well
////	const glm::vec3 &tint = CalculateTopTint();
////	m_pSpineSkeleton->r = tint.r;
////	m_pSpineSkeleton->g = tint.g;
////	m_pSpineSkeleton->b = tint.b;
////	m_pSpineSkeleton->a = CalculateAlpha();
////
////	//m_pSpineSkeleton->root->rotation = m_vRotation.Get().z;
////
////	//m_pSpineSkeleton->root->scaleX = m_vScale.Get().x;
////	//m_pSpineSkeleton->root->scaleY = m_vScale.Get().y;
////
////	// This actually produces the Local transform as far as Harmony is concerned
////	spSkeleton_updateWorldTransform(m_pSpineSkeleton);
//}
