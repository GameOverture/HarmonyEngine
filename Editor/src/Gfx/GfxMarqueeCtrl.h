/**************************************************************************
 *	GfxMarqueeCtrl.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef GfxMarqueeCtrl_H
#define GfxMarqueeCtrl_H

#include "Global.h"

class GfxMarqueeCtrl : public HyEntity2d
{
protected:
	bool						m_bIsActive;

	HyPrimitive2d				m_BoundingVolume;	// This shape represents how the item's transformation is applied. Uses world/camera coordinates
	HyPrimitive2d				m_Outline;			// This shape represents the raw data in the form of an outline unaffected by camera zoom. Uses window coordinates

public:
	GfxMarqueeCtrl(HyEntity2d *pParent);
	virtual ~GfxMarqueeCtrl();

	void SetAsDrag(glm::vec2 ptStartPos, glm::vec2 ptDragPos);
	b2AABB GetSelection();
	void Hide();

	//QString Serialize();
	//void Deserialize(QString sData, HyCamera2d *pCamera);

	//void TransformSelf(glm::mat4 mtxTransform); // NOTE: Does not update m_Outline, requires a DeserializeOutline()

	//void DeserializeOutline(HyCamera2d *pCamera);

	//bool IsVemEnabled() const;
	//void EnableVertexEditMode();
	//SemState GetMouseSemHoverAction(bool bCtrlMod, bool bShiftMod, bool bSelectVert);
	//void SelectVemVerts(b2AABB selectionAabb, HyCamera2d *pCamera);
	//void TransformSemVerts(SemState eSemState, glm::vec2 ptStartPos, glm::vec2 ptDragPos, HyCamera2d *pCamera);
	//bool RemoveSelectedVerts();
	//QString SerializeVemVerts(HyCamera2d *pCamera);
	//void UnselectAllVemVerts();
	//void ClearVertexEditMode();

	// 'sCodeName' has the dereference operator appended to its name (aka '->' or '.')
	//static QString DeserializeAsRuntimeCode(QString sCodeName, EditorShape eShapeType, QJsonArray floatArray, QString sNewLine, uint32 &uiMaxVertListSizeOut);

protected:
	//void SetVertexGrabPointListSize(uint32 uiNumGrabPoints);
	//void ConvertTo(EditorShape eShape);
};

#endif // GfxMarqueeCtrl_H
