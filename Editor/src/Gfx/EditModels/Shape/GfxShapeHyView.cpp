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
#include "GfxShapeModel.h"
#include "GfxGrabPointView.h"

GfxShapeHyView::GfxShapeHyView(bool bIsFixture, HyEntity2d *pParent /*= nullptr*/) :
	IGfxEditView(pParent),
	m_bIsFixture(bIsFixture)
{
	// NOTE: m_PrimOutline does not have a parent because it is projected to window coordinates
	m_PrimOutline.UseWindowCoordinates();
	m_PrimOutline.SetWireframe(true);
	m_PrimOutline.SetDisplayOrder(DISPLAYORDER_TransformCtrl - 1);
}

/*virtual*/ GfxShapeHyView::~GfxShapeHyView()
{
	ClearPrimitives();
	ClearPreviewPrimitives();
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

	if(m_bIsFixture)
	{
		m_PrimOutline.SetTint(m_pModel->GetColor());
		m_PrimOutline.SetLineThickness(2.0f);
		for(HyPrimitive2d *pPrim : m_PrimList)
			pPrim->alpha.Set(0.25f);
	}
	else
	{
		m_PrimOutline.SetTint(bIsDark ? HyColor::White : HyColor::Black);
		m_PrimOutline.SetLineThickness(1.0f);

		for(HyPrimitive2d *pPrim : m_PrimList)
			pPrim->alpha.Set(1.0f);
	}
}

/*virtual*/ void GfxShapeHyView::DoRefreshView(EditModeState eEditModeState, ShapeMouseMoveResult eResult) /*override*/
{
	if(m_pModel == nullptr || static_cast<GfxShapeModel *>(m_pModel)->GetShapeType() == SHAPE_None)
	{
		ClearPrimitives();
		m_PrimOutline.SetAsNothing();
		ClearPreviewPrimitives();
		return;
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Sync Primitives with Model
	HyCamera2d *pCamera = HyEngine::Window().GetCamera2d(0);
	
	if(static_cast<GfxShapeModel *>(m_pModel)->GetShapeType() != SHAPE_Polygon)
	{
		if(m_PrimList.size() != 1)
		{
			ClearPrimitives();
			HyPrimitive2d *pNewPrim = new HyPrimitive2d(this);
			m_PrimList.push_back(pNewPrim);
		}
		m_PrimList[0]->SetAsShape(*static_cast<GfxShapeModel *>(m_pModel)->GetShapeFixture(0));

		if(m_PrimList[0]->GetShapeType() != HYFIXTURE_Nothing)
		{
			// Set `m_PrimOutline`
			if(static_cast<GfxShapeModel *>(m_pModel)->GetShapeType() == SHAPE_Box)
			{
				b2Vec2 *pVerts = static_cast<GfxShapeModel *>(m_pModel)->GetShapeFixture(0)->GetAsPolygon().vertices;
				std::vector<glm::vec2> projectedVertList;
				for(int i = 0; i < 4; ++i)
				{
					glm::vec2 ptScreenPos;
					pCamera->ProjectToCamera(glm::vec2(pVerts[i].x, pVerts[i].y), ptScreenPos);
					projectedVertList.push_back(ptScreenPos);
				}

				m_PrimOutline.SetAsPolygon(projectedVertList);
			}
			else if(static_cast<GfxShapeModel *>(m_pModel)->GetShapeType() == SHAPE_Circle)
			{
				b2Circle circle = static_cast<GfxShapeModel *>(m_pModel)->GetShapeFixture(0)->GetAsCircle();

				glm::vec2 ptCenter(circle.center.x, circle.center.y);
				pCamera->ProjectToCamera(ptCenter, ptCenter);
				float fRadius = circle.radius * pCamera->GetZoom();

				m_PrimOutline.SetAsCircle(ptCenter, fRadius);
			}
			else if(static_cast<GfxShapeModel *>(m_pModel)->GetShapeType() == SHAPE_LineSegment)
			{
				b2Segment seg = static_cast<GfxShapeModel *>(m_pModel)->GetShapeFixture(0)->GetAsSegment();
				glm::vec2 ptOne(seg.point1.x, seg.point1.y);
				pCamera->ProjectToCamera(ptOne, ptOne);
				glm::vec2 ptTwo(seg.point2.x, seg.point2.y);
				pCamera->ProjectToCamera(ptTwo, ptTwo);

				m_PrimOutline.SetAsLineSegment(ptOne, ptTwo);
			}
			else if(static_cast<GfxShapeModel *>(m_pModel)->GetShapeType() == SHAPE_Capsule)
			{
				b2Capsule capsule = static_cast<GfxShapeModel *>(m_pModel)->GetShapeFixture(0)->GetAsCapsule();
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
		else
			m_PrimOutline.SetAsNothing();
	}
	else // SHAPE_Polygon
	{
		int iNumFixtures = static_cast<GfxShapeModel *>(m_pModel)->GetNumShapeFixtures();
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
			m_PrimList[iIndex]->SetAsShape(*static_cast<GfxShapeModel *>(m_pModel)->GetShapeFixture(iIndex));

		// Set `m_PrimOutline`
		const QList<GfxGrabPointModel> &grabPointModelList = m_pModel->GetGrabPointList();
		std::vector<glm::vec2> projectedVertList;
		for(int i = 0; i < grabPointModelList.size(); ++i)
		{
			glm::vec2 ptScreenPos;
			pCamera->ProjectToCamera(grabPointModelList[i].GetPos(), ptScreenPos);
			projectedVertList.push_back(ptScreenPos);
		}
		m_PrimOutline.SetAsLineChain(projectedVertList, static_cast<GfxShapeModel *>(m_pModel)->IsLoopClosed());
	}

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
		if(static_cast<GfxShapeModel *>(m_pModel)->GetShapeType() != SHAPE_Polygon)
		{
			HyGuiLog("GfxShapeHyView::RefreshView - SHAPEMOUSEMOVE_AppendVertex - called with non-polygon/linechain shape type", LOGTYPE_Error);
			break;
		}
		if(static_cast<GfxShapeModel *>(m_pModel)->IsLoopClosed() || grabPointModelList.empty())
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
		if(static_cast<GfxShapeModel *>(m_pModel)->GetShapeType() != SHAPE_Polygon)
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

	case SHAPEMOUSEMOVE_HoverGrabPoint:
		if(eEditModeState == EDITMODE_MouseDownTransform)
			DoHoverGrabPoint(eEditModeState);
		break;

	case SHAPEMOUSEMOVE_HoverCenter:
		if(eEditModeState == EDITMODE_MouseDownTransform)
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

void GfxShapeHyView::ClearPreviewPrimitives()
{
	for(HyPrimitive2d *pPrim : m_PrimPreviewList)
		delete pPrim;
	m_PrimPreviewList.clear();
}

void GfxShapeHyView::DoHoverGrabPoint(EditModeState eEditModeState)
{
	const QList<GfxGrabPointModel> &grabPointModelList = m_pModel->GetGrabPointList();
	glm::mat4 mtxTransform(1.0f);
	int iVertexIndex = -1;
	m_pModel->GetTransformPreview(mtxTransform, iVertexIndex);
	glm::vec4 vTranslate = mtxTransform[3];

	if(iVertexIndex < 0 || iVertexIndex >= grabPointModelList.size())
	{
		HyGuiLog("GfxShapeHyView::DoHoverGrabPoint - invalid m_iVertexIndex", LOGTYPE_Error);
		return;
	}
	if(m_pModel->IsHoverGrabPointSelected() == false)
		HyGuiLog("GfxShapeHyView::DoHoverGrabPoint - Hover vertex not selected on box transform", LOGTYPE_Error);

	// Apply grab point drag logic based on shape type
	switch(static_cast<GfxShapeModel *>(m_pModel)->GetShapeType())
	{
	case SHAPE_Box: // Lock vertices together to keep box form
		if(m_pModel->GetNumGrabPointsSelected() == 1)
		{
			glm::vec2 ptVertPos = grabPointModelList[iVertexIndex].GetPos();

			// Find the opposite/locked vertex by iterating over all vertices and finding the one that is farthest from iVertexIndex
			int iLockedVertIndex = -1;
			float fMaxDistance = -1.0f;
			for(int i = 0; i < grabPointModelList.size(); ++i)
			{
				float fDistance = glm::distance(ptVertPos, grabPointModelList[i].GetPos());
				if(fDistance > fMaxDistance)
				{
					fMaxDistance = fDistance;
					iLockedVertIndex = i;
				}
			}

			// Translate the selected vertex
			m_GrabPointViewList[iVertexIndex]->pos.Set(ptVertPos + glm::vec2(vTranslate.x, vTranslate.y));

			// Determine the lower/upper bounds based on the selected and locked vertices
			glm::vec2 ptSelectedVert = m_GrabPointViewList[iVertexIndex]->pos.Get();
			glm::vec2 ptLockedVert = m_GrabPointViewList[iLockedVertIndex]->pos.Get();
			glm::vec2 ptLowerBound, ptUpperBound;
			HySetVec(ptLowerBound, ptLockedVert.x < ptSelectedVert.x ? ptLockedVert.x : ptSelectedVert.x, ptLockedVert.y < ptSelectedVert.y ? ptLockedVert.y : ptSelectedVert.y);
			HySetVec(ptUpperBound, ptLockedVert.x >= ptSelectedVert.x ? ptLockedVert.x : ptSelectedVert.x, ptLockedVert.y >= ptSelectedVert.y ? ptLockedVert.y : ptSelectedVert.y);

			// Update the other 2 vertices that aren't the selected or locked
			for(int i = 0; i < grabPointModelList.size(); ++i)
			{
				if(i == iVertexIndex || i == iLockedVertIndex)
					continue;

				glm::vec2 ptCurrentVert = m_GrabPointViewList[i]->pos.Get();
				if(ptCurrentVert.x == ptLockedVert.x)
					ptCurrentVert.x = ptUpperBound.x;
				else
					ptCurrentVert.x = ptLowerBound.x;
				if(ptCurrentVert.y == ptLockedVert.y)
					ptCurrentVert.y = ptUpperBound.y;
				else
					ptCurrentVert.y = ptLowerBound.y;
				m_GrabPointViewList[i]->pos.Set(ptCurrentVert);
			}
		}
		else // TODO: Better control when 2 verts selected
		{
			
		}
		break;
	case SHAPE_Circle:
		break;
	case SHAPE_LineSegment:
		break;
	case SHAPE_Polygon:
		break;
	case SHAPE_Capsule:
		break;

	default:
		HyGuiLog("GfxShapeHyView::DoHoverGrabPoint - Unsupported shape type for grab point transform: " % QString::number(static_cast<GfxShapeModel *>(m_pModel)->GetShapeType()), LOGTYPE_Error);
		break;
	}
}
