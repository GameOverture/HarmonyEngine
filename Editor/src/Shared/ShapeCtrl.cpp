/**************************************************************************
 *	ShapeCtrl.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "ShapeCtrl.h"

ShapeCtrl::ShapeCtrl(HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_eShape(SHAPE_None),
	m_BoundingVolume(this),
	m_Outline(this)
{
	m_Outline.UseWindowCoordinates();
	m_Outline.SetWireframe(true);

	SetVisible(false);
}

/*virtual*/ ShapeCtrl::~ShapeCtrl()
{
}

EditorShape ShapeCtrl::GetShapeType() const
{
	return m_eShape;
}

void ShapeCtrl::Setup(EditorShape eShape, HyColor color, float fBvAlpha, float fOutlineAlpha)
{
	if(eShape == SHAPE_None)
	{
		m_BoundingVolume.SetAsNothing();
		m_Outline.SetAsNothing();
	}
	else if(m_eShape != SHAPE_None && m_eShape != eShape)
		ConvertTo(eShape);

	m_eShape = eShape;

	m_BoundingVolume.SetTint(color);
	m_Outline.SetTint(color);

	if(fBvAlpha == 0.0f)
	{
		m_BoundingVolume.SetVisible(false);
		m_BoundingVolume.alpha.Set(0.0f);
	}
	else
	{
		m_BoundingVolume.SetVisible(true);
		m_BoundingVolume.alpha.Tween(fBvAlpha, 0.5f);
	}

	if(fOutlineAlpha == 0.0f)
	{
		m_Outline.SetVisible(false);
		m_Outline.alpha.Set(0.0f);
	}
	else
	{
		m_Outline.SetVisible(true);
		m_Outline.alpha.Tween(fOutlineAlpha, 0.5f);
	}
}

HyPrimitive2d &ShapeCtrl::GetPrimitive()
{
	return m_BoundingVolume;
}

void ShapeCtrl::SetAsDrag(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos, HyCamera2d *pCamera)
{
	SetVisible(true);

	glm::vec2 ptLowerBound, ptUpperBound, ptCenter;
	if(bShiftMod)
	{
		ptCenter = ptStartPos;

		glm::vec2 vRadius = ptStartPos - ptDragPos;
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

	glm::vec2 ptWindowLowerBound, ptWindowUpperBound, ptWindowCenter;
	pCamera->ProjectToCamera(ptLowerBound, ptWindowLowerBound);
	pCamera->ProjectToCamera(ptUpperBound, ptWindowUpperBound);
	ptWindowCenter = ptWindowLowerBound + ((ptWindowUpperBound - ptWindowLowerBound) * 0.5f);

	switch(m_eShape)
	{
	case SHAPE_None:
		m_BoundingVolume.SetAsNothing();
		m_Outline.SetAsNothing();
		break;

	case SHAPE_Box:
		m_BoundingVolume.SetAsBox((ptUpperBound.x - ptLowerBound.x) * 0.5f, (ptUpperBound.y - ptLowerBound.y) * 0.5f, ptCenter, 0.0f);
		m_Outline.SetAsBox((ptWindowUpperBound.x - ptWindowLowerBound.x) * 0.5f, (ptWindowUpperBound.y - ptWindowLowerBound.y) * 0.5f, ptWindowCenter, 0.0f);
		break;

	case SHAPE_Circle:
		m_BoundingVolume.SetAsCircle(ptCenter, glm::distance(ptCenter, ptUpperBound));
		m_Outline.SetAsCircle(ptWindowCenter, glm::distance(ptWindowCenter, ptWindowUpperBound));
		break;

	case SHAPE_Polygon: {
		glm::vec2 ptVertList[6];
		float fRadius = glm::distance(ptCenter, ptUpperBound);
		ptVertList[0] = ptCenter + glm::vec2(fRadius, 0.0f);
		ptVertList[1] = ptCenter + glm::vec2(fRadius * 0.5f, fRadius * sqrt(3.0f) * 0.5f);
		ptVertList[2] = ptCenter + glm::vec2(-fRadius * 0.5f, fRadius * sqrt(3.0f) * 0.5f);
		ptVertList[3] = ptCenter + glm::vec2(-fRadius, 0.0f);
		ptVertList[4] = ptCenter + glm::vec2(-fRadius * 0.5f, -fRadius * sqrt(3.0f) * 0.5f);
		ptVertList[5] = ptCenter + glm::vec2(fRadius * 0.5f, -fRadius * sqrt(3.0f) * 0.5f);
		m_BoundingVolume.SetAsPolygon(ptVertList, 6);

		fRadius = glm::distance(ptWindowCenter, ptWindowUpperBound);
		ptVertList[0] = ptWindowCenter + glm::vec2(fRadius, 0.0f);
		ptVertList[1] = ptWindowCenter + glm::vec2(fRadius * 0.5f, fRadius * sqrt(3.0f) * 0.5f);
		ptVertList[2] = ptWindowCenter + glm::vec2(-fRadius * 0.5f, fRadius * sqrt(3.0f) * 0.5f);
		ptVertList[3] = ptWindowCenter + glm::vec2(-fRadius, 0.0f);
		ptVertList[4] = ptWindowCenter + glm::vec2(-fRadius * 0.5f, -fRadius * sqrt(3.0f) * 0.5f);
		ptVertList[5] = ptWindowCenter + glm::vec2(fRadius * 0.5f, -fRadius * sqrt(3.0f) * 0.5f);
		m_Outline.SetAsPolygon(ptVertList, 6);
		break; }

	case SHAPE_LineSegment:
		m_BoundingVolume.SetAsLineSegment(ptStartPos, ptDragPos);
		pCamera->ProjectToCamera(ptStartPos, ptStartPos);
		pCamera->ProjectToCamera(ptDragPos, ptDragPos);
		m_Outline.SetAsLineSegment(ptStartPos, ptDragPos);
		break;

	case SHAPE_LineChain: {
		std::vector<glm::vec2> vertList;
		vertList.push_back(ptStartPos);
		vertList.push_back(ptDragPos);
		m_BoundingVolume.SetAsLineChain(vertList);

		pCamera->ProjectToCamera(ptStartPos, ptStartPos);
		pCamera->ProjectToCamera(ptDragPos, ptDragPos);
		vertList.clear();
		vertList.push_back(ptStartPos);
		vertList.push_back(ptDragPos);
		m_Outline.SetAsLineChain(vertList);
		break; }

	case SHAPE_LineLoop: {
		std::vector<glm::vec2> vertList;
		vertList.push_back(ptStartPos);
		vertList.push_back(ptDragPos);
		m_BoundingVolume.SetAsLineLoop(vertList);

		pCamera->ProjectToCamera(ptStartPos, ptStartPos);
		pCamera->ProjectToCamera(ptDragPos, ptDragPos);
		vertList.clear();
		vertList.push_back(ptStartPos);
		vertList.push_back(ptDragPos);
		m_Outline.SetAsLineLoop(vertList);
		break; }

	default:
		HyGuiLog("ShapeCtrl::SetAsDrag - Unhandled shape type: " % QString::number(m_eShape), LOGTYPE_Error);
		break;
	}
}

QString ShapeCtrl::Serialize()
{
	HyShape2d shape;
	m_BoundingVolume.CalcLocalBoundingShape(shape);

	QList<float> floatList;
	switch(m_eShape)
	{
	case SHAPE_None:
		break;

	case SHAPE_Polygon:
	case SHAPE_Box: {
		const b2PolygonShape *pPolyShape = static_cast<const b2PolygonShape *>(shape.GetB2Shape());
		for(int i = 0; i < pPolyShape->m_count; ++i)
		{
			floatList.push_back(pPolyShape->m_vertices[i].x);
			floatList.push_back(pPolyShape->m_vertices[i].y);
		}
		break; }

	case SHAPE_Circle: {
		const b2CircleShape *pCircleShape = static_cast<const b2CircleShape *>(shape.GetB2Shape());
		floatList.push_back(pCircleShape->m_p.x);
		floatList.push_back(pCircleShape->m_p.y);
		floatList.push_back(pCircleShape->m_radius);
		break; }

	case SHAPE_LineSegment: {
		const b2EdgeShape *pLineSegment = static_cast<const b2EdgeShape *>(shape.GetB2Shape());
		floatList.push_back(pLineSegment->m_vertex1.x);
		floatList.push_back(pLineSegment->m_vertex1.y);
		floatList.push_back(pLineSegment->m_vertex2.x);
		floatList.push_back(pLineSegment->m_vertex2.y);
		break; }

	case SHAPE_LineChain:
	case SHAPE_LineLoop: {
		const b2ChainShape *pChainShape = static_cast<const b2ChainShape *>(shape.GetB2Shape());
		for(int i = 0; i < pChainShape->m_count; ++i)
		{
			floatList.push_back(pChainShape->m_vertices[i].x);
			floatList.push_back(pChainShape->m_vertices[i].y);
		}
		break; }
	}

	QString sSerializedData;
	for(float f : floatList)
	{
		sSerializedData.append(QString::number(f));
		sSerializedData.append(',');
	}
	sSerializedData.chop(1);

	return sSerializedData;
}

void ShapeCtrl::Deserialize(QString sData, HyCamera2d *pCamera)
{
	QStringList sFloatList = sData.split(',', Qt::SkipEmptyParts);

	m_DeserializedFloatList.clear();
	for(QString sFloat : sFloatList)
	{
		bool bOk;
		float fValue = sFloat.toFloat(&bOk);
		if(bOk)
			m_DeserializedFloatList.push_back(fValue);
	}

	switch(m_eShape)
	{
	case SHAPE_None:
		break;

	case SHAPE_Polygon:
	case SHAPE_Box: {
		std::vector<glm::vec2> vertList;
		for(int i = 0; i < m_DeserializedFloatList.size(); i += 2)
			vertList.push_back(glm::vec2(m_DeserializedFloatList[i], m_DeserializedFloatList[i + 1]));

		m_BoundingVolume.SetAsPolygon(vertList);
		break; }

	case SHAPE_Circle: {
		glm::vec2 ptCenter(m_DeserializedFloatList[0], m_DeserializedFloatList[1]);
		float fRadius = m_DeserializedFloatList[2];
		m_BoundingVolume.SetAsCircle(ptCenter, fRadius);
		break; }

	case SHAPE_LineSegment: {
		glm::vec2 ptOne(m_DeserializedFloatList[0], m_DeserializedFloatList[1]);
		glm::vec2 ptTwo(m_DeserializedFloatList[2], m_DeserializedFloatList[3]);
		m_BoundingVolume.SetAsLineSegment(ptOne, ptTwo);
		break; }

	case SHAPE_LineChain:
	case SHAPE_LineLoop: {
		std::vector<glm::vec2> vertList;
		for(int i = 0; i < m_DeserializedFloatList.size(); i += 2)
			vertList.push_back(glm::vec2(m_DeserializedFloatList[i], m_DeserializedFloatList[i + 1]));

		m_BoundingVolume.SetAsLineChain(vertList);
		break; }
	}

	RefreshTransform(pCamera);
}

void ShapeCtrl::RefreshTransform(HyCamera2d *pCamera)
{
	if(m_DeserializedFloatList.empty())
		return;

	SetVisible(true);

	switch(m_eShape)
	{
	case SHAPE_None:
		break;

	case SHAPE_Polygon:
	case SHAPE_Box: {
		std::vector<glm::vec2> vertList;
		for(int i = 0; i < m_DeserializedFloatList.size(); i += 2)
		{
			glm::vec2 ptCameraPos(m_DeserializedFloatList[i], m_DeserializedFloatList[i + 1]);
			pCamera->ProjectToCamera(ptCameraPos, ptCameraPos);
			vertList.push_back(ptCameraPos);
		}

		

		m_Outline.SetAsPolygon(vertList);
		break; }

	case SHAPE_Circle: {
		glm::vec2 ptCenter(m_DeserializedFloatList[0], m_DeserializedFloatList[1]);
		pCamera->ProjectToCamera(ptCenter, ptCenter);
		float fRadius = m_DeserializedFloatList[2] * pCamera->GetZoom();

		m_Outline.SetAsCircle(ptCenter, fRadius);
		break; }

	case SHAPE_LineSegment: {
		glm::vec2 ptOne(m_DeserializedFloatList[0], m_DeserializedFloatList[1]);
		pCamera->ProjectToCamera(ptOne, ptOne);
		glm::vec2 ptTwo(m_DeserializedFloatList[2], m_DeserializedFloatList[3]);
		pCamera->ProjectToCamera(ptTwo, ptTwo);

		m_Outline.SetAsLineSegment(ptOne, ptTwo);
		break; }

	case SHAPE_LineChain:
	case SHAPE_LineLoop: {
		std::vector<glm::vec2> vertList;
		for(int i = 0; i < m_DeserializedFloatList.size(); i += 2)
		{
			glm::vec2 ptCameraPos(m_DeserializedFloatList[i], m_DeserializedFloatList[i + 1]);
			pCamera->ProjectToCamera(ptCameraPos, ptCameraPos);
			vertList.push_back(ptCameraPos);
		}

		m_Outline.SetAsLineChain(vertList);
		break; }
	}
}

void ShapeCtrl::ConvertTo(EditorShape eShape)
{
	HyGuiLog("ShapeCtrl::ConvertTo - Not implemented", LOGTYPE_Error);
}
