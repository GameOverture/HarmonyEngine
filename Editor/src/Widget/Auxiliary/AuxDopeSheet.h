/**************************************************************************
 *	AuxDopeSheet.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef AuxDopeSheet_H
#define AuxDopeSheet_H

#include <QWidget>

class EntityModel;

namespace Ui {
class AuxDopeSheet;
}

class AuxDopeSheet : public QWidget
{
	Q_OBJECT

	EntityModel *			m_pEntityModel;

public:
	explicit AuxDopeSheet(QWidget *pParent = nullptr);
	/*virtual*/ ~AuxDopeSheet();

	void SetEntityModel(EntityModel *pEntModel);

private:
	Ui::AuxDopeSheet *ui;

private Q_SLOTS:
	void on_actionRewind_triggered();
	void on_actionPreviousKeyFrame_triggered();
	void on_actionPlayAnimations_triggered();
	void on_actionNextKeyFrame_triggered();
	void on_actionLastKeyFrame_triggered();
};

#endif // WIDGETOUTPUTLOG_H
