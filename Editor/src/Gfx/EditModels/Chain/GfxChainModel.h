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
	friend class GfxPrimitiveModel;

	// "Chain", "Data" - when serialized in property (QJsonArray of floats)
	HyChain2d							m_Chain;				// This is the actual shape data used for physics/collision/rendering - usually just one fixture, but could be multiple for complex polygons
	
	// Extra validation used with Chain
	bool								m_bReverseWindingOrder;
	bool								m_bSelfIntersecting;
	glm::vec2							m_ptSelfIntersection;
	bool								m_bLoopClosed;

public:
	GfxChainModel(HyColor color, const QList<float> &floatList = QList<float>());
	virtual ~GfxChainModel();

	virtual bool IsValidModel() const override;

	virtual QList<float> Serialize() const override;

	void TransformData(glm::mat4 mtxTransform);

	const HyChain2d &GetChainFixture() const;
	bool IsLoopClosed() const;

	virtual QString MouseTransformReleased(QString sShapeCodeName, QPointF ptWorldMousePos) override; // Returns undo command description (blank if no change)

protected:
	virtual void DoDeserialize(const QList<float> &floatList) override;
	virtual EditModeAction DoMouseMoveIdle(glm::vec2 ptWorldMousePos) override;
	virtual void DoTransformCreation(glm::vec2 ptStartPos, glm::vec2 ptDragPos) override;

	bool CheckIfAddVertexOnEdge(glm::vec2 ptWorldMousePos);
	bool IsShareEdge(const std::vector<glm::vec2> &a, const std::vector<glm::vec2> &b, int &a0, int &a1, int &b0, int &b1);
};

#endif // GfxChainModel_H
