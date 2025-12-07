/**************************************************************************
 *	DlgSliceSpriteSheet.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DlgSliceSpriteSheet_H
#define DlgSliceSpriteSheet_H

#include "Global.h"

#include <QDialog>

namespace Ui {
class DlgSliceSpriteSheet;
}

class QGraphicsScene;
class QPixmap;
class QGraphicsPixmapItem;

class DlgSliceSpriteSheet : public QDialog
{
	Q_OBJECT

	struct Sheet
	{
		QString				sBaseAssetName;
		QPixmap				sheetPixmap;
		QVector<QPixmap>	framePixmapList;

		Sheet(QString sImagePath) :
			sBaseAssetName(QFileInfo(sImagePath).completeBaseName()),
			sheetPixmap(sImagePath)
		{ }
	};
	QVector<Sheet>			m_SheetList;
	int						m_iCurrentSheetIndex;

	QGraphicsScene *		m_pPreviewScene;
	QPixmap *				m_pPreviewPixmap;
	QGraphicsPixmapItem *	m_pPreviewPixmapItem;

public:
	explicit DlgSliceSpriteSheet(QWidget *parent = 0);
	~DlgSliceSpriteSheet();

	void Clear();

private Q_SLOTS:
	void on_btnImageBrowse_clicked();

	void on_txtMainFilter_textChanged(const QString &arg1);

	void on_sbTileSizeX_valueChanged(int iArg);
	void on_sbTileSizeY_valueChanged(int iArg);
	void on_sbOffsetX_valueChanged(int iArg);
	void on_sbOffsetY_valueChanged(int iArg);
	void on_sbPaddingX_valueChanged(int iArg);
	void on_sbPaddingY_valueChanged(int iArg);

	void on_txtAssetsBaseName_textChanged(const QString &arg1);
	void on_actionPreviewNext_triggered();
	void on_actionPreviewBack_triggered();

	void on_buttonBox_accepted();

private:
	Ui::DlgSliceSpriteSheet *ui;

	void AssemblePixmaps(int iSheetIndex);

	void ErrorCheck();
};


#endif // DlgSliceSpriteSheet_H
