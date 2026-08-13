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

#include "vendor/libtiled/gidmapper.h"
#include "vendor/libtiled/tilelayer.h"

class AtlasTileSet;
class AtlasManager;
class TileData;

typedef QMap<QPoint, TileData *> TileMapBrush;

class TileMapModel : public IEditModeModel
{
	Project &									m_ProjectRef;

	std::unique_ptr<Tiled::Map>					m_pTiledMap;

	bool										m_bValidHoverCoord;
	QPoint										m_ptHoverCoord;

	QMap<const AtlasTileSet *, TileMapBrush>	m_TileSetBrushMap;
	QMap<int, TileMapBrush>						m_PresetBrushMap;
	TileMapBrush *								m_pCurrentBrush;

public:
	TileMapModel(Project &projectRef, QUndoStack *pUndoStack, QString sLayerCodeName);
	virtual ~TileMapModel();

	virtual QJsonObject Serialize() const override;
	virtual void Deserialize(bool bEnabled, const QJsonObject &serializedObj) override;

	virtual Qt::CursorShape MouseMoveIdle() override;
	virtual void MouseIdleRightClick() override;
	virtual bool MousePressEvent(EditModeState eEditModeState, bool bShiftHeld, glm::vec2 ptClickPos) override; // Returns whether transform has begun (otherwise marquee select)
	virtual void MouseTransform(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos) override;
	virtual void MouseMarqueeReleased(EditModeState eEditModeState, bool bLeftClick, QPointF ptBotLeft, QPointF ptTopRight) override;
	virtual void MouseClickTransformReleased(glm::vec2 ptReleasePos) override;

	virtual void OnDeleteKeyPressed() override;
	
	virtual QString GetActionText(EditModeState eEditModeState, QString sNodeCodeName) const override; // Returns undo command description (blank if no change)
	virtual void ClearAction() override;

	glm::ivec2 GetGridSize() const;
	void SetGridSize(glm::ivec2 vGridSize);

	HyTileMapLayout GetLayout() const;
	void SetLayout(HyTileMapLayout eLayout);

	void SetHoverCoordinates(bool bValidHoverCoord, QPoint ptHoverCoord);

	const Tiled::TileLayer &GetTiledTileLayer() const;
	QList<AtlasTileSet *> UsedTilesets() const;

	HyShader *GetGridShader();

	void CreateBrush(const AtlasTileSet *pTileSet, QList<TileData *> tileList);
	void CreateBrush(int iPresetIndex, TileMapBrush brush);
	void SetBrush(const AtlasTileSet *pTileSet);
	void SetBrush(int iPresetIndex);

	void SetCellsBrush();
	void SetCell(int iX, int iY, TileData *pTileData);

	void UpdateTileIds(const std::vector<std::pair<uint16, uint16>> &modifiedIndexList); // Pair<old, new>
};

#endif // TileMapModel_H
