/**************************************************************************
 *	WgtTileSetCollision.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WgtTileSetCollision_H
#define WgtTileSetCollision_H

#include "Global.h"
#include "IWgtTileSetItem.h"

namespace Ui {
class WgtTileSetCollision;
}

class TileData;

class WgtTileSetCollision : public IWgtTileSetItem
{
	Q_OBJECT

	Ui::WgtTileSetCollision *		ui;

	bool							m_bPaintingTiles;
	QList<TileData *>				m_FrameList;

public:
	WgtTileSetCollision(AuxTileSet *pAuxTileSet, QJsonObject initObj, QWidget *pParent = nullptr);
	virtual ~WgtTileSetCollision();
	
	virtual void OnInit(QJsonObject serializedObj) override;
	virtual QJsonObject SerializeCurrentWidgets() override;

	void SetOrderBtns(bool bUpEnabled, bool bDownEnabled);

	QString GetName() const;
	bool IsPaintingTiles() const;

protected:
	virtual QFrame *GetBorderFrame() const override;

private Q_SLOTS:
	void on_actionDelete_triggered();
	void on_actionUpward_triggered();
	void on_actionDownward_triggered();
	void on_txtName_editingFinished();
	void on_btnColor_clicked();
	void on_btnFramePreview_clicked();
	void on_btnHz10_clicked();
	void on_btnHz20_clicked();
	void on_btnHz30_clicked();
	void on_btnHz40_clicked();
	void on_btnHz60_clicked();
	void on_sbFrameRate_valueChanged(double dNewValue);
	void on_chkStartRandom_toggled(bool bChecked);
};

#endif // WgtTileSetCollision_H
