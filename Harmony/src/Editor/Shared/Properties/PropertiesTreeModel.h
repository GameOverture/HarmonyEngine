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
	PROPERTIESTYPE_double,          // delegateBuilder [int] = QDoubleSpinBox's decimals value. Aka percision.
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
	const PropertiesType						eTYPE;
	const bool									bREAD_ONLY;

	QColor										color;
	QString										sToolTip;

	QVariant									defaultData;
	QVariant									minRange;
	QVariant									maxRange;
	QVariant									stepAmt;
	QString										sPrefix;
	QString										sSuffix;

	QVariant									delegateBuilder; // Some types need an additional QVariant to build their delegate widget (e.g. ComboBox uses defaultData as currently selected index, but also needs a string list to select from)

	PropertiesDef(PropertiesType eType_, bool bReadOnly, QVariant defaultData_) :
		eTYPE(eType_),
		bREAD_ONLY(bReadOnly),
		defaultData(defaultData_)
	{ }
	PropertiesDef(PropertiesType eType_, bool bReadOnly, QVariant defaultData_, QVariant minRange_, QVariant maxRange_, QVariant stepAmt_, QString sPrefix_, QString sSuffix_, QVariant delegateBuilder_ = QVariant()) :
		eTYPE(eType_),
		bREAD_ONLY(bReadOnly),
		defaultData(defaultData_),
		minRange(minRange_),
		maxRange(maxRange_),
		stepAmt(stepAmt_),
		sPrefix(sPrefix_),
		sSuffix(sSuffix_),
		delegateBuilder(delegateBuilder_)
	{ }
};

class PropertiesTreeModel : public ITreeModel
{
	Q_OBJECT

	ProjectItem &								m_OwnerRef;
	const int									m_iSTATE_INDEX;
	const QVariant								m_iSUBSTATE;

	QMap<TreeModelItem *, PropertiesDef *>		m_PropertyDefMap;

public:
	explicit PropertiesTreeModel(ProjectItem &ownerRef, int iStateIndex, QVariant subState, QObject *pParent = nullptr);
	virtual ~PropertiesTreeModel();

	ProjectItem &GetOwner();
	//QVariant GetValue(QString sUniquePropertyName) const;

	bool AppendCategory(QString sName, QColor color, QVariant commonDelegateBuilder = QVariant(), bool bCheckable = false, bool bStartChecked = false, QString sToolTip = "");
	bool AppendProperty(QString sCategoryName, QString sName, PropertiesDef defintion, QString sToolTip, bool bReadOnly = false);

	//void RefreshProperties();

	virtual QVariant data(const QModelIndex &index, int iRole = Qt::DisplayRole) const override;
	virtual Qt::ItemFlags flags(const QModelIndex& index) const override;

	// This creates an Undo command to be pushed on the UndoStack
	virtual bool setData(const QModelIndex &index, const QVariant &value, int iRole = Qt::EditRole) override;

private:
	PropertiesTreeItem *ValidateCategory(QString sCategoryName, QString sUniquePropertyName);
};

#endif // PROPERTIESTREEMODEL_H
