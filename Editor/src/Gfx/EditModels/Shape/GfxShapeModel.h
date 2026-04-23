///**************************************************************************
//*	GfxShapeModel.h
//*
//*	Harmony Engine - Editor Tool
//*	Copyright (c) 2025 Jason Knobler
//*
//*	Harmony Editor Tool License:
//*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
//*************************************************************************/
//#ifndef GfxShapeModel_H
//#define GfxShapeModel_H
//
//#include "Global.h"
//#include "IGfxEditModel.h"
//
//class GfxShapeModel : public IGfxEditModel
//{
//	friend class GfxPrimLayerModel;
//
//public:
//	GfxShapeModel(HyColor color);
//	virtual ~GfxShapeModel();
//
//	
//	HyShape2d *GetShape(int iIndex);
//	const HyShape2d *GetShape(int iIndex) const;
//
//	EditorShape GetShapeType() const;
//	void SetShapeType(EditorShape eNewShape, QList<float> floatList);
//
//	void TransformData(glm::mat4 mtxTransform);
//
//	bool IsLoopClosed() const;
//
//	virtual QString GetActionText(QString sNodeCodeName) const override;	// Returns undo command description (blank if no change)
//	//virtual QJsonObject GetActionSerialized() const override;
//
//protected:
//	virtual QString DoDeserialize(const QJsonObject &serializedObj) override;
//	virtual EditModeAction DoMouseMoveIdle() override;
//	virtual void DoTransformCreation(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos) override;
//
//	bool CheckIfAddVertexOnEdge();
//
//
//};
//
//#endif // GfxShapeModel_H
