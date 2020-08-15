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

#include <QDialog>

namespace Ui {
class DlgImportTileSheet;
}

class DlgImportTileSheet : public QDialog
{
	Q_OBJECT

	Project &				m_ProjectRef;
	QPixmap *				m_pOrigPixmap;
	QVector<QPixmap *>		m_TilePixmaps;
	QPixmap *				m_pPreviewPixmap;

public:
	explicit DlgImportTileSheet(Project &projectRef, QWidget *parent = 0);
	~DlgImportTileSheet();

private Q_SLOTS:

	void on_buttonBox_accepted();

	void on_btnImageBrowse_clicked();

	void on_sbGridX_valueChanged(int iArg);
	void on_sbGridY_valueChanged(int iArg);
	void on_sbOffsetX_valueChanged(int iArg);
	void on_sbOffsetY_valueChanged(int iArg);
	void on_sbPaddingX_valueChanged(int iArg);
	void on_sbPaddingY_valueChanged(int iArg);

private:
	Ui::DlgImportTileSheet *ui;

	void AssemblePixmaps();

	void ErrorCheck();
};


#endif // DlgImportTileSheet_H
