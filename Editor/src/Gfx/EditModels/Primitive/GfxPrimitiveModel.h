/**************************************************************************
*	GfxPrimLayerModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef GfxPrimLayerModel_H
#define GfxPrimLayerModel_H

#include "Global.h"
#include "IGfxEditModel.h"
#include "GfxShapeModel.h"
#include "GfxChainModel.h"

class GfxPrimLayerModel : public IGfxEditModel
{
	bool								m_bIsShape;
	GfxShapeModel						m_ShapeModel;
	GfxChainModel						m_ChainModel;

public:
	GfxPrimLayerModel();
	virtual ~GfxPrimLayerModel();

	bool IsShapeModel() const;

	QString GetPrimType() const;
	//void SetPrimType(QString sNewShape, QList<float> floatList);

	virtual QJsonObject Serialize() const override;

	virtual QString GetActionText(QString sNodeCodeName) const override; // Returns undo command description (blank if no change)

protected:
	virtual QString DoDeserialize(const QJsonObject &floatList) override;
	virtual EditModeAction DoMouseMoveIdle() override;
	virtual void DoTransformCreation(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos) override;
};

#endif // GfxPrimLayerModel_H
