/**************************************************************************
 *	Polygon2dHyView.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "Polygon2dHyView.h"
#include "GrabPoint.h"

Polygon2dHyView::Polygon2dHyView(HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent)
{
	// NOTE: m_Outline does not have a parent because it is projected to window coordinates
	m_Outline.UseWindowCoordinates();
	m_Outline.SetWireframe(true);
	m_Outline.SetDisplayOrder(DISPLAYORDER_TransformCtrl - 1);
	//m_Outline.SetVisible(false);
}

/*virtual*/ Polygon2dHyView::~Polygon2dHyView()
{
	ClearPrimitives();
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
	m_Outline.SetTint(bIsDark ? HyColor::White : HyColor::Black);
}

/*virtual*/ void Polygon2dHyView::RefreshView(bool bTransformPreview) /*override*/
{
	if(m_pModel == nullptr)
	{
		ClearPrimitives();
		m_Outline.SetAsNothing();
		SetVertexGrabPointListSize(0);
		return;
	}

	RefreshColor();

	if(m_pModel->GetType() == SHAPE_LineChain)
	{
		if(m_PrimList.size() != 1)
		{
			ClearPrimitives();
			HyPrimitive2d *pNewPrim = new HyPrimitive2d(this);
			m_PrimList.push_back(pNewPrim);
		}

		m_PrimList[0]->SetAsLineChain(static_cast<HyChain2d *>(m_pModel->GetFixture(0))->GetChainData());
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
	}

	RefreshOutline();
}

/*virtual*/ void Polygon2dHyView::OnHoverClear() /*override*/
{

}

// Using 'floatList' (which are stored in world coordinates) construct the 'm_Outline' by first converting points to camera space
// Also update 'm_VertexGrabPointList' with the converted to camera space points
void Polygon2dHyView::RefreshOutline()
{
	// TODO: Use model's grabpoints instead of hardcoding it here

	QList<float> floatList = m_pModel->GetData();
	HyCamera2d *pCamera = HyEngine::Window().GetCamera2d(0);
	switch(m_pModel->GetType())
	{
	case SHAPE_None:
		SetVertexGrabPointListSize(0);
		m_Outline.SetAsNothing();
		break;

	case SHAPE_Polygon:
		grabpoints;
		break;

	case SHAPE_Box: {
		if(floatList.size() & 1)
			HyGuiLog("ShapeCtrl::RefreshOutline was a box/polygon with an odd number of serialized floats", LOGTYPE_Error);
		SetVertexGrabPointListSize(floatList.size() / 2);

		std::vector<glm::vec2> vertList;
		int iGrabPtCountIndex = 0;
		for(int i = 0; i < floatList.size(); i += 2, iGrabPtCountIndex++)
		{
			glm::vec2 ptCameraPos(floatList[i], floatList[i + 1]);
			pCamera->ProjectToCamera(ptCameraPos, ptCameraPos);
			vertList.push_back(ptCameraPos);

			m_VertexGrabPointList[iGrabPtCountIndex]->pos.Set(ptCameraPos);
		}

		m_Outline.SetAsPolygon(vertList);
		break; }

	case SHAPE_Circle: {
		SetVertexGrabPointListSize(5);

		glm::vec2 ptCenter(floatList[0], floatList[1]);
		pCamera->ProjectToCamera(ptCenter, ptCenter);
		float fRadius = floatList[2] * pCamera->GetZoom();

		m_VertexGrabPointList[0]->pos.Set(ptCenter);
		m_VertexGrabPointList[1]->pos.Set(ptCenter + glm::vec2(fRadius, 0.0f));
		m_VertexGrabPointList[2]->pos.Set(ptCenter + glm::vec2(0.0f, fRadius));
		m_VertexGrabPointList[3]->pos.Set(ptCenter + glm::vec2(-fRadius, 0.0f));
		m_VertexGrabPointList[4]->pos.Set(ptCenter + glm::vec2(0.0f, -fRadius));

		m_Outline.SetAsCircle(ptCenter, fRadius);
		break; }

	case SHAPE_LineSegment: {
		SetVertexGrabPointListSize(2);

		glm::vec2 ptOne(floatList[0], floatList[1]);
		pCamera->ProjectToCamera(ptOne, ptOne);
		glm::vec2 ptTwo(floatList[2], floatList[3]);
		pCamera->ProjectToCamera(ptTwo, ptTwo);

		m_VertexGrabPointList[0]->pos.Set(ptOne);
		m_VertexGrabPointList[1]->pos.Set(ptTwo);

		m_Outline.SetAsLineSegment(ptOne, ptTwo);
		break; }

	case SHAPE_Capsule: {
		SetVertexGrabPointListSize(3);

		glm::vec2 ptOne(floatList[0], floatList[1]);
		pCamera->ProjectToCamera(ptOne, ptOne);
		glm::vec2 ptTwo(floatList[2], floatList[3]);
		pCamera->ProjectToCamera(ptTwo, ptTwo);

		float fRadius = floatList[4] * pCamera->GetZoom();

		m_VertexGrabPointList[0]->pos.Set(ptOne);
		m_VertexGrabPointList[1]->pos.Set(ptTwo);
		m_VertexGrabPointList[2]->pos.Set(ptTwo + (glm::normalize(ptTwo - ptOne) * fRadius));

		m_Outline.SetAsLineSegment(ptOne, ptTwo);
		break; }

	case SHAPE_LineChain: {
		if(floatList.size() & 1) asdf;
			HyGuiLog("ShapeCtrl::RefreshOutline was a LineChain/LineLoop with an odd number of serialized floats", LOGTYPE_Error);
		SetVertexGrabPointListSize(floatList.size() / 2);

		std::vector<glm::vec2> vertList;
		int iGrabPtCountIndex = 0;
		for(int i = 0; i < floatList.size(); i += 2, iGrabPtCountIndex++)
		{
			glm::vec2 ptCameraPos(floatList[i], floatList[i + 1]);
			pCamera->ProjectToCamera(ptCameraPos, ptCameraPos);
			vertList.push_back(ptCameraPos);

			m_VertexGrabPointList[iGrabPtCountIndex]->pos.Set(ptCameraPos);
		}

		m_Outline.SetAsLineChain(vertList, false);// m_eShape == SHAPE_LineLoop);
		break; }
	}

	for(GrabPoint *pGrabPt : m_VertexGrabPointList)
		pGrabPt->SetVisible(true);
}

void Polygon2dHyView::SetVertexGrabPointListSize(uint32 uiNumGrabPoints)
{
	while(static_cast<uint32>(m_VertexGrabPointList.size()) > uiNumGrabPoints)
	{
		delete m_VertexGrabPointList.back();
		m_VertexGrabPointList.pop_back();
	}

	while(static_cast<uint32>(m_VertexGrabPointList.size()) < uiNumGrabPoints)
	{
		GrabPoint *pNewGrabPt = new GrabPoint(HyGlobal::GetEditorColor(EDITORCOLOR_ShapeGrabPointOutline),
											  HyGlobal::GetEditorColor(EDITORCOLOR_ShapeGrabPointFill),
											  HyGlobal::GetEditorColor(EDITORCOLOR_ShapeGrabPointSelectedOutline),
											  HyGlobal::GetEditorColor(EDITORCOLOR_ShapeGrabPointSelectedFill),
											  this);
		pNewGrabPt->SetDisplayOrder(DISPLAYORDER_TransformCtrl);
		m_VertexGrabPointList.push_back(pNewGrabPt);
	}
}

void Polygon2dHyView::ClearPrimitives()
{
	for(HyPrimitive2d *pPrim : m_PrimList)
		delete pPrim;
	m_PrimList.clear();
}
