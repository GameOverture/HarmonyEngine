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
	// NOTE: WHEN THIS CONSTRUCTOR IS USED. Init() MUST BE CALLED
}

WidgetVectorSpinBox::WidgetVectorSpinBox(SpinBoxType eSpinBoxType, QVariant minValue, QVariant maxValue, QWidget *parent /*= nullptr*/) :
	QWidget(parent),
	ui(new Ui::WidgetVectorSpinBox)
{
	ui->setupUi(this);
	Init(eSpinBoxType, minValue, maxValue);
}

/*virtual*/ WidgetVectorSpinBox::~WidgetVectorSpinBox()
{
	delete ui;
}

void WidgetVectorSpinBox::Init(SpinBoxType eSpinBoxType, QVariant minValue, QVariant maxValue)
{
	ui->stackedWidget->setCurrentIndex(eSpinBoxType);
	switch(eSpinBoxType)
	{
	case SPINBOXTYPE_Int2d:
		ui->intSpinBoxX->setMinimum(minValue.toInt());
		ui->intSpinBoxY->setMinimum(minValue.toInt());
		ui->intSpinBoxX->setMaximum(maxValue.toInt());
		ui->intSpinBoxY->setMaximum(maxValue.toInt());

		ui->intSpinBoxX->setFocus();
		ui->intSpinBoxX->selectAll();
		break;

	case SPINBOXTYPE_Double2d:
		ui->doubleSpinBoxX->setMinimum(minValue.toDouble());
		ui->doubleSpinBoxY->setMinimum(minValue.toDouble());
		ui->doubleSpinBoxX->setMaximum(maxValue.toDouble());
		ui->doubleSpinBoxY->setMaximum(maxValue.toDouble());

		ui->doubleSpinBoxX->setFocus();
		ui->doubleSpinBoxX->selectAll();
		break;

	case SPINBOXTYPE_Int3d:
		ui->intSpinBox3dX->setMinimum(minValue.toInt());
		ui->intSpinBox3dY->setMinimum(minValue.toInt());
		ui->intSpinBox3dZ->setMinimum(minValue.toInt());
		ui->intSpinBox3dX->setMaximum(maxValue.toInt());
		ui->intSpinBox3dY->setMaximum(maxValue.toInt());
		ui->intSpinBox3dZ->setMaximum(maxValue.toInt());

		ui->intSpinBox3dX->setFocus();
		ui->intSpinBox3dX->selectAll();
		break;

	case SPINBOXTYPE_Double3d:
		ui->doubleSpinBox3dX->setMinimum(minValue.toDouble());
		ui->doubleSpinBox3dY->setMinimum(minValue.toDouble());
		ui->doubleSpinBox3dZ->setMinimum(minValue.toDouble());
		ui->doubleSpinBox3dX->setMaximum(maxValue.toDouble());
		ui->doubleSpinBox3dY->setMaximum(maxValue.toDouble());
		ui->doubleSpinBox3dZ->setMaximum(maxValue.toDouble());

		ui->doubleSpinBox3dX->setFocus();
		ui->doubleSpinBox3dX->selectAll();
		break;

	case SPINBOXTYPE_Int4d:
		ui->intSpinBox4dX->setMinimum(minValue.toInt());
		ui->intSpinBox4dY->setMinimum(minValue.toInt());
		ui->intSpinBox4dZ->setMinimum(minValue.toInt());
		ui->intSpinBox4dW->setMinimum(minValue.toInt());
		ui->intSpinBox4dX->setMaximum(maxValue.toInt());
		ui->intSpinBox4dY->setMaximum(maxValue.toInt());
		ui->intSpinBox4dZ->setMaximum(maxValue.toInt());
		ui->intSpinBox4dW->setMaximum(maxValue.toInt());

		ui->intSpinBox4dX->setFocus();
		ui->intSpinBox4dX->selectAll();
		break;

	case SPINBOXTYPE_Double4d:
		ui->doubleSpinBox4dX->setMinimum(minValue.toDouble());
		ui->doubleSpinBox4dY->setMinimum(minValue.toDouble());
		ui->doubleSpinBox4dZ->setMinimum(minValue.toDouble());
		ui->doubleSpinBox4dW->setMinimum(minValue.toDouble());
		ui->doubleSpinBox4dX->setMaximum(maxValue.toDouble());
		ui->doubleSpinBox4dY->setMaximum(maxValue.toDouble());
		ui->doubleSpinBox4dZ->setMaximum(maxValue.toDouble());
		ui->doubleSpinBox4dW->setMaximum(maxValue.toDouble());

		ui->doubleSpinBox4dX->setFocus();
		ui->doubleSpinBox4dX->selectAll();
		break;

	default:
		HyGuiLog("Unknown WidgetVectorSpinBox type: " % QString::number(eSpinBoxType), LOGTYPE_Error);
		break;
	}
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
