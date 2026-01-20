/**************************************************************************
 *	GfxMarqueeCtrl.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "GfxMarqueeCtrl.h"

GfxMarqueeCtrl::GfxMarqueeCtrl(HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_bIsActive(false),
	m_BoundingVolume(this)
{
	m_Outline.UseWindowCoordinates();
	m_Outline.SetWireframe(true);
	m_Outline.SetDisplayOrder(DISPLAYORDER_TransformCtrl - 1);

	HyColor color = HyGlobal::GetEditorColor(EDITORCOLOR_Marquee);
	m_BoundingVolume.SetTint(color);
	m_Outline.SetTint(color.IsDark() ? color.Lighten() : color.Darken());
}

/*virtual*/ GfxMarqueeCtrl::~GfxMarqueeCtrl()
{
}

void GfxMarqueeCtrl::SetAsDrag(glm::vec2 ptStartPos, glm::vec2 ptDragPos)
{
	if(m_bIsActive == false)
	{
		m_BoundingVolume.alpha.Set(1.0f);
		m_BoundingVolume.alpha.Tween(0.25f, 0.5f);
		m_bIsActive = true;
	}

	glm::vec2 ptLowerBound, ptUpperBound, ptCenter;
	HySetVec(ptLowerBound, ptStartPos.x < ptDragPos.x ? ptStartPos.x : ptDragPos.x, ptStartPos.y < ptDragPos.y ? ptStartPos.y : ptDragPos.y);
	HySetVec(ptUpperBound, ptStartPos.x >= ptDragPos.x ? ptStartPos.x : ptDragPos.x, ptStartPos.y >= ptDragPos.y ? ptStartPos.y : ptDragPos.y);
	ptCenter = ptLowerBound + ((ptUpperBound - ptLowerBound) * 0.5f);

	HyCamera2d *pCamera = HyEngine::Window().GetCamera2d(0);
	glm::vec2 ptWindowLowerBound, ptWindowUpperBound, ptWindowCenter;
	pCamera->ProjectToCamera(ptLowerBound, ptWindowLowerBound);
	pCamera->ProjectToCamera(ptUpperBound, ptWindowUpperBound);
	ptWindowCenter = ptWindowLowerBound + ((ptWindowUpperBound - ptWindowLowerBound) * 0.5f);

	m_BoundingVolume.SetAsBox(HyRect((ptUpperBound.x - ptLowerBound.x) * 0.5f, (ptUpperBound.y - ptLowerBound.y) * 0.5f, ptCenter, 0.0f));
	m_Outline.SetAsBox(HyRect((ptWindowUpperBound.x - ptWindowLowerBound.x) * 0.5f, (ptWindowUpperBound.y - ptWindowLowerBound.y) * 0.5f, ptWindowCenter, 0.0f));

	SetVisible(true);
}

b2AABB GfxMarqueeCtrl::GetSelection()
{
	HyShape2d tmpShape;
	m_BoundingVolume.CalcLocalBoundingShape(tmpShape);

	b2AABB marqueeAabb;
	tmpShape.ComputeAABB(marqueeAabb, glm::mat4(1.0f));
	return marqueeAabb;
}

void GfxMarqueeCtrl::Hide()
{
	m_BoundingVolume.SetAsNothing();
	m_Outline.SetAsNothing();

	m_bIsActive = false;
}

//QString ShapeCtrl::Serialize()
//{
//	HyShape2d shape;
//	m_BoundingVolume.CalcLocalBoundingShape(shape);
//
//	if(shape.IsValid() == false)
//		return "";
//
//	QList<float> floatList;
//	switch(m_eShape)
//	{
//	case SHAPE_None:
//		break;
//
//	case SHAPE_Polygon:
//	case SHAPE_Box:
//		for(int i = 0; i < shape.GetAsPolygon().count; ++i)
//		{
//			floatList.push_back(shape.GetAsPolygon().vertices[i].x);
//			floatList.push_back(shape.GetAsPolygon().vertices[i].y);
//		}
//		break;
//
//	case SHAPE_Circle:
//		floatList.push_back(shape.GetAsCircle().center.x);
//		floatList.push_back(shape.GetAsCircle().center.y);
//		floatList.push_back(shape.GetAsCircle().radius);
//		break;
//
//	case SHAPE_LineSegment:
//		floatList.push_back(shape.GetAsSegment().point1.x);
//		floatList.push_back(shape.GetAsSegment().point1.y);
//		floatList.push_back(shape.GetAsSegment().point2.x);
//		floatList.push_back(shape.GetAsSegment().point2.y);
//		break;
//
//	case SHAPE_Capsule:
//		floatList.push_back(shape.GetAsCapsule().center1.x);
//		floatList.push_back(shape.GetAsCapsule().center1.y);
//		floatList.push_back(shape.GetAsCapsule().center2.x);
//		floatList.push_back(shape.GetAsCapsule().center2.y);
//		floatList.push_back(shape.GetAsCapsule().radius);
//		break;
//
//	case SHAPE_LineChain: {
//		const HyChainData *pChainData = m_BoundingVolume.GetChainData();
//		if(pChainData == nullptr)
//			break;
//
//		for(int i = 0; i < pChainData->iCount; ++i)
//		{
//			floatList.push_back(pChainData->pPointList[i].x);
//			floatList.push_back(pChainData->pPointList[i].y);
//		}
//		break; }
//	}
//
//	QString sSerializedData;
//	for(float f : floatList)
//	{
//		sSerializedData.append(QString::number(f));
//		sSerializedData.append(',');
//	}
//	sSerializedData.chop(1);
//
//	return sSerializedData;
//}
//
//void ShapeCtrl::Deserialize(QString sData, HyCamera2d *pCamera)
//{
//	if(sData.isEmpty())
//		return;
//
//	QStringList sFloatList = sData.split(',', Qt::SkipEmptyParts);
//
//	m_DeserializedFloatList.clear();
//	for(QString sFloat : sFloatList)
//	{
//		bool bOk;
//		float fValue = sFloat.toFloat(&bOk);
//		if(bOk)
//			m_DeserializedFloatList.push_back(fValue);
//	}
//
//	switch(m_eShape)
//	{
//	case SHAPE_None:
//		break;
//
//	case SHAPE_Polygon:
//	case SHAPE_Box: {
//		std::vector<glm::vec2> vertList;
//		for(int i = 0; i < m_DeserializedFloatList.size(); i += 2)
//			vertList.push_back(glm::vec2(m_DeserializedFloatList[i], m_DeserializedFloatList[i + 1]));
//
//		m_BoundingVolume.SetAsPolygon(vertList);
//		break; }
//
//	case SHAPE_Circle: {
//		glm::vec2 ptCenter(m_DeserializedFloatList[0], m_DeserializedFloatList[1]);
//		float fRadius = m_DeserializedFloatList[2];
//		m_BoundingVolume.SetAsCircle(ptCenter, fRadius);
//		break; }
//
//	case SHAPE_LineSegment: {
//		glm::vec2 ptOne(m_DeserializedFloatList[0], m_DeserializedFloatList[1]);
//		glm::vec2 ptTwo(m_DeserializedFloatList[2], m_DeserializedFloatList[3]);
//		m_BoundingVolume.SetAsLineSegment(ptOne, ptTwo);
//		break; }
//
//	case SHAPE_Capsule: {
//		glm::vec2 pt1(m_DeserializedFloatList[0], m_DeserializedFloatList[1]);
//		glm::vec2 pt2(m_DeserializedFloatList[2], m_DeserializedFloatList[3]);
//		float fRadius = m_DeserializedFloatList[4];
//		m_BoundingVolume.SetAsCapsule(pt1, pt2, fRadius);
//		break; }
//
//	case SHAPE_LineChain: {
//		std::vector<glm::vec2> vertList;
//		for(int i = 0; i < m_DeserializedFloatList.size(); i += 2)
//			vertList.push_back(glm::vec2(m_DeserializedFloatList[i], m_DeserializedFloatList[i + 1]));
//
//		m_BoundingVolume.SetAsLineChain(vertList, false);// m_eShape == SHAPE_LineLoop);
//		break; }
//	}
//
//	DeserializeOutline(pCamera);
//}
//
//// NOTE: Does not update m_Outline, requires a DeserializeOutline()
//void ShapeCtrl::TransformSelf(glm::mat4 mtxTransform)
//{
//	HyShape2d shape;
//	m_BoundingVolume.CalcLocalBoundingShape(shape);
//	shape.TransformSelf(mtxTransform);
//	m_BoundingVolume.SetAsShape(shape);
//}
//
//void ShapeCtrl::DeserializeOutline(HyCamera2d *pCamera)
//{
//	if(m_DeserializedFloatList.empty() || pCamera == nullptr)
//		return;
//
//	// Using 'm_DeserializedFloatList' (which are stored in world coordinates) construct the 'm_Outline' by first converting points to camera space
//	// Also update 'm_VertexGrabPointList' with the converted to camera space points
//	switch(m_eShape)
//	{
//	case SHAPE_None:
//		break;
//
//	case SHAPE_Polygon:
//	case SHAPE_Box: {
//		if(m_DeserializedFloatList.size() & 1)
//			HyGuiLog("ShapeCtrl::RefreshOutline was a box/polygon with an odd number of serialized floats", LOGTYPE_Error);
//		SetVertexGrabPointListSize(m_DeserializedFloatList.size() / 2);
//
//		std::vector<glm::vec2> vertList;
//		int iGrabPtCountIndex = 0;
//		for(int i = 0; i < m_DeserializedFloatList.size(); i += 2, iGrabPtCountIndex++)
//		{
//			glm::vec2 ptCameraPos(m_DeserializedFloatList[i], m_DeserializedFloatList[i + 1]);
//			pCamera->ProjectToCamera(ptCameraPos, ptCameraPos);
//			vertList.push_back(ptCameraPos);
//
//			m_VertexGrabPointList[iGrabPtCountIndex]->pos.Set(ptCameraPos);
//		}
//
//		m_Outline.SetAsPolygon(vertList);
//		break; }
//
//	case SHAPE_Circle: {
//		SetVertexGrabPointListSize(5);
//
//		glm::vec2 ptCenter(m_DeserializedFloatList[0], m_DeserializedFloatList[1]);
//		pCamera->ProjectToCamera(ptCenter, ptCenter);
//		float fRadius = m_DeserializedFloatList[2] * pCamera->GetZoom();
//
//		m_VertexGrabPointList[0]->pos.Set(ptCenter);
//		m_VertexGrabPointList[1]->pos.Set(ptCenter + glm::vec2(fRadius, 0.0f));
//		m_VertexGrabPointList[2]->pos.Set(ptCenter + glm::vec2(0.0f, fRadius));
//		m_VertexGrabPointList[3]->pos.Set(ptCenter + glm::vec2(-fRadius, 0.0f));
//		m_VertexGrabPointList[4]->pos.Set(ptCenter + glm::vec2(0.0f, -fRadius));
//
//		m_Outline.SetAsCircle(ptCenter, fRadius);
//		break; }
//
//	case SHAPE_LineSegment: {
//		SetVertexGrabPointListSize(2);
//
//		glm::vec2 ptOne(m_DeserializedFloatList[0], m_DeserializedFloatList[1]);
//		pCamera->ProjectToCamera(ptOne, ptOne);
//		glm::vec2 ptTwo(m_DeserializedFloatList[2], m_DeserializedFloatList[3]);
//		pCamera->ProjectToCamera(ptTwo, ptTwo);
//
//		m_VertexGrabPointList[0]->pos.Set(ptOne);
//		m_VertexGrabPointList[1]->pos.Set(ptTwo);
//
//		m_Outline.SetAsLineSegment(ptOne, ptTwo);
//		break; }
//
//	case SHAPE_Capsule: {
//		SetVertexGrabPointListSize(3);
//
//		glm::vec2 ptOne(m_DeserializedFloatList[0], m_DeserializedFloatList[1]);
//		pCamera->ProjectToCamera(ptOne, ptOne);
//		glm::vec2 ptTwo(m_DeserializedFloatList[2], m_DeserializedFloatList[3]);
//		pCamera->ProjectToCamera(ptTwo, ptTwo);
//
//		float fRadius = m_DeserializedFloatList[4] * pCamera->GetZoom();
//
//		m_VertexGrabPointList[0]->pos.Set(ptOne);
//		m_VertexGrabPointList[1]->pos.Set(ptTwo);
//		m_VertexGrabPointList[2]->pos.Set(ptTwo + (glm::normalize(ptTwo - ptOne) * fRadius));
//
//		m_Outline.SetAsLineSegment(ptOne, ptTwo);
//		break; }
//
//	case SHAPE_LineChain: {
//		if(m_DeserializedFloatList.size() & 1)
//			HyGuiLog("ShapeCtrl::RefreshOutline was a LineChain/LineLoop with an odd number of serialized floats", LOGTYPE_Error);
//		SetVertexGrabPointListSize(m_DeserializedFloatList.size() / 2);
//
//		std::vector<glm::vec2> vertList;
//		int iGrabPtCountIndex = 0;
//		for(int i = 0; i < m_DeserializedFloatList.size(); i += 2, iGrabPtCountIndex++)
//		{
//			glm::vec2 ptCameraPos(m_DeserializedFloatList[i], m_DeserializedFloatList[i + 1]);
//			pCamera->ProjectToCamera(ptCameraPos, ptCameraPos);
//			vertList.push_back(ptCameraPos);
//
//			m_VertexGrabPointList[iGrabPtCountIndex]->pos.Set(ptCameraPos);
//		}
//
//		m_Outline.SetAsLineChain(vertList, false);// m_eShape == SHAPE_LineLoop);
//		break; }
//	}
//
//	if(IsVemEnabled())
//	{
//		for(GrabPoint *pGrabPt : m_VertexGrabPointList)
//			pGrabPt->SetVisible(true);
//	}
//}
//
//bool ShapeCtrl::IsVemEnabled() const
//{
//	return m_bIsVem;
//}
//
//void ShapeCtrl::EnableVertexEditMode()
//{
//	m_bIsVem = true;
//
//	for(GrabPoint *pGrabPt : m_VertexGrabPointList)
//		pGrabPt->SetVisible(true);
//
//	UnselectAllVemVerts();
//}
//
//SemState ShapeCtrl::GetMouseSemHoverAction(bool bCtrlMod, bool bShiftMod, bool bSelectVert)
//{
//	if(bCtrlMod && m_eShape != SHAPE_None && m_eShape != SHAPE_Box && m_eShape != SHAPE_Circle && m_eShape != SHAPE_LineSegment)
//	{
//		if(m_eShape == SHAPE_Polygon && m_VertexGrabPointList.count() >= B2_MAX_POLYGON_VERTICES)
//			return SEMSTATE_Invalid;
//		
//		return SEMSTATE_Add;
//	}
//
//	// Get selected grab points
//	QList<GrabPoint *> selectedGrabPtList;
//	for(GrabPoint *pGrabPt : m_VertexGrabPointList)
//	{
//		if(pGrabPt->IsSelected())
//			selectedGrabPtList.push_back(pGrabPt);
//	}
//
//	if(selectedGrabPtList.isEmpty() == false && m_eShape != SHAPE_Circle)
//	{
//		b2AABB selectedVertsArea;
//		HyMath::InvalidateAABB(selectedVertsArea);
//		for(GrabPoint *pSelectedPt : selectedGrabPtList)
//		{
//			if(b2IsValidAABB(selectedVertsArea) == false)
//				selectedVertsArea = pSelectedPt->GetSceneAABB();
//			else
//				selectedVertsArea = b2AABB_Union(selectedVertsArea, pSelectedPt->GetSceneAABB());
//		}
//
//		if(HyMath::TestPointAABB(selectedVertsArea, HyEngine::Input().GetMousePos()))
//			return SEMSTATE_Translate;
//	}
//
//	switch(m_eShape)
//	{
//	case SHAPE_None:
//		return SEMSTATE_None;
//
//	case SHAPE_Box:
//	case SHAPE_LineSegment:
//	case SHAPE_Polygon:
//	case SHAPE_Capsule:
//	case SHAPE_LineChain:
//		for(GrabPoint *pGrabPt : m_VertexGrabPointList)
//		{
//			if(pGrabPt->IsMouseHover())
//			{
//				if(bSelectVert)
//					pGrabPt->SetSelected(true);
//				return SEMSTATE_GrabPoint;
//			}
//		}
//		break;
//
//	case SHAPE_Circle:
//		for(int i = 0; i < m_VertexGrabPointList.size(); ++i)
//		{
//			if(m_VertexGrabPointList[i]->IsMouseHover())
//			{
//				if(bSelectVert)
//					m_VertexGrabPointList[i]->SetSelected(true);
//
//				if(i == 0)
//					return SEMSTATE_Translate;
//
//				return i & 1 ? SEMSTATE_RadiusHorizontal : SEMSTATE_RadiusVertical;
//			}
//		}
//		break;
//	}
//
//	return SEMSTATE_None;
//}
//
//void ShapeCtrl::SelectVemVerts(b2AABB selectionAabb, HyCamera2d *pCamera)
//{
//	for(GrabPoint *pGrabPt : m_VertexGrabPointList)
//	{
//		glm::vec2 ptPos = pGrabPt->pos.Get();
//		pCamera->ProjectToWorld(ptPos, ptPos);
//		if(HyMath::TestPointAABB(selectionAabb, ptPos))
//			pGrabPt->SetSelected(true);
//	}
//}
//
//void ShapeCtrl::TransformSemVerts(SemState eSemState, glm::vec2 ptStartPos, glm::vec2 ptDragPos, HyCamera2d *pCamera)
//{
//	// Calculate mouse drag (vTranslate) in camera coordinates
//	pCamera->ProjectToCamera(ptStartPos, ptStartPos);
//	pCamera->ProjectToCamera(ptDragPos, ptDragPos);
//	glm::vec2 vTranslate = ptDragPos - ptStartPos;
//
//	// Apply the transform based on the action
//	DeserializeOutline(pCamera);
//	switch(eSemState)
//	{
//	case SEMSTATE_Translate:
//	case SEMSTATE_GrabPoint: {
//		// Get currently selected grab points
//		QList<GrabPoint *> selectedGrabPtList;
//		for(GrabPoint *pGrabPt : m_VertexGrabPointList)
//		{
//			if(pGrabPt->IsSelected())
//				selectedGrabPtList.push_back(pGrabPt);
//		}
//
//		// Apply special cases to default functionality (Default: offset selected vertices by 'vTranslate')
//		if(m_eShape == SHAPE_Box)
//		{
//			// Box has special case to lock vertices together to keep box form
//			if(selectedGrabPtList.size() == 1)
//			{
//				// Find the opposite vertex by iterating over all vertices and finding the one that is furthest from selectedVert
//				GrabPoint *pOppositeVert = m_VertexGrabPointList[0];
//				float fMaxDistance = -1.0f;
//				for(GrabPoint *pVert : m_VertexGrabPointList)
//				{
//					float fDistance = glm::distance(selectedGrabPtList[0]->pos.Get(), pVert->pos.Get());
//					if(fDistance > fMaxDistance)
//					{
//						fMaxDistance = fDistance;
//						pOppositeVert = pVert;
//					}
//				}
//
//				// Translate the selected vertex
//				selectedGrabPtList[0]->pos.Offset(vTranslate);
//
//				// Update the other 2 vertices that aren't the selected or opposite
//				for(GrabPoint *pVert : m_VertexGrabPointList)
//				{
//					if(pVert == selectedGrabPtList[0] || pVert == pOppositeVert)
//						continue; // Skip the selected and opposite vertex
//
//					glm::vec2 ptNewPos = HyMath::ClosestPointOnRay(pOppositeVert->pos.Get(), glm::normalize(pVert->pos.Get() - pOppositeVert->pos.Get()), selectedGrabPtList[0]->pos.Get());
//					pVert->pos.Set(ptNewPos);
//				}
//
//				// Deselect all grab points because positions have been handled
//				selectedGrabPtList.clear();
//			}
//			else // TODO: Better control when 2 verts selected
//			{
//				// Select all verts
//				selectedGrabPtList.clear();
//				for(GrabPoint *pGrabPt : m_VertexGrabPointList)
//					selectedGrabPtList.push_back(pGrabPt);
//			}
//		}
//		else if(m_eShape == SHAPE_Circle)
//		{
//			// Select all verts
//			selectedGrabPtList.clear();
//			for(GrabPoint *pGrabPt : m_VertexGrabPointList)
//				selectedGrabPtList.push_back(pGrabPt);
//		}
//
//		// any points remaining in 'selectedGrabPtList' will be translated (as default functionality)
//		for(GrabPoint *pSelectedPt : selectedGrabPtList)
//			pSelectedPt->pos.Offset(vTranslate);
//
//		std::vector<glm::vec2> vertList;
//		for(GrabPoint *pGrabPt : m_VertexGrabPointList)
//			vertList.push_back(pGrabPt->pos.Get());
//
//		switch(m_eShape)
//		{
//		case SHAPE_Box:
//		case SHAPE_Polygon:
//			m_Outline.SetAsPolygon(vertList);
//			break;
//
//		case SHAPE_LineSegment:
//			m_Outline.SetAsLineSegment(vertList[0], vertList[1]);
//			break;
//
//		case SHAPE_Capsule: {
//			HyShape2d tmpShape;
//			m_Outline.CalcLocalBoundingShape(tmpShape);
//			m_Outline.SetAsCapsule(vertList[0], vertList[1], tmpShape.GetAsCapsule().radius);
//			break; }
//
//		case SHAPE_LineChain:
//			m_Outline.SetAsLineChain(vertList, false);// m_eShape == SHAPE_LineLoop);
//			break;
//
//		case SHAPE_Circle: {
//			HyShape2d tmpShape;
//			m_Outline.CalcLocalBoundingShape(tmpShape);
//			m_Outline.SetAsCircle(vertList[0], tmpShape.GetAsCircle().radius);
//			break; }
//
//		default:
//			HyGuiLog("ShapeCtrl::TransformVemVerts - Unhandled shape type", LOGTYPE_Error);
//			break;
//		}
//		break; }
//
//	//////////////////////////////////////////////////////////////////////////
//	case SEMSTATE_RadiusHorizontal:
//	case SEMSTATE_RadiusVertical:
//		m_Outline.SetAsCircle(m_VertexGrabPointList[0]->pos.Get(), glm::distance(m_VertexGrabPointList[0]->pos.Get(), ptDragPos));
//		break;
//
//	//////////////////////////////////////////////////////////////////////////
//	case SEMSTATE_Add: {
//		std::vector<glm::vec2> vertList;
//		for(GrabPoint *pGrabPt : m_VertexGrabPointList)
//			vertList.push_back(pGrabPt->pos.Get());
//
//		switch(m_eShape)
//		{
//		case SHAPE_Polygon: {
//			auto fpCompareDistance = [ptDragPos](const glm::vec2 &ptA, const glm::vec2 &ptB) {
//				return glm::distance(ptDragPos, ptA) < glm::distance(ptDragPos, ptB);
//			};
//
//			auto closestIter = std::min_element(vertList.begin(), vertList.end(), fpCompareDistance);
//			auto closestIndex = std::distance(vertList.begin(), closestIter);
//
//			auto nextIt = std::next(closestIter);
//			auto nextIndex = closestIndex + 1;
//
//			if(nextIt == vertList.end())
//				vertList.push_back(ptDragPos);
//			else
//			{
//				auto insertIt = vertList.begin() + nextIndex;
//				vertList.insert(insertIt, ptDragPos);
//			}
//
//			m_Outline.SetAsPolygon(vertList);
//			break; }
//
//		case SHAPE_LineChain: {
//			bool bPrepend = false;
//			bool bUseClosestEnd = true;
//
//			// Use selected verts to determine whether to prepend or append
//			for(int i = 0; i < m_VertexGrabPointList.size(); )
//			{
//				if(m_VertexGrabPointList[i]->IsSelected())
//				{
//					bUseClosestEnd = false;
//
//					if(i <= m_VertexGrabPointList.size() / 2)
//					{
//						bPrepend = true;
//						
//						if(i >= ((m_VertexGrabPointList.size() - 1) - i))
//							break;
//						else
//							i = ((m_VertexGrabPointList.size() - 1) - i);
//					}
//					else
//					{
//						bPrepend = false;
//						break;
//					}
//				}
//				else
//					++i;
//			}
//
//			if(bUseClosestEnd)
//			{
//				float fPrependDistance = glm::distance(ptDragPos, vertList.front());
//				float fAppendDistance = glm::distance(ptDragPos, vertList.back());
//				bPrepend = (fPrependDistance < fAppendDistance);
//			}
//
//			if(bPrepend)
//				vertList.insert(vertList.begin(), ptDragPos);
//			else
//				vertList.push_back(ptDragPos);
//
//			m_Outline.SetAsLineChain(vertList, false);// m_eShape == SHAPE_LineLoop);
//			break; }
//
//		case SHAPE_Box:
//		case SHAPE_Circle:
//		case SHAPE_LineSegment:
//		case SHAPE_Capsule:
//		default:
//			HyGuiLog("ShapeCtrl::TransformVemVerts - VEMACTION_Add invalid shape type", LOGTYPE_Error);
//		}
//		break; }
//
//	default:
//		HyGuiLog("ShapeCtrl::TransformVemVerts - Unhandled VEM ACTION", LOGTYPE_Error);
//		break;
//	}
//}
//
//bool ShapeCtrl::RemoveSelectedVerts()
//{
//	bool bHasSelection = false;
//	std::vector<glm::vec2> vertList;
//	for(GrabPoint *pGrabPt : m_VertexGrabPointList)
//	{
//		if(pGrabPt->IsSelected())
//			bHasSelection = true; // Skip over selected items
//		else
//			vertList.push_back(pGrabPt->pos.Get());
//	}
//	if(bHasSelection == false || vertList.size() <= 2)
//		return false;
//
//	if(m_eShape == SHAPE_Polygon)
//		m_Outline.SetAsPolygon(vertList);
//	else if(m_eShape == SHAPE_LineChain)
//		m_Outline.SetAsLineChain(vertList, false);// m_eShape == SHAPE_LineLoop);
//	else
//	{
//		HyGuiLog("ShapeCtrl::TransformVemVerts - VEMACTION_Add invalid shape type", LOGTYPE_Error);
//		return false;
//	}
//
//	return true;
//}
//
//QString ShapeCtrl::SerializeVemVerts(HyCamera2d *pCamera)
//{
//	HyShape2d shape;
//	m_Outline.CalcLocalBoundingShape(shape);
//
//	QList<float> floatList;
//	switch(m_eShape)
//	{
//	case SHAPE_None:
//		break;
//
//	case SHAPE_Polygon:
//	case SHAPE_Box:
//		for(int i = 0; i < shape.GetAsPolygon().count; ++i)
//		{
//			glm::vec2 ptVert(shape.GetAsPolygon().vertices[i].x, shape.GetAsPolygon().vertices[i].y);
//			pCamera->ProjectToWorld(ptVert, ptVert);
//
//			floatList.push_back(ptVert.x);
//			floatList.push_back(ptVert.y);
//		}
//		break;
//
//	case SHAPE_Circle: {
//		glm::vec2 ptCenter(shape.GetAsCircle().center.x, shape.GetAsCircle().center.y);
//		pCamera->ProjectToWorld(ptCenter, ptCenter);
//
//		floatList.push_back(ptCenter.x);
//		floatList.push_back(ptCenter.y);
//		floatList.push_back(shape.GetAsCircle().radius / pCamera->GetZoom());
//		break; }
//
//	case SHAPE_LineSegment: {
//		glm::vec2 ptVert1(shape.GetAsSegment().point1.x, shape.GetAsSegment().point1.y);
//		glm::vec2 ptVert2(shape.GetAsSegment().point2.x, shape.GetAsSegment().point2.y);
//		pCamera->ProjectToWorld(ptVert1, ptVert1);
//		pCamera->ProjectToWorld(ptVert2, ptVert2);
//
//		floatList.push_back(ptVert1.x);
//		floatList.push_back(ptVert1.y);
//		floatList.push_back(ptVert2.x);
//		floatList.push_back(ptVert2.y);
//		break; }
//
//	case SHAPE_Capsule: {
//		glm::vec2 ptVert1(shape.GetAsCapsule().center1.x, shape.GetAsCapsule().center1.y);
//		glm::vec2 ptVert2(shape.GetAsCapsule().center2.x, shape.GetAsCapsule().center2.y);
//		pCamera->ProjectToWorld(ptVert1, ptVert1);
//		pCamera->ProjectToWorld(ptVert2, ptVert2);
//
//		floatList.push_back(ptVert1.x);
//		floatList.push_back(ptVert1.y);
//		floatList.push_back(ptVert2.x);
//		floatList.push_back(ptVert2.y);
//		floatList.push_back(shape.GetAsCapsule().radius / pCamera->GetZoom());
//		break; }
//
//	case SHAPE_LineChain: {
//		const HyChainData *pChainData = m_Outline.GetChainData();
//		if(pChainData == nullptr)
//			break;
//
//		for(int i = 0; i < pChainData->iCount; ++i)
//		{
//			glm::vec2 ptVert(pChainData->pPointList[i]);
//			pCamera->ProjectToWorld(ptVert, ptVert);
//
//			floatList.push_back(ptVert.x);
//			floatList.push_back(ptVert.y);
//		}
//		break; }
//	}
//
//	QString sSerializedData;
//	for(float f : floatList)
//	{
//		sSerializedData.append(QString::number(f));
//		sSerializedData.append(',');
//	}
//	sSerializedData.chop(1);
//
//	return sSerializedData;
//}
//
//void ShapeCtrl::UnselectAllVemVerts()
//{
//	for(GrabPoint *pGrabPt : m_VertexGrabPointList)
//		pGrabPt->SetSelected(false);
//}
//
//void ShapeCtrl::ClearVertexEditMode()
//{
//	for(GrabPoint *pGrabPt : m_VertexGrabPointList)
//		pGrabPt->SetVisible(false);
//
//	m_bIsVem = false;
//}
//

//
//void ShapeCtrl::SetVertexGrabPointListSize(uint32 uiNumGrabPoints)
//{
//	while(static_cast<uint32>(m_VertexGrabPointList.size()) > uiNumGrabPoints)
//	{
//		delete m_VertexGrabPointList.back();
//		m_VertexGrabPointList.pop_back();
//	}
//
//	while(static_cast<uint32>(m_VertexGrabPointList.size()) < uiNumGrabPoints)
//	{
//		GrabPoint *pNewGrabPt = new GrabPoint(HyGlobal::GetEditorColor(EDITORCOLOR_ShapeGrabPointOutline),
//											  HyGlobal::GetEditorColor(EDITORCOLOR_ShapeGrabPointFill),
//											  HyGlobal::GetEditorColor(EDITORCOLOR_ShapeGrabPointSelectedOutline),
//											  HyGlobal::GetEditorColor(EDITORCOLOR_ShapeGrabPointSelectedFill),
//											  m_BoundingVolume.ParentGet());
//		pNewGrabPt->SetVisible(false);
//		pNewGrabPt->SetDisplayOrder(DISPLAYORDER_TransformCtrl);
//		m_VertexGrabPointList.push_back(pNewGrabPt);
//	}
//}
//
////void ShapeCtrl::ConvertTo(EditorShape eShape)
////{
////	HyGuiLog("ShapeCtrl::ConvertTo - Not implemented", LOGTYPE_Error);
////}
