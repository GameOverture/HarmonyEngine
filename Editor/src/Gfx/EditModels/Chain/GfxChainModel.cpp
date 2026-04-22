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
	IGfxEditModel(EDITMODETYPE_Chain, color)
{
	QJsonObject serializedObj;
	serializedObj.insert("type", HYLINECHAIN_Name);

	QJsonArray dataArray;
	for(float f : floatList)
		dataArray.append(f);
	serializedObj.insert("data", dataArray);

	serializedObj.insert("outline", 1.0f);

	m_FixtureList.append(new HyChain2d());

	Deserialize(serializedObj);
}

/*virtual*/ GfxChainModel::~GfxChainModel()
{
	delete m_FixtureList[0];
}

HyChain2d *GfxChainModel::GetChain()
{
	return static_cast<HyChain2d *>(m_FixtureList[0]);
}

const HyChain2d *GfxChainModel::GetChain() const
{
	return static_cast<HyChain2d *>(m_FixtureList[0]);
}

/*virtual*/ QJsonObject GfxChainModel::Serialize() const /*override*/
{
	QJsonObject serializedObj;
	serializedObj.insert("type", HYLINECHAIN_Name);

	std::vector<float> serializedData = GetChain()->SerializeSelf();
	QJsonArray dataArray;
	for(float f : serializedData)
		dataArray.append(f);
	serializedObj.insert("data", dataArray);

	serializedObj.insert("outline", 1.0f);

	return serializedObj;
}

void GfxChainModel::TransformData(glm::mat4 mtxTransform)
{
	GetChain()->TransformSelf(mtxTransform);
	SyncViews(EDITMODE_Idle, EDITMODEACTION_None);
}

bool GfxChainModel::IsLoopClosed() const
{
	return GetChain()->GetChainData().bLoop;
}

/*virtual*/ QString GfxChainModel::GetActionText(QString sNodeCodeName) const /*override*/
{
	QString sUndoText;
	switch(m_eCurAction)
	{
	case EDITMODEACTION_None:
	case EDITMODEACTION_Outside:
		break;
	case EDITMODEACTION_Creation:
		sUndoText = "Create new chain fixture " % sNodeCodeName;
		break;
	case EDITMODEACTION_Inside:
	case EDITMODEACTION_HoverCenter:
		sUndoText = "Translate chain " % sNodeCodeName;
		break;
	case EDITMODEACTION_AppendVertex:
		sUndoText = "Append vertex on " % sNodeCodeName;
		break;
	case EDITMODEACTION_InsertVertex:
		sUndoText = "Insert vertex on " % sNodeCodeName;
		break;
	case EDITMODEACTION_HoverGrabPoint:
		sUndoText = "Translate vert(s) on " % sNodeCodeName;
		break;

	default:
		HyGuiLog("GfxChainModel::MouseTransformReleased - Invalid m_eCurTransform", LOGTYPE_Error);
		break;
	}

	return sUndoText;
}

/*virtual*/ QString GfxChainModel::DoDeserialize(const QJsonObject &serializedObj) /*override*/
{

}

/*virtual*/ EditModeAction GfxChainModel::DoMouseMoveIdle() /*override*/
{
	m_iGrabPointIndex = -1;
	m_ptGrabPointPos = glm::vec2(0.0f, 0.0f);

	for(int i = 0; i < m_GrabPointList.size(); ++i)
	{
		if(m_GrabPointList[i].IsMouseHover())
		{
			m_iGrabPointIndex = i;
			return EDITMODEACTION_HoverGrabPoint;
		}
	}
	if(m_GrabPointCenter.IsMouseHover())
		return EDITMODEACTION_HoverCenter;

	if(m_GrabPointList.empty())
		return EDITMODEACTION_Creation;

	if(CheckIfAddVertexOnEdge())
		return EDITMODEACTION_InsertVertex;

	if(IsLoopClosed() == false)
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
				if(HyEngine::Input().GetWorldMousePos(m_ptGrabPointPos) == false)
					HyGuiLog("GfxChainModel::MouseMoveIdle - Failed to get world mouse pos for append vertex", LOGTYPE_Error);

				return EDITMODEACTION_AppendVertex;
			}
			else if(m_GrabPointList.back().IsSelected())
			{
				m_iGrabPointIndex = m_GrabPointList.size();
				if(HyEngine::Input().GetWorldMousePos(m_ptGrabPointPos) == false)
					HyGuiLog("GfxChainModel::MouseMoveIdle - Failed to get world mouse pos for append vertex", LOGTYPE_Error);
				
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
	m_GrabPointList[1].Set(GRABPOINT_EndpointSelected, ptDragPos);
}

bool GfxChainModel::CheckIfAddVertexOnEdge()
{
	if(m_GrabPointList.size() < 2)
		return false;

	HyShape2d tmpEdgeShape;
	for(int i = 0; i < m_GrabPointList.size(); ++i)
	{
		if(i == (m_GrabPointList.size() - 1) && IsLoopClosed() == false)
			break;

		// TODO: Test whether to convert all this to camera space
		glm::vec2 pt1 = m_GrabPointList[i].GetPos();
		glm::vec2 pt2 = m_GrabPointList[(i + 1) % m_GrabPointList.size()].GetPos();

		glm::vec2 ptWorldMousePos;
		if(HyEngine::Input().GetWorldMousePos(ptWorldMousePos) == false)
		{
			HyGuiLog("GfxShapeModel::CheckIfAddVertexOnEdge - Failed to get world mouse position for hit testing", LOGTYPE_Error);
			return false;
		}

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
