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
	// NOTE: m_PrimOutline does not have a parent because it is projected to window coordinates
	m_PrimOutline.UseWindowCoordinates();
	m_PrimOutline.SetWireframe(true);
	m_PrimOutline.SetDisplayOrder(DISPLAYORDER_TransformCtrl - 1);
}

/*virtual*/ GfxChainView::~GfxChainView()
{
	ClearPreviewPrimitives();
}

/*virtual*/ void GfxChainView::RefreshColor() /*override*/
{
	if(m_pModel == nullptr)
		return;

	m_PrimOutline.SetTint(m_pModel->GetColor());
}

/*virtual*/ void GfxChainView::DoRefreshView(EditModeState eEditModeState, ShapeMouseMoveResult eResult) /*override*/
{
	if(m_pModel == nullptr)
	{
		m_PrimOutline.SetAsNothing();
		ClearPreviewPrimitives();
		return;
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Sync Primitives with Model
	HyCamera2d *pCamera = HyEngine::Window().GetCamera2d(0);
		
	const HyChainData &chainDataRef = static_cast<GfxChainModel *>(m_pModel)->GetChainFixture().GetChainData();
	std::vector<glm::vec2> projectedVertList;
	for(int i = 0; i < chainDataRef.iCount; ++i)
	{
		glm::vec2 ptScreenPos;
		pCamera->ProjectToCamera(chainDataRef.pPointList[i], ptScreenPos);
		projectedVertList.push_back(ptScreenPos);
	}
	m_PrimOutline.SetAsLineChain(projectedVertList, chainDataRef.bLoop);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Apply Transform Preview if needed
	const QList<GfxGrabPointModel> &grabPointModelList = m_pModel->GetGrabPointList();
	glm::mat4 mtxTransform(1.0f);
	int iVertexIndex = -1;
	m_pModel->GetTransformPreview(mtxTransform, iVertexIndex);
	glm::vec4 vTranslate = mtxTransform[3];

	switch(eResult)
	{
	case SHAPEMOUSEMOVE_Creation:
		break;
	case SHAPEMOUSEMOVE_Outside:
		break;
	case SHAPEMOUSEMOVE_Inside:
		break;

	case SHAPEMOUSEMOVE_AppendVertex: {
		if(static_cast<GfxChainModel *>(m_pModel)->IsLoopClosed() || grabPointModelList.empty())
		{
			HyGuiLog("GfxChainView::RefreshView called with closed loop (or grab points empty)", LOGTYPE_Error);
			break;
		}

		glm::vec2 ptNewVertex = grabPointModelList[iVertexIndex].GetPos();
		ptNewVertex += glm::vec2(vTranslate.x, vTranslate.y);
		pCamera->ProjectToCamera(ptNewVertex, ptNewVertex);
		m_GrabPointViewList[iVertexIndex]->pos.Set(ptNewVertex);

		glm::vec2 ptEndPoint;
		if(grabPointModelList.front().IsSelected())
			ptEndPoint = grabPointModelList.front().GetPos();
		else
			ptEndPoint = grabPointModelList.back().GetPos();
		pCamera->ProjectToCamera(ptEndPoint, ptEndPoint);

		if(m_PrimPreviewList.size() != 1)
		{
			ClearPreviewPrimitives();
			m_PrimPreviewList.append(new HyPrimitive2d(this));
		}
		m_PrimPreviewList[0]->UseWindowCoordinates();
		m_PrimPreviewList[0]->SetTint(HyGlobal::GetEditorColor(EDITORCOLOR_EditMode));
		m_PrimPreviewList[0]->SetDisplayOrder(DISPLAYORDER_TransformCtrl - 2);
		m_PrimPreviewList[0]->SetAsLineSegment(ptNewVertex, ptEndPoint);
		break; }

	case SHAPEMOUSEMOVE_InsertVertex: {
		
		if(grabPointModelList.size() < 2)
		{
			HyGuiLog("GfxChainView::RefreshView called with less than 2 grab points", LOGTYPE_Error);
			break;
		}

		glm::vec2 ptInsertVertex = grabPointModelList[iVertexIndex].GetPos();
		ptInsertVertex += glm::vec2(vTranslate.x, vTranslate.y);
		pCamera->ProjectToCamera(ptInsertVertex, ptInsertVertex);
		m_GrabPointViewList[iVertexIndex]->pos.Set(ptInsertVertex);

		glm::vec2 ptConnectPoint1 = grabPointModelList[(iVertexIndex + 1) % grabPointModelList.size()].GetPos();
		pCamera->ProjectToCamera(ptConnectPoint1, ptConnectPoint1);

		glm::vec2 ptConnectPoint2;
		if(iVertexIndex == 0)
			ptConnectPoint2 = grabPointModelList[grabPointModelList.size() - 1].GetPos();
		else
			ptConnectPoint2 = grabPointModelList[iVertexIndex - 1].GetPos();
		pCamera->ProjectToCamera(ptConnectPoint2, ptConnectPoint2);

		if(m_PrimPreviewList.size() != 2)
		{
			ClearPreviewPrimitives();
			m_PrimPreviewList.append(new HyPrimitive2d(this));
			m_PrimPreviewList.append(new HyPrimitive2d(this));
		}
		m_PrimPreviewList[0]->UseWindowCoordinates();
		m_PrimPreviewList[0]->SetTint(HyGlobal::GetEditorColor(EDITORCOLOR_EditMode));
		m_PrimPreviewList[0]->SetDisplayOrder(DISPLAYORDER_TransformCtrl - 2);
		m_PrimPreviewList[0]->SetAsLineSegment(ptInsertVertex, ptConnectPoint1);
		m_PrimPreviewList[1]->UseWindowCoordinates();
		m_PrimPreviewList[1]->SetTint(HyGlobal::GetEditorColor(EDITORCOLOR_EditMode));
		m_PrimPreviewList[1]->SetDisplayOrder(DISPLAYORDER_TransformCtrl - 2);
		m_PrimPreviewList[1]->SetAsLineSegment(ptInsertVertex, ptConnectPoint2);
		break; }

	case SHAPEMOUSEMOVE_HoverGrabPoint:
			DoHoverGrabPoint(eEditModeState);
		break;

	case SHAPEMOUSEMOVE_HoverCenter:
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
}

void GfxChainView::ClearPreviewPrimitives()
{
	for(HyPrimitive2d *pPrim : m_PrimPreviewList)
		delete pPrim;
	m_PrimPreviewList.clear();
}

void GfxChainView::DoHoverGrabPoint(EditModeState eEditModeState)
{
	const QList<GfxGrabPointModel> &grabPointModelList = m_pModel->GetGrabPointList();
	glm::mat4 mtxTransform(1.0f);
	int iVertexIndex = -1;
	m_pModel->GetTransformPreview(mtxTransform, iVertexIndex);
	glm::vec4 vTranslate = mtxTransform[3];

	if(iVertexIndex < 0 || iVertexIndex >= grabPointModelList.size())
	{
		HyGuiLog("GfxChainView::DoHoverGrabPoint - invalid m_iVertexIndex", LOGTYPE_Error);
		return;
	}
	if(m_pModel->IsHoverGrabPointSelected() == false)
		HyGuiLog("GfxChainView::DoHoverGrabPoint - Hover vertex not selected on box transform", LOGTYPE_Error);

	// Apply grab point drag logic based on shape type
}
