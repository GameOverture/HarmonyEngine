/**************************************************************************
*	IGfxEditModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "IGfxEditModel.h"
#include "IGfxEditView.h"
#include "MainWindow.h"

IGfxEditModel::IGfxEditModel() :
	m_bIsLineChain(false),
	m_eShapeType(SHAPE_None),
	m_fOutline(0.0f),
	m_GrabPointCenter(GRABPOINT_Center),
	m_bSelfIntersecting(false),
	m_ptSelfIntersection(0.0f, 0.0f),
	m_bLoopClosed(false),
	m_sMalformedReason("No Data"),
	m_eCurAction(EDITMODEACTION_None),
	m_vDragDelta(0.0f, 0.0f),
	m_iGrabPointIndex(-1),
	m_ptGrabPointPos(0.0f, 0.0f)
{
	ClearAction();
}

/*virtual*/ IGfxEditModel::~IGfxEditModel()
{
	ClearFixtures();
}

bool IGfxEditModel::IsValidModel() const
{
	return m_sMalformedReason.isEmpty();
}

QJsonObject IGfxEditModel::Serialize() const
{
	QJsonObject serializedObj;
	if(m_bIsLineChain)
		serializedObj.insert("type", HYLINECHAIN_Name);
	else
		serializedObj.insert("type", HyGlobal::ShapeName(m_eShapeType));

	std::vector<float> dataList = SerializeData();
	QJsonArray dataArray;
	for(float f : dataList)
		dataArray.push_back(f);
	serializedObj.insert("data", dataArray);

	serializedObj.insert("outline", m_fOutline);

	return serializedObj;
}

void IGfxEditModel::Deserialize(const QJsonObject &serializedObj)
{
	m_sMalformedReason = DeserializeData(serializedObj);
	SyncViews(EDITMODE_Idle, EDITMODEACTION_None);
}

void IGfxEditModel::AddView(IGfxEditView *pView)
{
	if(m_ViewList.contains(pView))
		return;
	m_ViewList.push_back(pView);
}

bool IGfxEditModel::RemoveView(IGfxEditView *pView)
{
	return m_ViewList.removeOne(pView);
}

void IGfxEditModel::SyncViews(EditModeState eEditModeState, EditModeAction eResult) const
{
	for(IGfxEditView *pView : m_ViewList)
		pView->SyncWithModel(eEditModeState, eResult);
}

int IGfxEditModel::GetNumFixtures() const
{
	return m_FixtureList.size();
}

const QList<GfxGrabPointModel> &IGfxEditModel::GetGrabPointList() const
{
	return m_GrabPointList;
}

const GfxGrabPointModel &IGfxEditModel::GetGrabPoint(int iIndex) const
{
	if(iIndex < 0 || iIndex >= m_GrabPointList.size())
	{
		HyGuiLog("IGfxEditModel::GetGrabPoint - Index out of range", LOGTYPE_Error);
		return m_GrabPointCenter;
	}
	return m_GrabPointList[iIndex];
}

const GfxGrabPointModel *IGfxEditModel::GetActiveGrabPoint() const
{
	if(m_iGrabPointIndex < 0 || m_iGrabPointIndex >= m_GrabPointList.size())
		return nullptr;

	return &m_GrabPointList[m_iGrabPointIndex];
}

const GfxGrabPointModel &IGfxEditModel::GetCenterGrabPoint() const
{
	return m_GrabPointCenter;
}

int IGfxEditModel::GetActiveGrabPointIndex() const
{
	return m_iGrabPointIndex;
}

int IGfxEditModel::GetNumGrabPointsSelected() const
{
	int iNumSelected = 0;
	for(const GfxGrabPointModel &grabPtModel : m_GrabPointList)
	{
		if(grabPtModel.IsSelected())
			++iNumSelected;
	}
	return iNumSelected;
}

bool IGfxEditModel::IsAllGrabPointsSelected() const
{
	for(const GfxGrabPointModel &grabPtModel : m_GrabPointList)
	{
		if(grabPtModel.IsSelected() == false)
			return false;
	}
	return true;
}

bool IGfxEditModel::IsHoverGrabPointSelected() const
{
	if(m_iGrabPointIndex < 0 || m_iGrabPointIndex >= m_GrabPointList.size())
		return false;

	return m_GrabPointList[m_iGrabPointIndex].IsSelected();
}

void IGfxEditModel::DeselectAllGrabPoints()
{
	for(GfxGrabPointModel &grabPtModel : m_GrabPointList)
		grabPtModel.SetSelected(false);
}

Qt::CursorShape IGfxEditModel::MouseMoveIdle()
{
	EditModeAction eResult = DoMouseMoveIdle();

	switch(eResult)
	{
	case EDITMODEACTION_Creation:
		MainWindow::SetStatus("Edit Mode - Creation", 0);
		return Qt::CrossCursor;

	case EDITMODEACTION_Outside:
		MainWindow::SetStatus("Edit Mode", 0);
		return Qt::ArrowCursor;

	case EDITMODEACTION_Inside:
		if(IsAllGrabPointsSelected())
		{
			MainWindow::SetStatus("Edit Mode - Translate ", 0);
			return Qt::SizeAllCursor;
		}
		else
		{
			MainWindow::SetStatus("Edit Mode - Mouse Over", 0);
			return Qt::ArrowCursor;
		}

	case EDITMODEACTION_AppendVertex:
		MainWindow::SetStatus("Edit Mode - Append Vertex", 0);
		return Qt::CrossCursor;

	case EDITMODEACTION_InsertVertex:
		MainWindow::SetStatus("Edit Mode - Insert Vertex", 0);
		return Qt::CrossCursor;

	case EDITMODEACTION_HoverGrabPoint:
		if(IsHoverGrabPointSelected())
		{
			MainWindow::SetStatus("Edit Mode - Translate Vertex", 0);
			return Qt::SizeAllCursor;
		}
		else if(m_iGrabPointIndex >= 0)
		{
			MainWindow::SetStatus("Edit Mode - Select Vertex", 0);
			return m_GrabPointList[m_iGrabPointIndex].GetHoverCursor();
		}

	case EDITMODEACTION_HoverCenter:
		MainWindow::SetStatus("Edit Mode - Translate", 0);
		return Qt::SizeAllCursor;

	default:
		HyGuiLog("IGfxEditModel::MouseMoveIdle - unsupported edit mode action!", LOGTYPE_Error);
		break;
	}

	MainWindow::SetStatus("Edit Mode", 0);
	return Qt::ArrowCursor;
}

bool IGfxEditModel::MousePressEvent(EditModeState eEditModeState, bool bShiftHeld, Qt::MouseButtons uiButtonFlags)
{
	bool bStartTransform = false;

	EditModeAction eResult = DoMouseMoveIdle();
	if(eResult == EDITMODEACTION_AppendVertex || eResult == EDITMODEACTION_InsertVertex)
	{
		if(m_iGrabPointIndex == -1)
		{
			HyGuiLog("GfxShapeModel::MousePressEvent - Insert vertex index was -1 on AddVertex/Crosshair", LOGTYPE_Error);
			return EDITMODEACTION_None;
		}
		m_GrabPointList.insert(m_GrabPointList.begin() + m_iGrabPointIndex, GfxGrabPointModel(GRABPOINT_VertexSelected, m_ptGrabPointPos));
		for(int i = 0; i < m_GrabPointList.size(); ++i)
			m_GrabPointList[i].SetSelected(false);
		m_GrabPointList[m_iGrabPointIndex].SetSelected(true);

		bStartTransform = true;
	}
	else if(eResult == EDITMODEACTION_HoverGrabPoint)
	{
		if(m_iGrabPointIndex == -1)
		{
			HyGuiLog("GfxShapeModel::OnMousePressEvent - Hover vertex index was -1 on HoverVertex/HoverSelectedVertex", LOGTYPE_Error);
			return EDITMODEACTION_None;
		}

		if(bShiftHeld)
		{
			if(uiButtonFlags & Qt::LeftButton)
				m_GrabPointList[m_iGrabPointIndex].SetSelected(!m_GrabPointList[m_iGrabPointIndex].IsSelected());
			else if(uiButtonFlags & Qt::RightButton)
				m_GrabPointList[m_iGrabPointIndex].SetSelected(false);
		}
		else
		{
			if(uiButtonFlags & Qt::LeftButton)
			{
				for(int i = 0; i < m_GrabPointList.size(); ++i)
					m_GrabPointList[i].SetSelected(false);
				m_GrabPointList[m_iGrabPointIndex].SetSelected(true);
			}
			else if(uiButtonFlags & Qt::RightButton)
				m_GrabPointList[m_iGrabPointIndex].SetSelected(false);
		}

		bStartTransform = true;
	}
	else if(eResult == EDITMODEACTION_Creation)
		bStartTransform = true;
	else if(eResult == EDITMODEACTION_Inside)
		bStartTransform = IsAllGrabPointsSelected();

	m_eCurAction = eResult;

	return bStartTransform;
}

void IGfxEditModel::MouseMarqueeReleased(EditModeState eEditModeState, bool bLeftClick, QPointF ptBotLeft, QPointF ptTopRight)
{
	// Select grab points within marquee
	for(GfxGrabPointModel &grabPtModel : m_GrabPointList)
	{
		grabPtModel.SetSelected(false);

		QPointF ptGrabPos(grabPtModel.GetPos().x, grabPtModel.GetPos().y);
		if(ptGrabPos.x() >= ptBotLeft.x() && ptGrabPos.x() <= ptTopRight.x() &&
		   ptGrabPos.y() >= ptBotLeft.y() && ptGrabPos.y() <= ptTopRight.y())
		{
			if(bLeftClick)
				grabPtModel.SetSelected(true);
		}
	}

	SyncViews(eEditModeState, EDITMODEACTION_None);
}

void IGfxEditModel::MouseTransform(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos)
{
	HySetVec(m_vDragDelta, 0.0f, 0.0f);
	if(false == (HyCompareFloat(ptStartPos.x, ptDragPos.y) && HyCompareFloat(ptStartPos.y, ptDragPos.y)))
		HySetVec(m_vDragDelta, ptDragPos.x - ptStartPos.x, ptDragPos.y - ptStartPos.y);

	if(m_eCurAction == EDITMODEACTION_Creation)
		DoTransformCreation(bShiftMod, ptStartPos, ptDragPos);

	if(m_eCurAction == EDITMODEACTION_AppendVertex)
		m_GrabPointList[m_iGrabPointIndex].SetPos(ptDragPos);

	SyncViews(EDITMODE_MouseDragTransform, m_eCurAction);
}

glm::vec2 IGfxEditModel::GetDragDelta() const
{
	return m_vDragDelta;
}

void IGfxEditModel::ClearAction()
{
	m_eCurAction = EDITMODEACTION_None;
	m_vDragDelta = glm::vec2(0.0f, 0.0f);
	m_iGrabPointIndex = -1;
	m_ptGrabPointPos = glm::vec2(0.0f, 0.0f);
}

void IGfxEditModel::TransformData(glm::mat4 mtxTransform)
{
	for(IHyFixture2d *pFixture : m_FixtureList)
		pFixture->TransformSelf(mtxTransform);

	SyncViews(EDITMODE_Idle, EDITMODEACTION_None);
}

void IGfxEditModel::ClearFixtures()
{
	for(IHyFixture2d *pFixture : m_FixtureList)
		delete pFixture;
	m_FixtureList.clear();
}

std::vector<float> IGfxEditModel::SerializeData() const
{
	if(m_FixtureList.empty())
		return std::vector<float>();
	if(m_eShapeType != SHAPE_Polygon)
		return m_FixtureList[0]->SerializeSelf();

	// SHAPE_Polygon
	std::vector<float> returnList;
	for(const GfxGrabPointModel &grabPt : m_GrabPointList)
	{
		glm::vec2 ptVertex = grabPt.GetPos();
		returnList.push_back(static_cast<float>(ptVertex.x));
		returnList.push_back(static_cast<float>(ptVertex.y));
	}
	returnList.push_back(m_bLoopClosed ? 1.0f : 0.0f); // Final float indicates whether loop is closed
	return returnList;
}

QString IGfxEditModel::DeserializeData(const QJsonObject &serializedObj)
{
	m_bSelfIntersecting = false;
	HySetVec(m_ptSelfIntersection, 0.0f, 0.0f);

	QString sType = serializedObj["type"].toString();
	if(sType == HYLINECHAIN_Name)
	{
		m_bIsLineChain = true;
		m_eShapeType = SHAPE_None;
	}
	else
	{
		m_bIsLineChain = false;
		m_eShapeType = HyGlobal::GetShapeFromString(sType);
	}

	QJsonArray dataArray = serializedObj["data"].toArray();
	std::vector<float> floatList;
	for(const QJsonValue &val : dataArray)
		floatList.push_back(static_cast<float>(val.toDouble()));

	m_fOutline = static_cast<float>(serializedObj["outline"].toDouble());

	if(floatList.empty())
	{
		ClearFixtures();
		m_GrabPointList.clear();
		return "No data provided";
	}

	HyFixtureType eFixtureType = HYFIXTURE_Nothing;
	if(m_bIsLineChain)
	{
		if(m_FixtureList.size() > 1)
			ClearFixtures();
		if(m_FixtureList.empty())
			m_FixtureList.push_back(new HyChain2d());

		eFixtureType = HYFIXTURE_LineChain;
	}
	else
	{
		if(m_FixtureList.size() == 1 && m_FixtureList[0]->GetType() == HYFIXTURE_LineChain)
			ClearFixtures();
		if(m_FixtureList.empty())
			m_FixtureList.push_back(new HyShape2d());
		
		eFixtureType = HyGlobal::ConvertShapeToFixtureType(m_eShapeType);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Acquire `m_GrabPointList` and `m_GrabPointCenter`
	std::vector<glm::vec2> grabPointList;
	glm::vec2 ptCentroid;

	if(m_bIsLineChain == false && m_eShapeType == SHAPE_Polygon)
	{
		// SHAPE_Polygon deserialization
		if((floatList.size() & 1) == 0)
			HyGuiLog("GfxShapeModel::SetData for polygon had an even number of floats (final, odd float indcates loop)", LOGTYPE_Error);
		else
		{
			int iNumVertFloats = floatList.size() - 1;
			grabPointList.reserve(iNumVertFloats / 2);
			for(int i = 0; i < iNumVertFloats; i += 2)
				grabPointList.emplace_back(glm::vec2(floatList[i], floatList[i + 1]));

			m_bLoopClosed = floatList.back() != 0.0f;
		}

		ptCentroid = HyGlobal::CalculateCentroid(grabPointList);
	}
	else
	{
		QString sResult = m_FixtureList[0]->DeserializeSelf(eFixtureType, floatList).c_str();
		if(sResult.isEmpty() == false)
			return sResult;

		grabPointList = m_FixtureList[0]->CalcGrabPoints();
		m_FixtureList[0]->GetCentroid(ptCentroid);
	}

	if(grabPointList.empty())
	{
		m_GrabPointList.clear();
		return "Failed to calculate grab points";
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Assemble `m_GrabPointList` and `m_GrabPointCenter`
	m_GrabPointCenter.SetPos(ptCentroid);

	if(m_bIsLineChain || m_eShapeType == SHAPE_Polygon)
	{
		// Preserve existing grab points where possible (keeps selection)
		for(int i = 0; i < grabPointList.size(); ++i)
		{
			if(static_cast<int>(m_GrabPointList.size()) - 1 < i)
				m_GrabPointList.push_back(GfxGrabPointModel(GRABPOINT_Vertex, grabPointList[i]));
			else
				m_GrabPointList[i].Set(m_GrabPointList[i].IsSelected() ? GRABPOINT_VertexSelected : GRABPOINT_Vertex, grabPointList[i]);
		}
		if(static_cast<int>(m_GrabPointList.size()) > static_cast<int>(grabPointList.size())) // Truncate to new size
			m_GrabPointList.resize(grabPointList.size());

		if(m_bLoopClosed == false && m_GrabPointList.size() > 1)
		{
			m_GrabPointList.front().SetType(m_GrabPointList.front().IsSelected() ? GRABPOINT_EndpointSelected : GRABPOINT_Endpoint);
			m_GrabPointList.back().SetType(m_GrabPointList.back().IsSelected() ? GRABPOINT_EndpointSelected : GRABPOINT_Endpoint);
		}
	}
	else
	{
		switch(m_eShapeType)
		{
		case SHAPE_None:
			m_GrabPointList.clear();
			break;

		case SHAPE_Box:
			if(grabPointList.size() != 4)
				return "Invalid box shape data";

			m_GrabPointList.resize(4);
			for(int i = 0; i < 4; ++i)
				m_GrabPointList[i].Set(GRABPOINT_ShapeCtrlAll, grabPointList[i]);
			break;

		case SHAPE_Circle:
			if(grabPointList.size() != 4)
				return "Invalid circle shape data";

			m_GrabPointList.resize(4);
			m_GrabPointList[0].Set(GRABPOINT_ShapeCtrlVert, grabPointList[0]); // Top
			m_GrabPointList[1].Set(GRABPOINT_ShapeCtrlHorz, grabPointList[1]); // Right
			m_GrabPointList[2].Set(GRABPOINT_ShapeCtrlVert, grabPointList[2]); // Bottom
			m_GrabPointList[3].Set(GRABPOINT_ShapeCtrlHorz, grabPointList[3]); // Left
			break;

		case SHAPE_LineSegment:
			if(grabPointList.size() != 2)
				return "Invalid line segment shape data";

			m_GrabPointList.resize(2);
			m_GrabPointList[0].Set(GRABPOINT_ShapeCtrlAll, grabPointList[0]);
			m_GrabPointList[1].Set(GRABPOINT_ShapeCtrlAll, grabPointList[1]);
			break;

		case SHAPE_Polygon:
			HyGuiLog("IGfxEditModel::DeserializeData - Polygon shape type should have been handled by line chain deserialization!", LOGTYPE_Error);
			break;

		case SHAPE_Capsule:
			if(grabPointList.size() != 6)
				return "Invalid capsule shape data";

			m_GrabPointList.resize(6);
			m_GrabPointList[0].Set(GRABPOINT_ShapeCtrlVert, grabPointList[0]); // Pt1
			m_GrabPointList[1].Set(GRABPOINT_ShapeCtrlVert, grabPointList[1]); // Pt2
			m_GrabPointList[2].Set(GRABPOINT_ShapeCtrlHorz, grabPointList[2]); // Center1 Right
			m_GrabPointList[3].Set(GRABPOINT_ShapeCtrlHorz, grabPointList[3]); // Center1 Left
			m_GrabPointList[4].Set(GRABPOINT_ShapeCtrlHorz, grabPointList[4]); // Center2 Right
			m_GrabPointList[5].Set(GRABPOINT_ShapeCtrlHorz, grabPointList[5]); // Center2 Left
			break;
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	if(m_GrabPointList.empty() && (m_bIsLineChain == true || m_eShapeType != SHAPE_None))
		return "Grab points not provided";

	if(m_bIsLineChain || m_eShapeType == SHAPE_Polygon)
	{
		std::vector<glm::vec2> vertexList;
		vertexList.reserve(m_GrabPointList.size());
		for(const GfxGrabPointModel &grabPt : m_GrabPointList)
			vertexList.emplace_back(grabPt.GetPos());

		// Validate no self-intersections
		glm::vec2 pt1, pt2, pt3, pt4;
		for(int i = 0; i < vertexList.size(); ++i)
		{
			const glm::vec2 &pt1 = vertexList[i];
			const glm::vec2 &pt2 = vertexList[(i + 1) % vertexList.size()];
			for(int j = i + 2; j < vertexList.size(); ++j)
			{
				// Don't check adjacent edges
				if(j == (i + 1) % vertexList.size() || (i == 0 && j == vertexList.size() - 1))
					continue;

				const glm::vec2 &pt3 = vertexList[j];
				const glm::vec2 &pt4 = vertexList[(j + 1) % vertexList.size()];
				m_bSelfIntersecting = HyMath::TestSegmentsOverlap(pt1, pt2, pt3, pt4, m_ptSelfIntersection);
				if(m_bSelfIntersecting)
					return "Has self-intersecting edges";
			}
		}

		if(m_eShapeType == SHAPE_Polygon) // Assemble `m_FixtureList` with valid sub-polygons (convex and <= 8 vertices)
		{
			// Ensure counter-clockwise winding on `vertexList`
			float fArea = 0.0f;
			for(int i = 0; i < vertexList.size(); ++i)
			{
				const glm::vec2 &ptCurrent = vertexList[i];
				const glm::vec2 &ptNext = vertexList[(i + 1) % m_GrabPointList.size()];
				fArea += (ptNext.x - ptCurrent.x) * (ptNext.y + ptCurrent.y);
			}
			if(fArea > 0.0f) // Clockwise winding, needs reverse
			{
				std::reverse(vertexList.begin(), vertexList.end());
				std::reverse(m_GrabPointList.begin(), m_GrabPointList.end());
			}

			// Decompose into convex polygons with <= 8 vertices
			if(vertexList.size() <= 8 && HyMath::IsConvexPolygon(vertexList))
				AssemblePolygonFixtures({ vertexList });
			else
				AssemblePolygonFixtures(MergeTriangles(HyMath::Triangulate(vertexList)));
		}
	}

	return QString();
}

void IGfxEditModel::AssemblePolygonFixtures(std::vector<std::vector<glm::vec2>> subPolygonList)
{
	ClearFixtures();
	for(std::vector<glm::vec2> &subPoly : subPolygonList)
	{
		HyShape2d *pShape2d = new HyShape2d();
		pShape2d->SetAsPolygon(subPoly);
		m_FixtureList.push_back(pShape2d);
	}
}

std::vector<glm::vec2> IGfxEditModel::MergePolygons(const std::vector<glm::vec2> &ptA, const std::vector<glm::vec2> &ptB, int a0, int a1, int b0, int b1)
{
	std::vector<glm::vec2> out;

	// ptA: from a1 -> a0
	for(int i = a1; i != a0; i = (i + 1) % ptA.size())
		out.push_back(ptA[i]);
	out.push_back(ptA[a0]);

	// ptB: from b1 -> b0
	for(int i = b1; i != b0; i = (i + 1) % ptB.size())
		out.push_back(ptB[i]);
	out.push_back(ptB[b0]);

	return out;
}

std::vector<std::vector<glm::vec2>> IGfxEditModel::MergeTriangles(const std::vector<HyTriangle2d> &triangleList)
{
	std::vector<std::vector<glm::vec2>> polys;
	for(const HyTriangle2d &t : triangleList)
		polys.push_back({ t.m_ptA, t.m_ptB, t.m_ptC });

	bool bMerged;
	do
	{
		bMerged = false;

		for(size_t i = 0; i < polys.size() && !bMerged; ++i)
		{
			for(size_t j = i + 1; j < polys.size(); ++j)
			{
				int a0, a1, b0, b1;
				if(!IsShareEdge(polys[i], polys[j], a0, a1, b0, b1))
					continue;

				std::vector<glm::vec2> mergedPoly = MergePolygons(polys[i], polys[j], a0, a1, b0, b1);
				if(mergedPoly.size() <= 8 && HyMath::IsConvexPolygon(mergedPoly))
				{
					polys[i] = mergedPoly;
					polys.erase(polys.begin() + j);
					bMerged = true;
					break;
				}
			}
		}
	} while(bMerged);

	return polys;
}

bool IGfxEditModel::IsShareEdge(const std::vector<glm::vec2> &a, const std::vector<glm::vec2> &b, int &a0, int &a1, int &b0, int &b1)
{
	for(int i = 0; i < (int)a.size(); ++i)
	{
		int ni = (i + 1) % a.size();
		for(int j = 0; j < (int)b.size(); ++j)
		{
			int nj = (j + 1) % b.size();

			if(a[i] == b[nj] && a[ni] == b[j])
			{
				a0 = i; a1 = ni;
				b0 = j; b1 = nj;
				return true;
			}
		}
	}
	return false;
}
