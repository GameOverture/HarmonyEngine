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

class AuxTileSet : public QWidget
{
	Q_OBJECT

	AtlasTileSet *								m_pTileSet;

	bool										m_bIsImportingTileSheet;
	QPixmap *									m_pImportTileSheetPixmap;
	QPolygonF									m_ImportPolygon;

	enum TabIndex
	{
		TAB_AddTiles = 0,
		TAB_Properties
	};

public:
	explicit AuxTileSet(QWidget *pParent = nullptr);
	virtual ~AuxTileSet();

	void Init(AtlasTileSet *pTileSet);
	AtlasTileSet *GetTileSet() const;

	void SetTileShapeWidget(TileSetShape eTileShape);
	void SetTileSizeWidgets(QSize tileSize); // Blocks the WgtVectorSpinBox::SetValue signal

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

	void on_actionSave_triggered();
	void on_tabWidget_currentChanged(int iIndex);

	void on_radTileSheet_toggled(bool bChecked);
	void on_radTileImages_toggled(bool bChecked);

	void on_btnImageBrowse_clicked();

	void on_cmbTileShape_currentIndexChanged(int iIndex);
	void OnTileSizeChanged(QVariant newSize);

	void OnTextureRegionChanged(QVariant newSize);
	void OnStartOffsetChanged(QVariant newOffset);
	void OnPaddingChanged(QVariant newPadding);

	void on_btnConfirmAdd_clicked();
};

#endif // WIDGETOUTPUTLOG_H
