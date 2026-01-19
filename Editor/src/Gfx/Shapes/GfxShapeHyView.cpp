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

Polygon2dHyView::Polygon2dHyView(HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent)
{
	// NOTE: m_PrimOutline does not have a parent because it is projected to window coordinates
	m_PrimOutline.UseWindowCoordinates();
	m_PrimOutline.SetWireframe(true);
	m_PrimOutline.SetDisplayOrder(DISPLAYORDER_TransformCtrl - 1);

	m_AppendSegmentLine.UseWindowCoordinates();
	m_AppendSegmentLine.SetTint(HyGlobal::GetEditorColor(EDITORCOLOR_EditMode));
	m_AppendSegmentLine.SetDisplayOrder(DISPLAYORDER_TransformCtrl - 2);
	m_AppendSegmentLine.SetVisible(false);
}

/*virtual*/ Polygon2dHyView::~Polygon2dHyView()
{
	ClearPrimitives();
	ClearGrabPoints();
}

HyPrimitive *Polygon2dHyView::GetPrimitive(int iIndex)
{
	if(iIndex < 0 || iIndex >= m_PrimList.size())
	{
		HyGuiLog("Polygon2dHyView::GetPrimitive index out of range", LOGTYPE_Error);
		return nullptr;
	}

	return m_PrimList[iIndex];
}

/*virtual*/ void Polygon2dHyView::RefreshColor() /*override*/
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

/*virtual*/ void Polygon2dHyView::RefreshView(bool bTransformPreview) /*override*/
{
	if(m_pModel == nullptr)
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
			HyGuiLog("Polygon2dHyView::RefreshView - Unsupported shape type for primitive sync", LOGTYPE_Error);
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
}

/*virtual*/ void Polygon2dHyView::OnMouseMoveIdle(ShapeMouseMoveResult eResult) /*override*/
{
	HyCamera2d *pCamera = HyEngine::Window().GetCamera2d(0);

	switch(eResult)
	{
	case SHAPEMOUSEMOVE_Outside:
	case SHAPEMOUSEMOVE_Inside:
		m_AppendSegmentLine.SetVisible(false);
		break;

	case SHAPEMOUSEMOVE_Crosshair:
		if(m_pModel->GetType() == SHAPE_Polygon || m_pModel->GetType() == SHAPE_LineChain)
		{
			const QList<GfxGrabPointModel> &grabPointModelList = m_pModel->GetGrabPointList();

			glm::vec2 pt1;
			if(grabPointModelList.front().IsSelected())
				pt1 = grabPointModelList.front().GetPos();
			else
				pt1 = grabPointModelList.back().GetPos();
			pCamera->ProjectToCamera(pt1, pt1);

			glm::vec2 pt2 = HyEngine::Input().GetMousePos();

			m_AppendSegmentLine.SetAsLineSegment(pt1, pt2);
			m_AppendSegmentLine.SetVisible(true);
		}
		break;

	case SHAPEMOUSEMOVE_AddVertex:
		m_AppendSegmentLine.SetVisible(false);
		break;

	case SHAPEMOUSEMOVE_HoverVertex:
	case SHAPEMOUSEMOVE_HoverSelectedVertex:
		m_AppendSegmentLine.SetVisible(false);
		break;

	case SHAPEMOUSEMOVE_HoverCenter:
		m_AppendSegmentLine.SetVisible(false);
		break;

	default:
		HyGuiLog("Polygon2dModel::MouseMoveEvent - Unknown ShapeMouseMoveResult encountered", LOGTYPE_Error);
		break;
	}
}

void Polygon2dHyView::ClearPrimitives()
{
	for(HyPrimitive2d *pPrim : m_PrimList)
		delete pPrim;
	m_PrimList.clear();
}

void Polygon2dHyView::ClearGrabPoints()
{
	for(GfxGrabPointView *pGrabPtView : m_GrabPointViewList)
		delete pGrabPtView;
	m_GrabPointViewList.clear();
}
