/**************************************************************************
*	PropertiesTreeModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef PROPERTIESTREEMODEL_H
#define PROPERTIESTREEMODEL_H

#include "Global.h"
#include "PropertiesTreeView.h"
#include "ProjectItem.h"
#include "Shared/TreeModel/ITreeModel.h"

enum PropertiesType
{
	PROPERTIESTYPE_Root = 0,
	PROPERTIESTYPE_Category,
	PROPERTIESTYPE_CategoryChecked,

	PROPERTIESTYPE_bool,
	PROPERTIESTYPE_int,
	PROPERTIESTYPE_double,          // delegateBuilder [int] = QDoubleSpinBox's decimals value. Aka precision.
	PROPERTIESTYPE_ivec2,
	PROPERTIESTYPE_vec2,
	PROPERTIESTYPE_ivec3,
	PROPERTIESTYPE_vec3,
	PROPERTIESTYPE_ivec4,
	PROPERTIESTYPE_vec4,
	PROPERTIESTYPE_LineEdit,
	PROPERTIESTYPE_ComboBox,        // delegateBuilder [QStringList] = QComboBox's selection list
	PROPERTIESTYPE_StatesComboBox,  // delegateBuilder [ProjectItem *] = A pointer to the ProjectItem that owns this property
	PROPERTIESTYPE_Slider,
	PROPERTIESTYPE_SpriteFrames
};

struct PropertiesDef
{
	PropertiesType							eType;
	bool									bReadOnly;

	QColor									color;
	QString									sToolTip;

	QVariant								defaultData;
	QVariant								minRange;
	QVariant								maxRange;
	QVariant								stepAmt;
	QString									sPrefix;
	QString									sSuffix;

	QVariant								delegateBuilder; // Some types need an additional QVariant to build their delegate widget (e.g. ComboBox uses defaultData as currently selected index, but also needs a string list to select from)

	bool IsCategory() const {
		return eType == PROPERTIESTYPE_Category || eType == PROPERTIESTYPE_CategoryChecked;
	}
};

class PropertiesTreeModel : public ITreeModel
{
	Q_OBJECT

	ProjectItem &								m_OwnerRef;
	const int									m_iSTATE_INDEX;
	const QVariant								m_iSUBSTATE;

	QMap<TreeModelItem *, PropertiesDef>		m_PropertyDefMap;

	enum {
		COLUMN_Name = 0,
		COLUMN_Value = 1
	};

public:
	explicit PropertiesTreeModel(ProjectItem &ownerRef, int iStateIndex, QVariant subState, QObject *pParent = nullptr);
	virtual ~PropertiesTreeModel();

	ProjectItem &GetOwner();
	int GetStateIndex() const;
	const QVariant &GetSubstate() const;
	const PropertiesDef &GetPropertyDefinition(const QModelIndex &index) const;
	QString GetPropertyName(const QModelIndex &index) const;
	const QVariant &GetPropertyValue(const QModelIndex &index) const;

	bool AppendCategory(QString sName, QColor color, QVariant commonDelegateBuilder = QVariant(), bool bCheckable = false, bool bStartChecked = false, QString sToolTip = "");
	bool AppendProperty(QString sCategoryName,
						QString sName,
						QColor color,
						PropertiesType eType,
						QVariant delegateBuilder = QVariant(),
						bool bReadOnly = false,
						QString sToolTip = QString(),
						QVariant defaultData = QVariant(),
						QVariant minRange = QVariant(),
						QVariant maxRange = QVariant(),
						QVariant stepAmt = QVariant(),
						QString sPrefix = QString(),
						QString sSuffix = QString());

	void RefreshCategory(const QModelIndex &index);

	virtual QVariant data(const QModelIndex &index, int iRole = Qt::DisplayRole) const override;
	virtual Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
	QString ConvertValueToString(TreeModelItem *pTreeItem) const;
};

#endif // PROPERTIESTREEMODEL_H
