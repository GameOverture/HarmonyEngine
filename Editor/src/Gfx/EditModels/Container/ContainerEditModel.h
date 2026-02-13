/**************************************************************************
*	ContainerEditModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef ContainerEditModel_H
#define ContainerEditModel_H

#include "Global.h"
#include "IGfxEditModel.h"

class ContainerEditModel : public IGfxEditModel
{
	friend class GfxPrimitiveModel;

	// "Shape", "Data" - when serialized in property (QJsonArray of floats)
	HyShape2d							m_Box;			// This is the actual shape data used for physics/collision/rendering - usually just one fixture, but could be multiple for complex polygons

public:
	ContainerEditModel();
	virtual ~ContainerEditModel();

	virtual bool IsValidModel() const override;
	virtual QList<float> Serialize() const override;
	virtual QString GetActionText(QString sNodeCodeName) const override;	// Returns undo command description (blank if no change)
	virtual QList<float> GetActionSerialized() const override;

protected:
	virtual QString DoDeserialize(const QList<float> &floatList) override;
	virtual EditModeAction DoMouseMoveIdle(glm::vec2 ptWorldMousePos) override;
	virtual void DoTransformCreation(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos) override;
};

#endif // ContainerEditModel_H
