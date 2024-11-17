/**************************************************************************
 *	DlgImportTileSheet.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DlgImportTileSheet_H
#define DlgImportTileSheet_H

#include "AtlasTileSet.h"

#include <QDialog>

namespace Ui {
class DlgImportTileSheet;
}

class DlgImportTileSheet : public QDialog
{
	Q_OBJECT

	QSize					m_DefaultTileSize;
	AtlasTileSet &			m_TileSetRef;


	QPixmap *				m_pOrigPixmap;
	QVector<QPixmap *>		m_TilePixmaps;
	QPixmap *				m_pPreviewPixmap;

	bool					m_bIsShowingTileSheet;

public:
	explicit DlgImportTileSheet(AtlasTileSet &tileSetRef, QWidget *parent = 0);
	~DlgImportTileSheet();

private Q_SLOTS:

	void on_radTileSheet_toggled(bool bChecked);
	void on_radTileImages_toggled(bool bChecked);

	void on_btnImageBrowse_clicked();

	void on_sbTileSizeX_valueChanged(int iArg);
	void on_sbTileSizeY_valueChanged(int iArg);
	void on_sbOffsetX_valueChanged(int iArg);
	void on_sbOffsetY_valueChanged(int iArg);
	void on_sbPaddingX_valueChanged(int iArg);
	void on_sbPaddingY_valueChanged(int iArg);

	void on_buttonBox_accepted();

private:
	Ui::DlgImportTileSheet *ui;

	void SetWidgets(bool bTileSheet);

	void AssemblePixmaps();

	void ErrorCheck();
};


#endif // DlgImportTileSheet_H
