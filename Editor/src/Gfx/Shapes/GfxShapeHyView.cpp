/**************************************************************************
 *	GfxShapeHyView.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "GfxShapeHyView.h"
#include "GfxGrabPointView.h"

GfxShapeHyView::GfxShapeHyView(HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent)
{
	// NOTE: m_PrimOutline does not have a parent because it is projected to window coordinates
	m_PrimOutline.UseWindowCoordinates();
	m_PrimOutline.SetWireframe(true);
	m_PrimOutline.SetDisplayOrder(DISPLAYORDER_TransformCtrl - 1);
}

/*virtual*/ GfxShapeHyView::~GfxShapeHyView()
{
	ClearPrimitives();
	ClearGrabPoints();
}

HyPrimitive *GfxShapeHyView::GetPrimitive(int iIndex)
{
	if(iIndex < 0 || iIndex >= m_PrimList.size())
	{
		HyGuiLog("GfxShapeHyView::GetPrimitive index out of range", LOGTYPE_Error);
		return nullptr;
	}

	return m_PrimList[iIndex];
}

/*virtual*/ void GfxShapeHyView::RefreshColor() /*override*/
{
	if(m_pModel == nullptr)
		return;

	HyColor color = m_pModel->GetColor();
	bool bIsDark = color.IsDark();
	for(HyPrimitive2d *pPrim : m_PrimList)
	{
		pPrim->SetTint(m_pModel->GetColor());
		if(bIsDark)
			color = color.Lighten();
		else
			color = color.Darken();
	}
	m_PrimOutline.SetTint(bIsDark ? HyColor::White : HyColor::Black);
}

/*virtual*/ void GfxShapeHyView::RefreshView(ShapeMouseMoveResult eResult, bool bMouseDown) /*override*/
{
	if(m_pModel == nullptr || m_pModel->GetType() == SHAPE_None)
	{
		ClearPrimitives();
		m_PrimOutline.SetAsNothing();
		ClearGrabPoints();
		return;
	}

	RefreshColor();
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Sync Primitives with Model
	HyCamera2d *pCamera = HyEngine::Window().GetCamera2d(0);
	if(m_pModel->GetType() == SHAPE_LineChain)
	{
		ClearPrimitives();
		
		const HyChainData &chainDataRef = static_cast<HyChain2d *>(m_pModel->GetFixture(0))->GetChainData();
		std::vector<glm::vec2> projectedVertList;
		for(int i = 0; i < chainDataRef.iCount; ++i)
		{
			glm::vec2 ptScreenPos;
			pCamera->ProjectToCamera(chainDataRef.pPointList[i], ptScreenPos);
			projectedVertList.push_back(ptScreenPos);
		}
		m_PrimOutline.SetAsLineChain(projectedVertList, chainDataRef.bLoop);
	}
	else if(m_pModel->GetType() != SHAPE_Polygon)
	{
		if(m_PrimList.size() != 1)
		{
			ClearPrimitives();
			HyPrimitive2d *pNewPrim = new HyPrimitive2d(this);
			m_PrimList.push_back(pNewPrim);
		}
		m_PrimList[0]->SetAsShape(*static_cast<HyShape2d *>(m_pModel->GetFixture(0)));

		// Set `m_PrimOutline`
		if(m_pModel->GetType() == SHAPE_Box)
		{
			b2Vec2 *pVerts = static_cast<HyShape2d *>(m_pModel->GetFixture(0))->GetAsPolygon().vertices;
			std::vector<glm::vec2> projectedVertList;
			for(int i = 0; i < 4; ++i)
			{
				glm::vec2 ptScreenPos;
				pCamera->ProjectToCamera(glm::vec2(pVerts[i].x, pVerts[i].y), ptScreenPos);
				projectedVertList.push_back(ptScreenPos);
			}

			m_PrimOutline.SetAsPolygon(projectedVertList);
		}
		else if(m_pModel->GetType() == SHAPE_Circle)
		{
			b2Circle circle = static_cast<HyShape2d *>(m_pModel->GetFixture(0))->GetAsCircle();

			glm::vec2 ptCenter(circle.center.x, circle.center.y);
			pCamera->ProjectToCamera(ptCenter, ptCenter);
			float fRadius = circle.radius * pCamera->GetZoom();

			m_PrimOutline.SetAsCircle(ptCenter, fRadius);
		}
		else if(m_pModel->GetType() == SHAPE_LineSegment)
		{
			b2Segment seg = static_cast<HyShape2d *>(m_pModel->GetFixture(0))->GetAsSegment();
			glm::vec2 ptOne(seg.point1.x, seg.point1.y);
			pCamera->ProjectToCamera(ptOne, ptOne);
			glm::vec2 ptTwo(seg.point2.x, seg.point2.y);
			pCamera->ProjectToCamera(ptTwo, ptTwo);

			m_PrimOutline.SetAsLineSegment(ptOne, ptTwo);
		}
		else if(m_pModel->GetType() == SHAPE_Capsule)
		{
			b2Capsule capsule = static_cast<HyShape2d *>(m_pModel->GetFixture(0))->GetAsCapsule();
			glm::vec2 ptOne(capsule.center1.x, capsule.center1.y);
			pCamera->ProjectToCamera(ptOne, ptOne);
			glm::vec2 ptTwo(capsule.center2.x, capsule.center2.y);
			pCamera->ProjectToCamera(ptTwo, ptTwo);
			float fRadius = capsule.radius * pCamera->GetZoom();

			m_PrimOutline.SetAsLineSegment(ptOne, ptTwo);
		}
		else
			HyGuiLog("GfxShapeHyView::RefreshView - Unsupported shape type for primitive sync", LOGTYPE_Error);
	}
	else // SHAPE_Polygon
	{
		int iNumFixtures = m_pModel->GetNumFixtures();
		while(m_PrimList.size() < iNumFixtures)
		{
			HyPrimitive2d *pNewPrim = new HyPrimitive2d(this);
			m_PrimList.push_back(pNewPrim);
		}
		while(m_PrimList.size() > iNumFixtures)
		{
			delete m_PrimList.back();
			m_PrimList.pop_back();
		}
		for(int iIndex = 0; iIndex < iNumFixtures; ++iIndex)
			m_PrimList[iIndex]->SetAsShape(*static_cast<HyShape2d *>(m_pModel->GetFixture(iIndex)));

		// Set `m_PrimOutline`
		const QList<GfxGrabPointModel> &grabPointModelList = m_pModel->GetGrabPointList();
		std::vector<glm::vec2> projectedVertList;
		for(int i = 0; i < grabPointModelList.size(); ++i)
		{
			glm::vec2 ptScreenPos;
			pCamera->ProjectToCamera(grabPointModelList[i].GetPos(), ptScreenPos);
			projectedVertList.push_back(ptScreenPos);
		}
		m_PrimOutline.SetAsLineChain(projectedVertList, m_pModel->IsLoopClosed());
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Sync Grab Point Views with Model
	const QList<GfxGrabPointModel> &grabPointModelList = m_pModel->GetGrabPointList();
	while(static_cast<uint32>(m_GrabPointViewList.size()) > grabPointModelList.size())
	{
		delete m_GrabPointViewList.back();
		m_GrabPointViewList.pop_back();
	}
	while(static_cast<uint32>(m_GrabPointViewList.size()) < grabPointModelList.size())
		m_GrabPointViewList.push_back(new GfxGrabPointView(this));

	for(int i = 0; i < grabPointModelList.size(); ++i)
		m_GrabPointViewList[i]->Sync(&grabPointModelList[i]);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Apply Transform Preview if needed

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
		if(m_pModel->GetType() != SHAPE_Polygon && m_pModel->GetType() != SHAPE_LineChain)
		{
			HyGuiLog("GfxShapeHyView::RefreshView - SHAPEMOUSEMOVE_AppendVertex - called with non-polygon/linechain shape type", LOGTYPE_Error);
			break;
		}
		if(m_pModel->IsLoopClosed() || grabPointModelList.empty())
		{
			HyGuiLog("GfxShapeHyView::RefreshView called with closed loop (or grab points empty)", LOGTYPE_Error);
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
		if(m_pModel->GetType() != SHAPE_Polygon && m_pModel->GetType() != SHAPE_LineChain)
		{
			HyGuiLog("GfxShapeHyView::RefreshView - SHAPEMOUSEMOVE_InsertVertex - called with non-polygon/linechain shape type", LOGTYPE_Error);
			break;
		}
		if(grabPointModelList.size() < 2)
		{
			HyGuiLog("GfxShapeHyView::RefreshView called with less than 2 grab points", LOGTYPE_Error);
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

	case SHAPEMOUSEMOVE_HoverVertex:
		break;
	case SHAPEMOUSEMOVE_HoverSelectedVertex:
		break;
	case SHAPEMOUSEMOVE_HoverCenter:
		if(bMouseDown)
		{
			ClearPreviewPrimitives();
			for(HyPrimitive2d *pPrim : m_PrimList)
			{
				m_PrimPreviewList.append(new HyPrimitive2d(this));
				*m_PrimPreviewList.last() = *pPrim;

				pPrim->alpha.Set(0.25f);
			}

			for(HyPrimitive2d *pPrim : m_PrimPreviewList)
			{
				HyShape2d tmpShape;
				pPrim->CalcLocalBoundingShape(tmpShape);
				tmpShape.TransformSelf(mtxTransform);
				pPrim->SetAsShape(tmpShape);
			}
			
			for(GfxGrabPointView *pGrabPtView : m_GrabPointViewList)
				pGrabPtView->pos.Offset(vTranslate.x, vTranslate.y);
		}
		break;
	}
}

void GfxShapeHyView::ClearPrimitives()
{
	for(HyPrimitive2d *pPrim : m_PrimList)
		delete pPrim;
	m_PrimList.clear();
}

void GfxShapeHyView::ClearGrabPoints()
{
	for(GfxGrabPointView *pGrabPtView : m_GrabPointViewList)
		delete pGrabPtView;
	m_GrabPointViewList.clear();
}

void GfxShapeHyView::ClearPreviewPrimitives()
{
	for(HyPrimitive2d *pPrim : m_PrimPreviewList)
		delete pPrim;
	m_PrimPreviewList.clear();
}
