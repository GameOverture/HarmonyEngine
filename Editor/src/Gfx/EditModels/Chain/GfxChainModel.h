/**************************************************************************
*	GfxChainModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef GfxChainModel_H
#define GfxChainModel_H

#include "Global.h"
#include "IGfxEditModel.h"

class GfxChainModel : public IGfxEditModel
{
	friend class GfxPrimLayerModel;

public:
	GfxChainModel(HyColor color, const QList<float> &floatList = QList<float>());
	virtual ~GfxChainModel();

	HyChain2d *GetChain();
	const HyChain2d *GetChain() const;

	virtual QJsonObject Serialize() const override;

	void TransformData(glm::mat4 mtxTransform);

	bool IsLoopClosed() const;

	virtual QString GetActionText(QString sNodeCodeName) const override; // Returns undo command description (blank if no change)

protected:
	virtual QString DoDeserialize(const QJsonObject &serializedObj) override;
	virtual EditModeAction DoMouseMoveIdle() override;
	virtual void DoTransformCreation(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos) override;

	bool CheckIfAddVertexOnEdge();
	bool IsShareEdge(const std::vector<glm::vec2> &a, const std::vector<glm::vec2> &b, int &a0, int &a1, int &b0, int &b1);
};

#endif // GfxChainModel_H
