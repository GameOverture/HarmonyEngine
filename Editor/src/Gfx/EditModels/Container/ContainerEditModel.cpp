/**************************************************************************
*	ContainerEditModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "ContainerEditModel.h"
#include "IGfxEditView.h"

ContainerEditModel::ContainerEditModel() :
	IGfxEditModel(EDITMODETYPE_Container, HyGlobal::GetEditorColor(EDITORCOLOR_Widgets))
{
}

/*virtual*/ ContainerEditModel::~ContainerEditModel()
{
}

/*virtual*/ bool ContainerEditModel::IsValidModel() const /*override*/
{
	return m_Box.IsValid();
}

/*virtual*/ QList<float> ContainerEditModel::Serialize() const /*override*/
{
	std::vector<float> serializedData = m_Box.SerializeSelf();
	return QList<float>(serializedData.begin(), serializedData.end());
}

/*virtual*/ QString ContainerEditModel::GetActionText(QString sNodeCodeName) const /*override*/
{
	QString sUndoText;
	switch(m_eCurAction)
	{
	case EDITMODEACTION_HoverCenter:
		sUndoText = "Translate Container " % sNodeCodeName;
		break;
	case EDITMODEACTION_AppendVertex:
	case EDITMODEACTION_InsertVertex:
		break;
	case EDITMODEACTION_HoverGrabPoint:
		sUndoText = "Adjust Container Size";
		break;

	case EDITMODEACTION_None:
	case EDITMODEACTION_Outside:
	case EDITMODEACTION_Creation:
	case EDITMODEACTION_Inside:

	default:
		HyGuiLog("ContainerEditModel::GetActionText - Invalid m_eCurTransform", LOGTYPE_Error);
		break;
	}

	return sUndoText;
}

/*virtual*/ QList<float> ContainerEditModel::GetActionSerialized() const /*override*/
{
	return Serialize();
}

/*virtual*/ QString ContainerEditModel::DoDeserialize(const QList<float> &floatList) /*override*/
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Assemble `m_GrabPointList` and `m_GrabPointCenter`
	std::vector<glm::vec2> grabPointList;
	if(floatList.empty() == false)
	{
		glm::vec2 ptCentroid;
		grabPointList = m_Box.DeserializeSelf(HYFIXTURE_Polygon, std::vector<float>(floatList.begin(), floatList.end()));
		m_Box.GetCentroid(ptCentroid);
		
		m_GrabPointCenter.Setup(ptCentroid);
	}

	// grabPointList is allowed to be empty, otherwise it will have proper data for the shape type
	if(grabPointList.empty())
		m_GrabPointList.clear();
	else
	{
		if(grabPointList.size() != 4)
			return "Invalid box shape data";

		m_GrabPointList.resize(4);
		for(int i = 0; i < 4; ++i)
			m_GrabPointList[i].Setup(GRABPOINT_ShapeCtrlAll, grabPointList[i]);
	}
	
	if(m_GrabPointList.empty())
	{
		m_Box.SetAsNothing();
		return "No container data provided";
	}

	return QString();
}

/*virtual*/ EditModeAction ContainerEditModel::DoMouseMoveIdle(glm::vec2 ptWorldMousePos) /*override*/
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

	if(IsValidModel() == false) // Any shape besides SHAPE_Polygon
		return EDITMODEACTION_Creation;

	if(m_Box.TestPoint(ptWorldMousePos, glm::identity<glm::mat4>()))
		return EDITMODEACTION_Inside;
	
	return EDITMODEACTION_Outside;
}

void ContainerEditModel::DoTransformCreation(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos)
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

	m_Box.SetAsBox(HyRect((ptUpperBound.x - ptLowerBound.x) * 0.5f, (ptUpperBound.y - ptLowerBound.y) * 0.5f, ptCenter, 0.0f));
}
