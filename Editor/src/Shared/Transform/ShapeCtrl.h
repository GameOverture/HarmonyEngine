/**************************************************************************
 *	ShapeCtrl.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef SHAPECTRL_H
#define SHAPECTRL_H

#include "Global.h"
#include "GrabPoint.h"
#include "IDraw.h"

enum SemState
{
	SEMSTATE_Invalid = -2,
	SEMSTATE_None = -1,

	SEMSTATE_Translate = 0,
	SEMSTATE_GrabPoint,
	SEMSTATE_RadiusHorizontal,
	SEMSTATE_RadiusVertical,
	SEMSTATE_Add,
	SEMSTATE_RemoveSelected
};

class ShapeCtrl
{
protected:
	EditorShape					m_eShape;

	HyPrimitive2d				m_BoundingVolume;	// This shape represents how the item's transformation is applied. Uses world/camera coordinates
	HyPrimitive2d				m_Outline;			// This shape represents the raw data in the form of an outline unaffected by camera zoom. Uses window coordinates

	QList<float>				m_DeserializedFloatList;
	QList<GrabPoint *>			m_VertexGrabPointList;	// GrabPoint's use world/camera coordinates

	bool						m_bIsVem;

public:
	ShapeCtrl(HyEntity2d *pParent);
	ShapeCtrl(const ShapeCtrl &copyRef);
	virtual ~ShapeCtrl();

	void Setup(EditorShape eShape, HyColor color, float fBvAlpha, float fOutlineAlpha); // Alphas of 0.0f just get their SetVisible(false)

	EditorShape GetShapeType() const;
	HyPrimitive2d &GetPrimitive(bool bTransformed);

	void SetAsDrag(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos, HyCamera2d *pCamera);
	void SetAsText(HyText2d *pTextNode, bool bShowOutline, HyCamera2d *pCamera);

	QString Serialize();
	void Deserialize(QString sData, HyCamera2d *pCamera);

	void TransformSelf(glm::mat4 mtxTransform); // NOTE: Does not update m_Outline, requires a DeserializeOutline()

	void DeserializeOutline(HyCamera2d *pCamera);

	bool IsVemEnabled() const;
	void EnableVertexEditMode();
	SemState GetMouseSemHoverAction(bool bCtrlMod, bool bShiftMod, bool bSelectVert);
	void SelectVemVerts(b2AABB selectionAabb, HyCamera2d *pCamera);
	void TransformSemVerts(SemState eSemState, glm::vec2 ptStartPos, glm::vec2 ptDragPos, HyCamera2d *pCamera);
	bool RemoveSelectedVerts();
	QString SerializeVemVerts(HyCamera2d *pCamera);
	void UnselectAllVemVerts();
	void ClearVertexEditMode();

	// 'sCodeName' has the dereference operator appended to its name (aka '->' or '.')
	static QString DeserializeAsRuntimeCode(QString sCodeName, EditorShape eShapeType, QJsonArray floatArray, QString sNewLine, uint32 &uiMaxVertListSizeOut);

protected:
	void SetVertexGrabPointListSize(uint32 uiNumGrabPoints);
	//void ConvertTo(EditorShape eShape);
};

#endif // SHAPECTRL_H
