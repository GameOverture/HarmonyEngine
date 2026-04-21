/**************************************************************************
 *	GfxChainView.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "GfxChainView.h"
#include "GfxChainModel.h"
#include "GfxGrabPointView.h"

GfxChainView::GfxChainView(HyEntity2d *pParent /*= nullptr*/) :
	IGfxEditView(pParent)
{
	// NOTE: m_DataPrim does not have a parent because it is projected to window coordinates
	m_DataPrim.UseWindowCoordinates();
	m_DataPrim.SetDisplayOrder(DISPLAYORDER_TransformCtrl - 1);
}

/*virtual*/ GfxChainView::~GfxChainView()
{
}

/*virtual*/ void GfxChainView::OnSyncModel(EditModeState eEditModeState, EditModeAction eEditModeAction) /*override*/
{
	if(eEditModeState == EDITMODE_Off || m_pModel == nullptr)
	{
		
		m_DataPrim.SetAsNothing(0);
		return;
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Sync Primitives with Model
	HyCamera2d *pCamera = HyEngine::Window().GetCamera2d(0);
		
	const HyChainData &chainDataRef = static_cast<GfxChainModel *>(m_pModel)->GetChain()->GetChainData();
	std::vector<glm::vec2> projectedVertList;
	for(int i = 0; i < chainDataRef.iCount; ++i)
	{
		glm::vec2 ptScreenPos;
		pCamera->ProjectToCamera(chainDataRef.pPointList[i], ptScreenPos);
		projectedVertList.push_back(ptScreenPos);
	}

	const QList<GfxGrabPointModel> &grabPointModelList = m_pModel->GetGrabPointList();

	switch(eEditModeAction)
	{
	case EDITMODEACTION_Creation:
		break;
	case EDITMODEACTION_Outside:
		break;
	case EDITMODEACTION_Inside:
		break;

	case EDITMODEACTION_AppendVertex: {



		break; }

	case EDITMODEACTION_InsertVertex: {
		
		
		break; }

	case EDITMODEACTION_HoverGrabPoint: {
		//const QList<GfxGrabPointModel> &grabPointModelList = m_pModel->GetGrabPointList();
		//if(iGrabPointIndex < 0 || iGrabPointIndex >= grabPointModelList.size())
		//{
		//	HyGuiLog("GfxChainView::DoHoverGrabPoint - invalid m_iGrabPointIndex", LOGTYPE_Error);
		//	return;
		//}
		//if(m_pModel->IsHoverGrabPointSelected() == false)
		//	HyGuiLog("GfxChainView::DoHoverGrabPoint - Hover vertex not selected on box transform", LOGTYPE_Error);

		// Apply grab point drag logic based on shape type

		break; }

	case EDITMODEACTION_HoverCenter:
		if(eEditModeState == EDITMODE_MouseDownTransform)
		{
			//ClearPreviewPrimitives();
			//m_PrimPreviewList.append(new HyPrimitive2d(this));
			//*m_PrimPreviewList.last() = m_PrimOutline;

			//pPrim->alpha.Set(0.25f);
			//

			//for(HyPrimitive2d *pPrim : m_PrimPreviewList)
			//{
			//	HyShape2d tmpShape;
			//	pPrim->CalcLocalBoundingShape(tmpShape);
			//	tmpShape.TransformSelf(mtxTransform);
			//	pPrim->SetAsShape(tmpShape);
			//}
			//
			//for(GfxGrabPointView *pGrabPtView : m_GrabPointViewList)
			//	pGrabPtView->pos.Offset(vTranslate.x, vTranslate.y);
		}
		break;
	}

	m_DataPrim.SetAsLineChain(0, projectedVertList, chainDataRef.bLoop, 1.0f);
}

GfxChainModel *GfxChainView::GetChainModel()
{
	return static_cast<GfxChainModel *>(m_pModel);
}
