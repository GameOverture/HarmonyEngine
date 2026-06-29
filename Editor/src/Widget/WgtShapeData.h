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

#include "VectorModel.h"

#include <QWidget>
#include <QSpinBox>
#include <QMouseEvent>

class EntityTreeItemData;
class EntityWidget;

namespace Ui {
class WgtShapeData;
}

class WgtShapeData : public QWidget
{
	Q_OBJECT

	enum ShapeDataType
	{
		// This is also the QStackedWidget's page index
		SHAPEDATATYPE_Primitive = 0,
		SHAPEDATATYPE_Shape,
		SHAPEDATATYPE_Chain,
	};

	EntityTreeItemData *m_pTreeItemData;

public:
	explicit WgtShapeData(QWidget *parent = 0);
	WgtShapeData(EntityTreeItemData *pTreeItemData, QWidget *parent = nullptr);
	virtual ~WgtShapeData();

	void Init(EntityTreeItemData *pTreeItemData);
	void UpdateModel(bool bIsActiveEditModeItem, EditModeType eEditModeType, EditorShape eShapeType);

	QVariant GetValue();

public Q_SLOTS:
	void SetValue(bool bIsActiveEditModeItem, QVariant data);
	
protected Q_SLOTS:
	void OnValueChanged(int iValue);
	void OnValueChanged(double dValue);

	void on_cmbPrimType_currentIndexChanged(int iIndex);
	void on_sbPrimOutline_valueChanged(double dValue);

	void on_cmbShapeType_currentIndexChanged(int iIndex);

Q_SIGNALS:
	void ValueChanged(QVariant newValue);

private:
	Ui::WgtShapeData *ui;
};

#endif // WgtShapeData_H
