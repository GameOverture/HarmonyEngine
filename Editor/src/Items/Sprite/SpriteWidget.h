/**************************************************************************
 *	SpriteWidget.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef SPRITEWIDGET_H
#define SPRITEWIDGET_H

#include "AtlasFrame.h"
#include "SpriteModels.h"
#include "IWidget.h"

#include <QWidget>
#include <QMenu>
#include <QComboBox>

namespace Ui {
class SpriteWidget;
}

class AtlasWidget;

class SpriteWidget : public IWidget
{
	Q_OBJECT

	bool					m_bPlayActive;
	float					m_fElapsedTime;
	bool					m_bIsBounced;

public:
	explicit SpriteWidget(ProjectItemData &itemRef, QWidget *pParent = nullptr);
	virtual ~SpriteWidget();
	
	virtual void OnGiveMenuActions(QMenu *pMenu) override;
	virtual void OnUpdateActions() override;
	virtual void OnFocusState(int iStateIndex, QVariant subState) override;

	bool IsPlayingAnim();

	void SetSelectedFrame(int iFrameIndex);
	void StopPlayingAnim();
	void GetSpriteInfo(int &iStateIndexOut, int &iFrameIndexOut);
	QPoint GetSelectedFrameOffset();

	void ApplyTranslate(QPoint ptPosition, bool bApplyAsOffset);

	void RefreshFrameRateSpinBox();
	
private Q_SLOTS:
	void on_framesView_selectionChanged(const QItemSelection &newSelection, const QItemSelection &oldSelection);

	void on_actionAlignLeft_triggered();
	
	void on_actionAlignRight_triggered();
	
	void on_actionAlignUp_triggered();
	
	void on_actionAlignDown_triggered();
	
	void on_actionAlignCenterVertical_triggered();
	
	void on_actionAlignCenterHorizontal_triggered();

	void on_actionImportFrames_triggered();

	void on_actionRemoveFrames_triggered();

	void on_actionRemoveAllFrames_triggered();

	void on_actionOrderFrameUpwards_triggered();

	void on_actionOrderFrameDownwards_triggered();

	void on_actionPlay_triggered();

	void on_actionFirstFrame_triggered();

	void on_actionLastFrame_triggered();

	void OnKeyShiftQ();
	void OnKeyShiftE();

	void on_chkReverse_clicked();

	void on_chkLoop_clicked();

	void on_chkBounce_clicked();

	void on_btnHz10_clicked();

	void on_btnHz20_clicked();

	void on_btnHz30_clicked();

	void on_btnHz40_clicked();

	void on_btnHz50_clicked();

	void on_btnHz60_clicked();

	void on_sbFrameRate_valueChanged(double dValue);

private:
	Ui::SpriteWidget *ui;
};

#endif // SPRITEWIDGET_H
