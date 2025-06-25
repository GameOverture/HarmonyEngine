/**************************************************************************
*	WgtVectorSpinBox.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "WgtVectorSpinBox.h"
#include "ui_WgtVectorSpinBox.h"

WgtVectorSpinBox::WgtVectorSpinBox(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::WgtVectorSpinBox)
{
	ui->setupUi(this);
	// NOTE: WHEN THIS CONSTRUCTOR IS USED. Init() MUST BE CALLED
}

WgtVectorSpinBox::WgtVectorSpinBox(SpinBoxType eSpinBoxType, QVariant minValue, QVariant maxValue, QWidget *parent /*= nullptr*/) :
	QWidget(parent),
	ui(new Ui::WgtVectorSpinBox)
{
	ui->setupUi(this);
	Init(eSpinBoxType, minValue, maxValue);
}

/*virtual*/ WgtVectorSpinBox::~WgtVectorSpinBox()
{
	delete ui;
}

void WgtVectorSpinBox::Init(SpinBoxType eSpinBoxType, QVariant minValue, QVariant maxValue)
{
	ui->stackedWidget->setCurrentIndex(eSpinBoxType);
	switch(eSpinBoxType)
	{
	case SPINBOXTYPE_Int2d:
		ui->intSpinBoxX->setMinimum(minValue.toInt());
		ui->intSpinBoxY->setMinimum(minValue.toInt());
		ui->intSpinBoxX->setMaximum(maxValue.toInt());
		ui->intSpinBoxY->setMaximum(maxValue.toInt());

		connect(ui->intSpinBoxX, SIGNAL(valueChanged(int)), this, SLOT(OnValueChanged(int)));
		connect(ui->intSpinBoxY, SIGNAL(valueChanged(int)), this, SLOT(OnValueChanged(int)));

		ui->intSpinBoxX->setFocus();
		ui->intSpinBoxX->selectAll();
		break;

	case SPINBOXTYPE_Double2d:
		ui->doubleSpinBoxX->setMinimum(minValue.toDouble());
		ui->doubleSpinBoxY->setMinimum(minValue.toDouble());
		ui->doubleSpinBoxX->setMaximum(maxValue.toDouble());
		ui->doubleSpinBoxY->setMaximum(maxValue.toDouble());

		connect(ui->doubleSpinBoxX, SIGNAL(valueChanged(double)), this, SLOT(OnValueChanged(double)));
		connect(ui->doubleSpinBoxY, SIGNAL(valueChanged(double)), this, SLOT(OnValueChanged(double)));

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

		connect(ui->intSpinBox3dX, SIGNAL(valueChanged(int)), this, SLOT(OnValueChanged(int)));
		connect(ui->intSpinBox3dY, SIGNAL(valueChanged(int)), this, SLOT(OnValueChanged(int)));
		connect(ui->intSpinBox3dZ, SIGNAL(valueChanged(int)), this, SLOT(OnValueChanged(int)));

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

		connect(ui->doubleSpinBox3dX, SIGNAL(valueChanged(double)), this, SLOT(OnValueChanged(double)));
		connect(ui->doubleSpinBox3dY, SIGNAL(valueChanged(double)), this, SLOT(OnValueChanged(double)));
		connect(ui->doubleSpinBox3dZ, SIGNAL(valueChanged(double)), this, SLOT(OnValueChanged(double)));

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

		connect(ui->intSpinBox4dX, SIGNAL(valueChanged(int)), this, SLOT(OnValueChanged(int)));
		connect(ui->intSpinBox4dY, SIGNAL(valueChanged(int)), this, SLOT(OnValueChanged(int)));
		connect(ui->intSpinBox4dZ, SIGNAL(valueChanged(int)), this, SLOT(OnValueChanged(int)));
		connect(ui->intSpinBox4dW, SIGNAL(valueChanged(int)), this, SLOT(OnValueChanged(int)));

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

		connect(ui->doubleSpinBox4dX, SIGNAL(valueChanged(double)), this, SLOT(OnValueChanged(double)));
		connect(ui->doubleSpinBox4dY, SIGNAL(valueChanged(double)), this, SLOT(OnValueChanged(double)));
		connect(ui->doubleSpinBox4dZ, SIGNAL(valueChanged(double)), this, SLOT(OnValueChanged(double)));
		connect(ui->doubleSpinBox4dW, SIGNAL(valueChanged(double)), this, SLOT(OnValueChanged(double)));

		ui->doubleSpinBox4dX->setFocus();
		ui->doubleSpinBox4dX->selectAll();
		break;

	default:
		HyGuiLog("Unknown WgtVectorSpinBox type: " % QString::number(eSpinBoxType), LOGTYPE_Error);
		break;
	}
}

QVariant WgtVectorSpinBox::GetValue()
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
		HyGuiLog("Unknown WgtVectorSpinBox type: " % QString::number(ui->stackedWidget->currentIndex()), LOGTYPE_Error);
		return QVariant();
	}
}

void WgtVectorSpinBox::SetValue(QVariant data)
{
	switch(ui->stackedWidget->currentIndex())
	{
	case SPINBOXTYPE_Int2d:
		if(data.type() != QVariant::Point && data.type() != QVariant::Size)
		{
			HyGuiLog("WgtVectorSpinBox::SetValue() data type is not a QPoint or QSize", LOGTYPE_Error);
			return;
		}

		if(data.type() == QVariant::Point)
		{
			ui->intSpinBoxX->setValue(data.toPoint().x());
			ui->intSpinBoxY->setValue(data.toPoint().y());
		}
		else if(data.type() == QVariant::Size)
		{
			ui->intSpinBoxX->setValue(data.toPoint().x());
			ui->intSpinBoxY->setValue(data.toPoint().y());
		}
		break;

	case SPINBOXTYPE_Double2d:
		if(data.type() != QVariant::PointF && data.type() != QVariant::SizeF)
		{
			HyGuiLog("WgtVectorSpinBox::SetValue() data type is not a QPointF or QSizeF", LOGTYPE_Error);
			return;
		}

		if(data.type() == QVariant::PointF)
		{
			ui->doubleSpinBoxX->setValue(data.toPointF().x());
			ui->doubleSpinBoxY->setValue(data.toPointF().y());
		}
		else if(data.type() == QVariant::SizeF)
		{
			ui->doubleSpinBoxX->setValue(data.toSizeF().width());
			ui->doubleSpinBoxY->setValue(data.toSizeF().height());
		}
		break;

	case SPINBOXTYPE_Int3d:
		if(data.type() != QVariant::Rect)
		{
			HyGuiLog("WgtVectorSpinBox::SetValue() data type is not a QRect", LOGTYPE_Error);
			return;
		}
		ui->intSpinBox3dX->setValue(data.toRect().left());
		ui->intSpinBox3dY->setValue(data.toRect().top());
		ui->intSpinBox3dZ->setValue(data.toRect().width());
		break;

	case SPINBOXTYPE_Double3d:
		if(data.type() != QVariant::RectF)
		{
			HyGuiLog("WgtVectorSpinBox::SetValue() data type is not a QRectF", LOGTYPE_Error);
			return;
		}
		ui->doubleSpinBox3dX->setValue(data.toRectF().left());
		ui->doubleSpinBox3dY->setValue(data.toRectF().top());
		ui->doubleSpinBox3dZ->setValue(data.toRectF().width());
		break;

	case SPINBOXTYPE_Int4d:
		if(data.type() != QVariant::Rect)
		{
			HyGuiLog("WgtVectorSpinBox::SetValue() data type is not a QRect", LOGTYPE_Error);
			return;
		}
		ui->intSpinBox4dX->setValue(data.toRect().left());
		ui->intSpinBox4dY->setValue(data.toRect().top());
		ui->intSpinBox4dZ->setValue(data.toRect().width());
		ui->intSpinBox4dW->setValue(data.toRect().height());
		break;

	case SPINBOXTYPE_Double4d:
		if(data.type() != QVariant::RectF)
		{
			HyGuiLog("WgtVectorSpinBox::SetValue() data type is not a QRectF", LOGTYPE_Error);
			return;
		}
		ui->doubleSpinBox4dX->setValue(data.toRectF().left());
		ui->doubleSpinBox4dY->setValue(data.toRectF().top());
		ui->doubleSpinBox4dZ->setValue(data.toRectF().width());
		ui->doubleSpinBox4dW->setValue(data.toRectF().height());
		break;
	}

	Q_EMIT ValueChanged(data);
}

void WgtVectorSpinBox::OnValueChanged(int iValue)
{
	QVariant data = GetValue();
	Q_EMIT ValueChanged(data);
}

void WgtVectorSpinBox::OnValueChanged(double dValue)
{
	QVariant data = GetValue();
	Q_EMIT ValueChanged(data);
}

/*virtual*/ void WgtVectorSpinBox::focusInEvent(QFocusEvent *pEvent) /*override*/
{
	QWidget::focusInEvent(pEvent);
	switch(ui->stackedWidget->currentIndex())
	{
	case SPINBOXTYPE_Int2d:
		ui->intSpinBoxX->setFocus(Qt::TabFocusReason);
		ui->intSpinBoxX->selectAll();
		break;
	case SPINBOXTYPE_Double2d:
		ui->doubleSpinBoxX->setFocus(Qt::TabFocusReason);
		ui->doubleSpinBoxX->selectAll();
		break;
	case SPINBOXTYPE_Int3d:
		ui->intSpinBox3dX->setFocus(Qt::TabFocusReason);
		ui->intSpinBox3dX->selectAll();
		break;
	case SPINBOXTYPE_Double3d:
		ui->doubleSpinBox3dX->setFocus(Qt::TabFocusReason);
		ui->doubleSpinBox3dX->selectAll();
		break;
	case SPINBOXTYPE_Int4d:
		ui->intSpinBox4dX->setFocus(Qt::TabFocusReason);
		ui->intSpinBox4dX->selectAll();
		break;
	case SPINBOXTYPE_Double4d:
		ui->doubleSpinBox4dX->setFocus(Qt::TabFocusReason);
		ui->doubleSpinBox4dX->selectAll();
		break;
	}
}
