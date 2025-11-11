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

	bool										m_bIsImportingTileSheet;
	QPixmap *									m_pImportTileSheetPixmap;
	QPolygonF									m_ImportPolygon;

public:
	explicit AuxTileSet(QWidget *pParent = nullptr);
	virtual ~AuxTileSet();

	void Init(AtlasTileSet *pTileSet);
	AtlasTileSet *GetTileSet() const;

	TileSetPage GetCurrentPage() const;

	void CmdSet_TileShapeWidget(TileSetShape eTileShape);	// Blocks the WgtVectorSpinBox::SetValue signal
	void CmdSet_TileSizeWidgets(QSize tileSize);			// Blocks the WgtVectorSpinBox::SetValue signal
	void CmdSet_TileOffsetWidgets(QPoint tileOffset);		// Blocks the WgtVectorSpinBox::SetValue signal

	void RefreshInfo();
	void UpdateSelection();

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

	void OnTabBarChanged(int iIndex);
};

#endif // WIDGETOUTPUTLOG_H
