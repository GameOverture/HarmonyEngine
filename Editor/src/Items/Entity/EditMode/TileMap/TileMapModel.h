/**************************************************************************
*	TileMapModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef TileMapModel_H
#define TileMapModel_H

#include "Global.h"
#include "IEditModeModel.h"
#include "PropertiesTreeModel.h"

#include "vendor/libtiled/tilelayer.h"

class AtlasTileSet;
class AtlasManager;

class TileMapModel : public IEditModeModel
{
	Project &					m_ProjectRef;

	HyTileMapLayout				m_eLayout;
	HyTileMapStagger			m_eStagger;
	glm::ivec2					m_vTileSize;

	Tiled::TileLayer			m_TiledLayer;

public:
	TileMapModel(Project &projectRef, QUndoStack *pUndoStack);
	virtual ~TileMapModel();

	virtual QJsonObject Serialize() const override;
	virtual void Deserialize(bool bEnabled, const QJsonObject &serializedObj) override;

	virtual Qt::CursorShape MouseMoveIdle() override;
	virtual void MouseIdleRightClick() override;
	virtual bool MousePressEvent(EditModeState eEditModeState, bool bShiftHeld) override; // Returns whether transform has begun (otherwise marquee select)
	virtual void MouseTransform(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos) override;
	virtual void MouseMarqueeReleased(EditModeState eEditModeState, bool bLeftClick, QPointF ptBotLeft, QPointF ptTopRight) override;
	virtual void MouseClickTransformReleased(glm::vec2 ptClickPos) override;

	virtual void OnDeleteKeyPressed() override;
	
	virtual QString GetActionText(EditModeState eEditModeState, QString sNodeCodeName) const override; // Returns undo command description (blank if no change)
	virtual void ClearAction() override;

	HyTileMapLayout GetLayout() const;
	HyTileMapStagger GetStagger() const;
	glm::ivec2 GetTileSize() const;
	const Tiled::TileLayer &GetTiledTileLayer() const;
	QList<AtlasTileSet *> UsedTilesets(const AtlasManager &atlasManagerRef) const;

	HyShader *GetSquareShader();
	HyShader *GetHexShader();

	void SetCell(int iX, int iY, AtlasTileSet *pTileSet, int iTileId);

	void UpdateTileIds(const std::vector<std::pair<uint16, uint16>> &modifiedIndexList); // Pair<old, new>
};

#endif // TileMapModel_H
