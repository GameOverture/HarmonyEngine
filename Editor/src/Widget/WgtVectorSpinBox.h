/**************************************************************************
*	WgtVectorSpinBox.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef WGTVECTORSPINBOX_H
#define WGTVECTORSPINBOX_H

#include <QWidget>
#include <QSpinBox>
#include <QMouseEvent>

namespace Ui {
class WgtVectorSpinBox;
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

class WgtIntSpinBox : public QSpinBox
{
	Q_OBJECT

public:
	explicit WgtIntSpinBox(QWidget *pParent = 0) :
		QSpinBox(pParent)
	{ }
	virtual ~WgtIntSpinBox() { }
};
class WgtDoubleSpinBox : public QDoubleSpinBox
{
	Q_OBJECT

public:
	explicit WgtDoubleSpinBox(QWidget *pParent = 0) :
		QDoubleSpinBox(pParent)
	{ }
	virtual ~WgtDoubleSpinBox() { }
};

class WgtVectorSpinBox : public QWidget
{
	Q_OBJECT

public:
	explicit WgtVectorSpinBox(QWidget *parent = 0);
	WgtVectorSpinBox(SpinBoxType eSpinBoxType, QVariant minValue, QVariant maxValue, QWidget *parent = nullptr);
	virtual ~WgtVectorSpinBox();

	void Init(SpinBoxType eSpinBoxType, QVariant minValue, QVariant maxValue); // Required when using default ctor

	QVariant GetValue();

public Q_SLOTS:
	void SetValue(QVariant data);
	
protected Q_SLOTS:
	void OnValueChanged(int iValue);
	void OnValueChanged(double dValue);

Q_SIGNALS:
	void ValueChanged(QVariant newValue);

protected:
	virtual void focusInEvent(QFocusEvent *pEvent) override;

private:
	Ui::WgtVectorSpinBox *ui;
};

#endif // WGTVECTORSPINBOX_H
