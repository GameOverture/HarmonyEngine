/**************************************************************************
 *	WgtTileSetAnimation.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WgtTileSetAnimation_H
#define WgtTileSetAnimation_H

#include "Global.h"
#include "IWgtTileSetItem.h"

#include <QTimer>

namespace Ui {
class WgtTileSetAnimation;
}

class TileData;

class WgtTileSetAnimation : public IWgtTileSetItem
{
	Q_OBJECT

	Ui::WgtTileSetAnimation *		ui;

	QList<quint32>					m_TileChecksumList;	// Each frame's checksum
	QTimer *						m_pPreviewTimer;
	int								m_iPreviewFrameIndex;

public:
	WgtTileSetAnimation(AuxTileSet *pAuxTileSet, QJsonObject initObj, QWidget *pParent = nullptr);
	virtual ~WgtTileSetAnimation();
	
	virtual void OnInit(QJsonObject serializedObj) override;
	virtual QJsonObject SerializeCurrentWidgets() override;

	void SetOrderBtns(bool bUpEnabled, bool bDownEnabled);

	QString GetName() const;

protected:
	virtual QFrame *GetBorderFrame() const override;

	void RefreshPreview();

protected Q_SLOTS:
	void OnPreviewUpdate();

private Q_SLOTS:
	void on_actionDelete_triggered();
	void on_actionUpward_triggered();
	void on_actionDownward_triggered();
	void on_txtName_editingFinished();
	void on_btnColor_clicked();
	void on_sbNumFrames_valueChanged(int iNewValue);
	void on_sbSeparation_valueChanged(int iNewValue);
	void on_btnHz1_clicked();
	void on_btnHz10_clicked();
	void on_btnHz20_clicked();
	void on_btnHz30_clicked();
	void on_btnHz40_clicked();
	void on_btnHz50_clicked();
	void on_btnHz60_clicked();
	void on_sbFrameDuration_valueChanged(int iNewValue);
	void on_chkEnabled_toggled(bool bChecked);
	void on_chkGlobalSync_toggled(bool bChecked);
	void on_chkLoop_toggled(bool bChecked);
	void on_chkReverse_toggled(bool bChecked);
	void on_chkBounce_toggled(bool bChecked);
};

#endif // WgtTileSetAnimation_H
