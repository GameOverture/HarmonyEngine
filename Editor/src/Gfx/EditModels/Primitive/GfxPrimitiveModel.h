/**************************************************************************
*	GfxPrimitiveModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef GfxPrimitiveModel_H
#define GfxPrimitiveModel_H

#include "Global.h"
#include "IGfxEditModel.h"
#include "GfxShapeModel.h"
#include "GfxChainModel.h"

class GfxPrimitiveModel : public IGfxEditModel
{
	bool								m_bIsShape;
	GfxShapeModel						m_ShapeModel;
	GfxChainModel						m_ChainModel;

public:
	GfxPrimitiveModel();
	virtual ~GfxPrimitiveModel();

	virtual bool IsValidModel() const override;
	bool IsShapeModel() const;

	QString GetPrimType() const;
	void SetPrimType(QString sNewShape);

	virtual QList<float> Serialize() const override;

	virtual QString MouseTransformReleased(QString sShapeCodeName, QPointF ptWorldMousePos) override; // Returns undo command description (blank if no change)

protected:
	virtual void DoDeserialize(const QList<float> &floatList) override;
	virtual EditModeAction DoMouseMoveIdle(glm::vec2 ptWorldMousePos) override;
	virtual void DoTransformCreation(glm::vec2 ptStartPos, glm::vec2 ptDragPos) override;
};

#endif // GfxShapeModel_H
