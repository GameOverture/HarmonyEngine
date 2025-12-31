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

	b2Filter						m_CachedB2Filter; // The "true" data is within the AtlasTileSet
	QUuid							m_SurfaceMaterialUuid;

	QList<TileData *>				m_FrameList;

public:
	WgtTileSetCollision(AuxTileSet *pAuxTileSet, QJsonObject initObj, QWidget *pParent = nullptr);
	virtual ~WgtTileSetCollision();
	
	virtual void OnInit(QJsonObject serializedObj) override;
	virtual QJsonObject SerializeCurrentWidgets() override;

	void SetOrderBtns(bool bUpEnabled, bool bDownEnabled);

protected:
	virtual QFrame *GetBorderFrame() const override;

private Q_SLOTS:
	void on_actionDelete_triggered();
	void on_actionUpward_triggered();
	void on_actionDownward_triggered();
	void on_txtName_editingFinished();
	void on_btnSetFilter_clicked();
	void on_btnSetMat_clicked();
	void on_chkIsSensor_toggled(bool bChecked);
};

#endif // WgtTileSetCollision_H
