/**************************************************************************
*	WidgetVectorSpinBox.cpp
*
*	Harmony Engine - Designer Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Designer Tool License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "WidgetVectorSpinBox.h"
#include "ui_WidgetVectorSpinBox.h"

#include "Global.h"

WidgetVectorSpinBox::WidgetVectorSpinBox(QWidget *parent) : QWidget(parent),
															ui(new Ui::WidgetVectorSpinBox)
{
	ui->setupUi(this);

	// NOTE: THIS CONSTRUCTOR IS INVALID TO USE. IT EXISTS FOR QT TO ALLOW Q_OBJECT TO WORK
	HyGuiLog("WidgetVectorSpinBox::WidgetVectorSpinBox() invalid constructor used", LOGTYPE_Error);
}

WidgetVectorSpinBox::WidgetVectorSpinBox(bool bIsIntVector, QWidget *parent) :  QWidget(parent),
																				ui(new Ui::WidgetVectorSpinBox)
{
	ui->setupUi(this);

	ui->stackedWidget->setCurrentIndex(bIsIntVector ? PAGE_Int : PAGE_Double);
}

/*virtual*/ WidgetVectorSpinBox::~WidgetVectorSpinBox()
{
	delete ui;
}

QVariant WidgetVectorSpinBox::GetValue()
{
	if(ui->stackedWidget->currentIndex() == PAGE_Int)
		return QVariant(QPoint(ui->intSpinBoxX->value(), ui->intSpinBoxY->value()));
	else
		return QVariant(QPointF(ui->doubleSpinBoxX->value(), ui->doubleSpinBoxY->value()));
}

void WidgetVectorSpinBox::SetValue(QVariant data)
{
	if(ui->stackedWidget->currentIndex() == PAGE_Int)
	{
		ui->intSpinBoxX->setValue(data.toPoint().x());
		ui->intSpinBoxY->setValue(data.toPoint().y());
	}
	else
	{
		ui->doubleSpinBoxX->setValue(data.toPointF().x());
		ui->doubleSpinBoxY->setValue(data.toPointF().y());
	}
}
