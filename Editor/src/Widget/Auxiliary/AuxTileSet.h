/**************************************************************************
 *	AuxTileSet.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef AuxTileSet_H
#define AuxTileSet_H

#include <QWidget>
#include <QDataWidgetMapper>

class AtlasTileSet;
class IWgtTileSetItem;
class WgtTileSetAnimation;
class WgtTileSetTerrainSet;
class WgtTileSetTerrain;
class TileData;

namespace Ui {
class AuxTileSet;
}

enum TileSetPage
{
	TILESETPAGE_Import,
	TILESETPAGE_Arrange,
	TILESETPAGE_Animation,
	TILESETPAGE_Autotile,
	TILESETPAGE_Collision,
	TILESETPAGE_CustomData
};

class AuxTileSet : public QWidget
{
	Q_OBJECT

	QTabBar *									m_pTabBar;

	AtlasTileSet *								m_pTileSet;
	WgtTileSetAnimation *						m_pSelectedAnimationWgt;
	WgtTileSetTerrainSet *						m_pSelectedTerrainSetWgt;
	WgtTileSetTerrain *							m_pSelectedTerrainWgt;

	bool										m_bIsImportingTileSheet;
	QPixmap *									m_pImportTileSheetPixmap;
	QPolygonF									m_ImportPolygon;

	QList<WgtTileSetAnimation *>				m_AnimationList;
	QList<WgtTileSetTerrainSet *>				m_TerrainSetList;

public:
	explicit AuxTileSet(QWidget *pParent = nullptr);
	virtual ~AuxTileSet();

	void Init(AtlasTileSet *pTileSet);
	AtlasTileSet *GetTileSet() const;

	TileSetPage GetCurrentPage() const;
	void SetCurrentPage(TileSetPage ePage);

	QUuid GetSelectedAnimation() const;
	QUuid GetSelectedTerrainSet() const;
	QUuid GetSelectedTerrain() const;

	void CmdSet_TileShapeWidget(TileSetShape eTileShape);	// Blocks the WgtVectorSpinBox::SetValue signal
	void CmdSet_TileSizeWidgets(QSize tileSize);			// Blocks the WgtVectorSpinBox::SetValue signal
	void CmdSet_TileOffsetWidgets(QPoint tileOffset);		// Blocks the WgtVectorSpinBox::SetValue signal

	void RefreshInfo();
	void UpdateGfxItemSelection();

	IWgtTileSetItem *FindWgtItem(QUuid uuid) const;
	int GetWgtItemIndex(QUuid uuid) const;
	void CmdSet_CreateWgtItem(TileSetWgtType eType, QJsonObject data);
	void CmdSet_DeleteWgtItem(QUuid uuid);
	void CmdSet_OrderWgtItem(QUuid uuid, int newIndex);
	void CmdSet_ModifyWgtItem(QUuid uuid, QJsonObject newData);
	void MakeSelectionChange(IWgtTileSetItem *pItem);

	void CmdSet_ApplyTerrainSet(QList<TileData *> tileDataList, QList<QUuid> terrainSetUuidList);

	void SetPainting_Animation(QUuid animUuid);

private:
	Ui::AuxTileSet *ui;

	void SetImportWidgets();
	void SliceSheetPixmaps();
	void ErrorCheckImport();

	bool IsPixmapAllTransparent(const QPixmap &pixmap);

private Q_SLOTS:
	void on_undoStack_cleanChanged(bool bClean);
	void on_undoStack_indexChanged(int iIndex);

	void on_radTileSheet_toggled(bool bChecked);
	void on_radTileImages_toggled(bool bChecked);

	void on_btnImageBrowse_clicked();

	void on_cmbTileShape_currentIndexChanged(int iIndex);
	void OnTileSizeChanged(QVariant newSize);
	void OnTileOffsetChanged(QVariant newOffset);

	void OnTextureRegionChanged(QVariant newSize);
	void OnStartOffsetChanged(QVariant newOffset);
	void OnPaddingChanged(QVariant newPadding);

	void on_radImportBottom_toggled(bool bChecked);
	void on_radImportTop_toggled(bool bChecked);
	void on_radImportLeft_toggled(bool bChecked);
	void on_radImportRight_toggled(bool bChecked);
	void on_btnConfirmAdd_clicked();

	void on_actionRemoveTiles_triggered();
	void on_actionReplaceTiles_triggered();

	void on_btnAddAnimation_clicked();
	void on_btnAddTerrainSet_clicked();

	void OnTabBarChanged(int iIndex);
};

#endif // WIDGETOUTPUTLOG_H
