/**************************************************************************
*	PropertiesTreeItem.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef PROPERTIESTREEITEM_H
#define PROPERTIESTREEITEM_H

#include "IModelTreeItem.h"

#include <QWidget>
#include <QVariant>

class PropertiesTreeModel;

enum PropertiesType
{
	PROPERTIESTYPE_Root = 0,
	PROPERTIESTYPE_Category,
	PROPERTIESTYPE_CategoryChecked,

	PROPERTIESTYPE_bool,
	PROPERTIESTYPE_int,
	PROPERTIESTYPE_double,          // delegateBuilder [int] = QDoubleSpinBox's decimals value. Aka percision.
	PROPERTIESTYPE_ivec2,
	PROPERTIESTYPE_vec2,
	PROPERTIESTYPE_LineEdit,
	PROPERTIESTYPE_ComboBox,        // delegateBuilder [QStringList] = QComboBox's selection list
	PROPERTIESTYPE_StatesComboBox,  // delegateBuilder [ProjectItem *] = A pointer to the ProjectItem that owns this property
	PROPERTIESTYPE_Slider,
	PROPERTIESTYPE_SpriteFrames
};

struct PropertiesDef
{
	PropertiesType  eType;

	QVariant        defaultData;
	QVariant        minRange;
	QVariant        maxRange;
	QVariant        stepAmt;
	QString         sPrefix;
	QString         sSuffix;

	QVariant        delegateBuilder; // Some types need an additional QVariant to build their delegate widget (e.g. ComboBox uses defaultData as currently selected index, but also needs a string list to select from)

	PropertiesDef()
	{ }

	// Convience constructors
	PropertiesDef(PropertiesType eType_,
				  QVariant defaultData_) :  eType(eType_),
											defaultData(defaultData_)
	{ }
	PropertiesDef(PropertiesType eType_,
				  QVariant defaultData_,
				  QVariant minRange_,
				  QVariant maxRange_,
				  QVariant stepAmt_,
				  QString sPrefix_,
				  QString sSuffix_,
				  QVariant delegateBuilder_ = QVariant()) : eType(eType_),
															defaultData(defaultData_),
															minRange(minRange_),
															maxRange(maxRange_),
															stepAmt(stepAmt_),
															sPrefix(sPrefix_),
															sSuffix(sSuffix_),
															delegateBuilder(delegateBuilder_)
	{ }
};

class PropertiesTreeItem : public IModelTreeItem
{
protected:
	const QString								m_sNAME;
	const bool									m_bREAD_ONLY;
	PropertiesTreeModel *						m_pTreeModel;

	QVariant									m_Data;
	PropertiesDef								m_DataDef;

	QColor										m_Color;
	QString										m_sToolTip;

public:
	explicit PropertiesTreeItem(QString sName, PropertiesTreeModel *pTreeModel, const PropertiesDef &propertiesDef, QColor color, QString sToolTip, bool bReadOnly);
	virtual ~PropertiesTreeItem();

	bool IsCategory() const;
	bool IsReadOnly() const;

	PropertiesType GetType() const;
	QString GetName() const;
	QString GetValue() const;

	QVariant GetData() const;
	const PropertiesDef &GetDataDef() const;

	QColor GetColor() const;

	void SetData(const QVariant &newData);

	virtual QString GetToolTip() const override;
};

#endif // PROPERTIESTREEITEM_H
