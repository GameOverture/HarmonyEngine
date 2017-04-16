/*******************************************************************************
 *	IHyDraw2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *********************************************************************************/
#include "Scene/Nodes/Draws/IHyDraw2d.h"
#include "HyEngine.h"
#include "Assets/Nodes/IHyNodeData.h"
#include "Assets/HyAssets.h"
#include "Scene/Nodes/Entities/HyEntity2d.h"
#include "Renderer/Components/HyWindow.h"
#include "Diagnostics/HyGuiComms.h"



IHyDraw2d::IHyDraw2d(HyType eInstType, const char *szPrefix, const char *szName, HyEntity2d *pParent /*= nullptr*/) :	IHyNode2d(eInstType, pParent),
																														
																														
																														
																														m_iDisplayOrder(0),
																														color(*this),
																														scissor(*this)
{
	m_bIsDraw2d = true;

#ifdef HY_DEBUG
	if(m_eTYPE != HYTYPE_Entity2d && m_eTYPE != HYTYPE_Primitive2d)
		HyAssert(m_sNAME.empty() == false, "IHyDraw2d of type '" << m_eTYPE << "' was constructed with a blank name");
#endif
}

/*virtual*/ IHyDraw2d::~IHyDraw2d(void)
{
	
}



//void IHyDraw2d::MakeBoundingVolumeDirty()
//{
//	m_uiAttributes |= ATTRIBFLAG_BoundingVolumeDirty;
//}

/*virtual*/ void IHyDraw2d::SetNewChildAttributes(IHyNode2d &childInst)
{
	IHyNode2d::SetNewChildAttributes(childInst);

	if(childInst.IsDraw2d())
	{
		if(GetCoordinateType() == HYCOORDTYPE_Screen)
			static_cast<IHyDraw2d &>(childInst).m_RenderState.Enable(HyRenderState::USINGSCREENCOORDS);
		else
			static_cast<IHyDraw2d &>(childInst).m_RenderState.Disable(HyRenderState::USINGSCREENCOORDS);

		static_cast<IHyDraw2d &>(childInst).alpha.Set(this->alpha.Get());

		// This will reassign all the display orders in the hierarchy, including this newly added child
		//_SetDisplayOrder(m_iDisplayOrder, false);
	}
}





/*virtual*/ void IHyDraw2d::InstUpdate()
{
	if((m_uiAttributes & ATTRIBFLAG_Scissor) != 0)
	{
		glm::mat4 mtx;
		GetWorldTransform(mtx);

		m_RenderState.SetScissorRect(static_cast<int32>(mtx[3].x + m_LocalScissorRect.x),
									 static_cast<int32>(mtx[3].y + m_LocalScissorRect.y),
									 static_cast<uint32>(mtx[0].x * m_LocalScissorRect.width),
									 static_cast<uint32>(mtx[1].y * m_LocalScissorRect.height));

		ForEachChild([&](IHyNode2d *pChildNode)
					{
						if(pChildNode->IsDraw2d())
							static_cast<IHyDraw2d *>(pChildNode)->m_RenderState.SetScissorRect(this->m_RenderState.GetScissorRect());
					});
	}

	if((m_uiAttributes & (ATTRIBFLAG_HasBoundingVolume | ATTRIBFLAG_MouseInput)) != 0)
	{
		if(m_uiAttributes & ATTRIBFLAG_BoundingVolumeDirty)
		{
			OnCalcBoundingVolume();
			m_uiAttributes &= ~ATTRIBFLAG_BoundingVolumeDirty;
		}

		if((m_uiAttributes & ATTRIBFLAG_MouseInput) != 0)
		{
			bool bLeftClickDown = IHyInputMap::IsMouseLeftDown();
			bool bMouseInBounds = m_BoundingVolume.IsWorldPointCollide(IHyInputMap::GetWorldMousePos());

			switch(m_eMouseInputState)
			{
			case MOUSEINPUT_None:
				if(bLeftClickDown == false && bMouseInBounds)
				{
					m_eMouseInputState = MOUSEINPUT_Hover;
					OnMouseEnter(m_pMouseInputUserParam);
				}
				break;

			case MOUSEINPUT_Hover:
				if(bMouseInBounds == false)
				{
					m_eMouseInputState = MOUSEINPUT_None;
					OnMouseLeave(m_pMouseInputUserParam);
				}
				else if(bLeftClickDown)
				{
					m_eMouseInputState = MOUSEINPUT_Down;
					OnMouseDown(m_pMouseInputUserParam);
				}
				break;

			case MOUSEINPUT_Down:
				if(bLeftClickDown == false)
				{
					m_eMouseInputState = MOUSEINPUT_None;
					OnMouseUp(m_pMouseInputUserParam);

					if(bMouseInBounds)
						OnMouseClicked(m_pMouseInputUserParam);
				}
				break;
			}
		}
	}

	if(m_fPrevAlphaValue != alpha.Get())
	{
		m_fPrevAlphaValue = alpha.Get();

		for(uint32 i = 0; i < m_ChildList.size(); ++i)
		{
			if(m_ChildList[i]->IsDraw2d())
				static_cast<IHyDraw2d *>(m_ChildList[i])->alpha.Set(alpha.Get());
		}

		//ForEachChild([&](IHyNode2d *pChildNode)
		//			{
		//				if(pChildNode->IsDraw2d())
		//				{
		//					static_cast<IHyDraw2d *>(pChildNode)->alpha.Set(this->alpha.Get());
		//				}
		//			});
	}

	OnUpdate();

	if(m_eLoadState == HYLOADSTATE_Loaded)
	{
		OnUpdateUniforms();
		m_RenderState.SetUniformCrc32(m_ShaderUniforms.GetCrc32());
	}
}

/*virtual*/ void IHyDraw2d::_SetScissor(HyScissor &scissorRef, bool bOverrideExplicitChildren)
{
	if(bOverrideExplicitChildren)
		m_uiExplicitFlags &= ~EXPLICIT_Scissor;

	if(0 == (m_uiExplicitFlags & EXPLICIT_DisplayOrder))
		scissor = scissorRef;
	//if(static_cast<IHyDraw2d *>(m_pParent)->m_RenderState.IsScissorRect())
	//{
	//	m_RenderState.SetScissorRect(static_cast<IHyDraw2d *>(m_pParent)->m_RenderState.GetScissorRect());
	//}
}

/*virtual*/ int32 IHyDraw2d::_SetDisplayOrder(int32 iOrderValue, bool bOverrideExplicitChildren)
{
	if(bOverrideExplicitChildren)
		m_uiExplicitFlags &= ~EXPLICIT_DisplayOrder;

	if(0 == (m_uiExplicitFlags & EXPLICIT_DisplayOrder))
	{
		++iOrderValue;
		m_iDisplayOrder = iOrderValue;

		for(uint32 i = 0; i < m_ChildList.size(); ++i)
		{
			if(m_ChildList[i]->IsDraw2d())
				iOrderValue = static_cast<IHyDraw2d *>(m_ChildList[i])->_SetDisplayOrder(iOrderValue, bOverrideExplicitChildren);
		}
	}

	return iOrderValue;
}
