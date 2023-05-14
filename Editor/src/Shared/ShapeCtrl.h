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
#include "TransformCtrl.h"

class ShapeCtrl
{
public:
	enum VemAction
	{
		VEMACTION_Invalid = -2,
		VEMACTION_None = -1,

		VEMACTION_Translate = 0,
		VEMACTION_GrabPoint,
		VEMACTION_RadiusHorizontal,
		VEMACTION_RadiusVertical,
		VEMACTION_Add,
		VEMACTION_RemoveSelected
	};

protected:
	EditorShape					m_eShape;

	HyPrimitive2d				m_BoundingVolume;	// Uses world/camera coordinates
	HyPrimitive2d				m_Outline;			// Uses window coordinates (unaffected by zoom)

	QList<float>				m_DeserializedFloatList;
	QList<GrabPoint *>			m_VertexGrabPointList;

	bool						m_bIsVem;

public:
	ShapeCtrl(HyEntity2d *pParent);
	ShapeCtrl(const ShapeCtrl &copyRef);
	virtual ~ShapeCtrl();

	void Setup(EditorShape eShape, HyColor color, float fBvAlpha, float fOutlineAlpha); // Alphas of 0.0f just get their SetVisible(false)

	EditorShape GetShapeType() const;
	HyPrimitive2d &GetPrimitive();

	void SetAsDrag(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos, HyCamera2d *pCamera);
	void SetAsText(HyText2d *pTextNode, bool bShowOutline, HyCamera2d *pCamera);

	QString Serialize();
	void Deserialize(QString sData, HyCamera2d *pCamera);

	void TransformSelf(glm::mat4 mtxTransform); // NOTE: Does not update m_Outline, requires a DeserializeOutline()

	void DeserializeOutline(HyCamera2d *pCamera);

	bool IsVemEnabled() const;
	void EnableVertexEditMode();
	VemAction GetMouseVemAction(bool bCtrlMod, bool bShiftMod, bool bSelectVert);
	void SelectVemVerts(b2AABB selectionAabb, HyCamera2d *pCamera);
	bool TransformVemVerts(VemAction eAction, glm::vec2 ptStartPos, glm::vec2 ptDragPos, HyCamera2d *pCamera);
	QString SerializeVemVerts(HyCamera2d *pCamera);
	void UnselectAllVemVerts();
	void ClearVertexEditMode();

	// 'sCodeName' has the dereference operator appended to its name (aka '->' or '.')
	static QString DeserializeAsRuntimeCode(QString sCodeName, EditorShape eShapeType, QString sData, QString sNewLine);

protected:
	void SetVertexGrabPointListSize(uint32 uiNumGrabPoints);
	//void ConvertTo(EditorShape eShape);
};

#endif // SHAPECTRL_H
