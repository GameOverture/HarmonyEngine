/**************************************************************************
*	GfxChainModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "GfxChainModel.h"
#include "IGfxEditView.h"

GfxChainModel::GfxChainModel(HyColor color, const QList<float> &floatList /*= QList<float>()*/) :
	IGfxEditModel(EDITMODETYPE_Chain, color),
	m_bSelfIntersecting(false),
	m_ptSelfIntersection(0.0f, 0.0f),
	m_bReverseWindingOrder(false),
	m_bLoopClosed(false)
{
	Deserialize(floatList);
}

/*virtual*/ GfxChainModel::~GfxChainModel()
{
}

/*virtual*/ bool GfxChainModel::IsValidModel() const /*override*/
{
	if(m_Chain.GetChainData().iCount < 4)
		return false;

	return m_bSelfIntersecting == false;
}

/*virtual*/ QList<float> GfxChainModel::Serialize() const /*override*/
{
	std::vector<float> serializedData = m_Chain.SerializeSelf();
	QList<float> returnList(serializedData.begin(), serializedData.end());
	return returnList;
}

void GfxChainModel::TransformData(glm::mat4 mtxTransform)
{
	m_Chain.TransformSelf(mtxTransform);
	SyncViews(EDITMODE_Idle, EDITMODEACTION_None);
}

const HyChain2d &GfxChainModel::GetChainFixture() const
{
	return m_Chain;
}

bool GfxChainModel::IsLoopClosed() const
{
	return m_bLoopClosed;
}

/*virtual*/ QString GfxChainModel::MouseTransformReleased(QString sShapeCodeName, QPointF ptWorldMousePos) /*override*/
{
	QString sUndoText;
	switch(m_eCurAction)
	{
	case EDITMODEACTION_None:
	case EDITMODEACTION_Outside:
		break;
	case EDITMODEACTION_Creation:
		sUndoText = "Create new chain fixture " % sShapeCodeName;
		break;
	case EDITMODEACTION_Inside:
	case EDITMODEACTION_HoverCenter:
		sUndoText = "Translate chain " % sShapeCodeName;
		break;
	case EDITMODEACTION_AppendVertex:
		sUndoText = "Append vertex on " % sShapeCodeName;
		break;
	case EDITMODEACTION_InsertVertex:
		sUndoText = "Insert vertex on " % sShapeCodeName;
		break;
	case EDITMODEACTION_HoverGrabPoint:
		sUndoText = "Translate vert(s) on " % sShapeCodeName;
		break;

	default:
		HyGuiLog("GfxChainModel::MouseTransformReleased - Invalid m_eCurTransform", LOGTYPE_Error);
		break;
	}

	return sUndoText;
}

/*virtual*/ void GfxChainModel::DoDeserialize(const QList<float> &floatList) /*override*/
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Assemble `m_GrabPointList`
	std::vector<glm::vec2> grabPointList;
	if(floatList.empty() == false)
	{
		grabPointList = m_Chain.DeserializeSelf(HYFIXTURE_LineChain, std::vector<float>(floatList.begin(), floatList.end()));
		
		// Find center point
		glm::vec2 ptCentroid;
		m_Chain.GetCentroid(ptCentroid);
		m_GrabPointCenter.Setup(ptCentroid);
	}

	// Preserve existing grab points where possible (keeps selection)
	for(int i = 0; i < grabPointList.size(); ++i)
	{
		if(static_cast<int>(m_GrabPointList.size()) - 1 < i)
			m_GrabPointList.push_back(GfxGrabPointModel(GRABPOINT_Vertex, grabPointList[i]));
		else
			m_GrabPointList[i].Setup(m_GrabPointList[i].IsSelected() ? GRABPOINT_VertexSelected : GRABPOINT_Vertex, grabPointList[i]);
	}
	if(static_cast<int>(m_GrabPointList.size()) > static_cast<int>(grabPointList.size())) // Truncate to new size
		m_GrabPointList.resize(grabPointList.size());

	if(m_bLoopClosed == false && m_GrabPointList.size() > 1)
	{
		m_GrabPointList.front().Setup(m_GrabPointList.front().IsSelected() ? GRABPOINT_EndpointSelected : GRABPOINT_Endpoint);
		m_GrabPointList.back().Setup(m_GrabPointList.back().IsSelected() ? GRABPOINT_EndpointSelected : GRABPOINT_Endpoint);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	if(m_GrabPointList.empty())
		m_Chain.ClearData();
}

/*virtual*/ EditModeAction GfxChainModel::DoMouseMoveIdle(glm::vec2 ptWorldMousePos) /*override*/
{
	m_iGrabPointIndex = -1;
	m_ptGrabPointPos = glm::vec2(0.0f, 0.0f);

	for(int i = 0; i < m_GrabPointList.size(); ++i)
	{
		if(m_GrabPointList[i].TestPoint(ptWorldMousePos))
		{
			m_iGrabPointIndex = i;
			return EDITMODEACTION_HoverGrabPoint;
		}
	}
	if(m_GrabPointCenter.TestPoint(ptWorldMousePos))
		return EDITMODEACTION_HoverCenter;

	if(m_GrabPointList.empty())
		return EDITMODEACTION_Creation;

	if(CheckIfAddVertexOnEdge(ptWorldMousePos))
		return EDITMODEACTION_InsertVertex;

	if(m_bLoopClosed == false)
	{
		// If only the first or last vertex in chain is selected, return EDITMODEACTION_AppendVertex to indicate appending verts to the selected end
		int iNumSelected = 0;
		for(const GfxGrabPointModel &grabPtModel : m_GrabPointList)
		{
			if(grabPtModel.IsSelected())
				++iNumSelected;
		}
		if(iNumSelected == 1)
		{
			if(m_GrabPointList.front().IsSelected())
			{
				m_iGrabPointIndex = 0;
				m_ptGrabPointPos = ptWorldMousePos;
				return EDITMODEACTION_AppendVertex;
			}
			else if(m_GrabPointList.back().IsSelected())
			{
				m_iGrabPointIndex = m_GrabPointList.size();
				m_ptGrabPointPos = ptWorldMousePos;
				return EDITMODEACTION_AppendVertex;
			}
		}
	}

	return EDITMODEACTION_Outside;
}

void GfxChainModel::DoTransformCreation(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos)
{
	glm::vec2 ptLowerBound, ptUpperBound, ptCenter;
	if(bShiftMod)
	{
		ptCenter = ptStartPos;

		glm::vec2 vRadius = ptCenter - ptDragPos;
		vRadius.x = abs(vRadius.x);
		vRadius.y = abs(vRadius.y);
		ptUpperBound = (ptCenter + vRadius);
		ptLowerBound = (ptCenter + (vRadius * -1.0f));
	}
	else
	{
		HySetVec(ptLowerBound, ptStartPos.x < ptDragPos.x ? ptStartPos.x : ptDragPos.x, ptStartPos.y < ptDragPos.y ? ptStartPos.y : ptDragPos.y);
		HySetVec(ptUpperBound, ptStartPos.x >= ptDragPos.x ? ptStartPos.x : ptDragPos.x, ptStartPos.y >= ptDragPos.y ? ptStartPos.y : ptDragPos.y);
		ptCenter = ptLowerBound + ((ptUpperBound - ptLowerBound) * 0.5f);
	}

	if(m_GrabPointList.size() == 0)
		m_GrabPointList.append(GfxGrabPointModel(GRABPOINT_Endpoint, ptDragPos));
	if(m_GrabPointList.size() == 1)
		m_GrabPointList.append(GfxGrabPointModel(GRABPOINT_Endpoint, ptDragPos));
	if(m_GrabPointList.size() != 2)
		HyGuiLog("GfxChainModel::MouseTransformDrag - Polygon or LineChain initial dragging with != 2 verts", LOGTYPE_Error);

	m_GrabPointList[0].SetSelected(false);
	m_GrabPointList[1].Setup(GRABPOINT_EndpointSelected, ptDragPos);
}

bool GfxChainModel::CheckIfAddVertexOnEdge(glm::vec2 ptWorldMousePos)
{
	if(m_GrabPointList.size() < 2)
		return false;

	HyShape2d tmpEdgeShape;
	for(int i = 0; i < m_GrabPointList.size(); ++i)
	{
		if(i == (m_GrabPointList.size() - 1) && m_bLoopClosed == false)
			break;

		glm::vec2 pt1 = m_GrabPointList[i].GetPos();
		glm::vec2 pt2 = m_GrabPointList[(i + 1) % m_GrabPointList.size()].GetPos();
		tmpEdgeShape.SetAsLineSegment(pt1, pt2);
		if(tmpEdgeShape.TestPoint(ptWorldMousePos, glm::identity<glm::mat4>()))
		{
			m_iGrabPointIndex = i + 1;
			m_ptGrabPointPos = HyMath::ClosestPointOnSegment(pt1, pt2, ptWorldMousePos);
			return true;
		}
	}

	return false;
}

bool GfxChainModel::IsShareEdge(const std::vector<glm::vec2> &a, const std::vector<glm::vec2> &b, int &a0, int &a1, int &b0, int &b1)
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
