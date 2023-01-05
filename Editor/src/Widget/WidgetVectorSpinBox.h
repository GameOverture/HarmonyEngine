/**************************************************************************
*	WidgetVectorSpinBox.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef WIDGETVECTORSPINBOX_H
#define WIDGETVECTORSPINBOX_H

#include <QWidget>

namespace Ui {
class WidgetVectorSpinBox;
}

enum SpinBoxType
{
	// This is also the QStackedWidget's page index
	SPINBOXTYPE_Int2d = 0,
	SPINBOXTYPE_Double2d,
	SPINBOXTYPE_Int3d,
	SPINBOXTYPE_Double3d,
	SPINBOXTYPE_Int4d,
	SPINBOXTYPE_Double4d,
};

class WidgetVectorSpinBox : public QWidget
{
	Q_OBJECT

public:
	explicit WidgetVectorSpinBox(QWidget *parent = 0);
	WidgetVectorSpinBox(SpinBoxType eSpinBoxType, QVariant minValue, QVariant maxValue, QWidget *parent = nullptr);
	virtual ~WidgetVectorSpinBox();

	void Init(SpinBoxType eSpinBoxType, QVariant minValue, QVariant maxValue); // Required when using default ctor

	QVariant GetValue();
	void SetValue(QVariant data);

private:
	Ui::WidgetVectorSpinBox *ui;
};

#endif // WIDGETVECTORSPINBOX_H
