/**************************************************************************
*	WidgetVectorSpinBox.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "WidgetVectorSpinBox.h"
#include "ui_WidgetVectorSpinBox.h"

WidgetVectorSpinBox::WidgetVectorSpinBox(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::WidgetVectorSpinBox)
{
	ui->setupUi(this);

	// NOTE: THIS CONSTRUCTOR IS INVALID TO USE. IT EXISTS FOR QT TO ALLOW Q_OBJECT TO WORK
	HyGuiLog("WidgetVectorSpinBox::WidgetVectorSpinBox() invalid constructor used", LOGTYPE_Error);
}

WidgetVectorSpinBox::WidgetVectorSpinBox(SpinBoxType eSpinBoxType, QWidget *parent /*= nullptr*/) :
	QWidget(parent),
	ui(new Ui::WidgetVectorSpinBox)
{
	ui->setupUi(this);

	ui->stackedWidget->setCurrentIndex(eSpinBoxType);
}

/*virtual*/ WidgetVectorSpinBox::~WidgetVectorSpinBox()
{
	delete ui;
}

QVariant WidgetVectorSpinBox::GetValue()
{
	switch(ui->stackedWidget->currentIndex())
	{
	case SPINBOXTYPE_Int2d:
		return QVariant(QPoint(ui->intSpinBoxX->value(), ui->intSpinBoxY->value()));
		
	case SPINBOXTYPE_Double2d:
		return QVariant(QPointF(ui->doubleSpinBoxX->value(), ui->doubleSpinBoxY->value()));

	case SPINBOXTYPE_Int3d:
		return QVariant(QRect(ui->intSpinBox3dX->value(), ui->intSpinBox3dY->value(), ui->intSpinBox3dZ->value(), 0));

	case SPINBOXTYPE_Double3d:
		return QVariant(QRectF(ui->doubleSpinBox3dX->value(), ui->doubleSpinBox3dY->value(), ui->doubleSpinBox3dZ->value(), 0));

	case SPINBOXTYPE_Int4d:
		return QVariant(QRect(ui->intSpinBox4dX->value(), ui->intSpinBox4dY->value(), ui->intSpinBox4dZ->value(), ui->intSpinBox4dW->value()));

	case SPINBOXTYPE_Double4d:
		return QVariant(QRectF(ui->doubleSpinBox4dX->value(), ui->doubleSpinBox4dY->value(), ui->doubleSpinBox4dZ->value(), ui->doubleSpinBox4dW->value()));

	default:
		HyGuiLog("Unknown WidgetVectorSpinBox type: " % QString::number(ui->stackedWidget->currentIndex()), LOGTYPE_Error);
		return QVariant();
	}
}

void WidgetVectorSpinBox::SetValue(QVariant data)
{
	switch(ui->stackedWidget->currentIndex())
	{
	case SPINBOXTYPE_Int2d:
		ui->intSpinBoxX->setValue(data.toPoint().x());
		ui->intSpinBoxY->setValue(data.toPoint().y());
		break;

	case SPINBOXTYPE_Double2d:
		ui->doubleSpinBoxX->setValue(data.toPointF().x());
		ui->doubleSpinBoxY->setValue(data.toPointF().y());
		break;

	case SPINBOXTYPE_Int3d:
		ui->intSpinBox3dX->setValue(data.toRect().left());
		ui->intSpinBox3dY->setValue(data.toRect().top());
		ui->intSpinBox3dZ->setValue(data.toRect().width());
		break;

	case SPINBOXTYPE_Double3d:
		ui->doubleSpinBox3dX->setValue(data.toRectF().left());
		ui->doubleSpinBox3dY->setValue(data.toRectF().top());
		ui->doubleSpinBox3dZ->setValue(data.toRectF().width());
		break;

	case SPINBOXTYPE_Int4d:
		ui->intSpinBox4dX->setValue(data.toRect().left());
		ui->intSpinBox4dY->setValue(data.toRect().top());
		ui->intSpinBox4dZ->setValue(data.toRect().width());
		ui->intSpinBox4dW->setValue(data.toRect().height());
		break;

	case SPINBOXTYPE_Double4d:
		ui->doubleSpinBox4dX->setValue(data.toRectF().left());
		ui->doubleSpinBox4dY->setValue(data.toRectF().top());
		ui->doubleSpinBox4dZ->setValue(data.toRectF().width());
		ui->doubleSpinBox4dW->setValue(data.toRectF().height());
		break;
	}
}
