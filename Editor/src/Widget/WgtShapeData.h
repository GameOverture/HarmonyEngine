/**************************************************************************
*	WgtShapeData.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef WgtShapeData_H
#define WgtShapeData_H

#include <QWidget>
#include <QSpinBox>
#include <QMouseEvent>

namespace Ui {
class WgtShapeData;
}

enum ShapeDataType
{
	// This is also the QStackedWidget's page index
	SHAPEDATATYPE_Primitive = 0,
	SHAPEDATATYPE_Shape,
	SHAPEDATATYPE_Chain,
};

class WgtShapeData : public QWidget
{
	Q_OBJECT

	QJsonArray			m_DataArray;

public:
	explicit WgtShapeData(QWidget *parent = 0);
	WgtShapeData(ShapeDataType eShapeDataType, QJsonObject serializedObj, QWidget *parent = nullptr);
	virtual ~WgtShapeData();

	void Init(ShapeDataType eShapeDataType, QJsonObject serializedObj); // Required when using default ctor

	QVariant GetValue();

public Q_SLOTS:
	void SetValue(QVariant data);
	
protected Q_SLOTS:
	void OnValueChanged(int iValue);
	void OnValueChanged(double dValue);

Q_SIGNALS:
	void ValueChanged(QVariant newValue);

private:
	Ui::WgtShapeData *ui;
};

#endif // WgtShapeData_H
