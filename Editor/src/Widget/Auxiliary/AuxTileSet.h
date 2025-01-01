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
	

public:
	explicit AuxTileSet(QWidget *pParent = nullptr);
	virtual ~AuxTileSet();

	void Init(AtlasTileSet *pTileSet);

private:
	Ui::AuxTileSet *ui;

	void SetImportWidgets();
	void SliceSheetPixmaps();
	void ErrorCheckImport();

private Q_SLOTS:
	void on_radTileSheet_toggled(bool bChecked);
	void on_radTileImages_toggled(bool bChecked);

	void on_btnImageBrowse_clicked();

	void OnTileSizeChanged(QVariant newSize);
	void OnStartOffsetChanged(QVariant newOffset);
	void OnPaddingChanged(QVariant newPadding);

	void on_btnConfirmAddRemove_clicked();

};

#endif // WIDGETOUTPUTLOG_H