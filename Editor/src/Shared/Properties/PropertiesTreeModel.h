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
#include "ProjectItemData.h"
#include "Shared/TreeModel/ITreeModel.h"

class PropertiesUndoCmd;

enum PropertiesColumn
{
	PROPERTIESCOLUMN_Name = 0,
	PROPERTIESCOLUMN_Value = 1
};

enum PropertiesType
{
	PROPERTIESTYPE_Unknown = -1,

	PROPERTIESTYPE_Root = 0,
	PROPERTIESTYPE_Category,

	PROPERTIESTYPE_bool,
	PROPERTIESTYPE_int,
	PROPERTIESTYPE_int64,
	PROPERTIESTYPE_double,			// delegateBuilder [int] = QDoubleSpinBox's decimals value. Aka precision.
	PROPERTIESTYPE_ivec2,
	PROPERTIESTYPE_vec2,
	PROPERTIESTYPE_ivec3,
	PROPERTIESTYPE_vec3,
	PROPERTIESTYPE_ivec4,
	PROPERTIESTYPE_vec4,
	PROPERTIESTYPE_LineEdit,
	PROPERTIESTYPE_FloatArray,		// QJsonArray of numbers (floats)
	PROPERTIESTYPE_ComboBoxInt,		// delegateBuilder [QStringList] = QComboBox's selection list
	PROPERTIESTYPE_ComboBoxString,	// delegateBuilder [QStringList] = QComboBox's selection list
	PROPERTIESTYPE_ComboBoxItems,	// delegateBuilder [ItemType] = Lists all items in the project of this type
	PROPERTIESTYPE_StatesComboBox,	// delegateBuilder [QUuid] = The ProjectItemData's UUID that contains the states
	PROPERTIESTYPE_Slider,
	PROPERTIESTYPE_Color,			// NOTE: Internally uses QVariant like `PROPERTIESTYPE_ivec3`
	PROPERTIESTYPE_SpriteFrames,	// delegateBuilder [QUuid] = The ProjectItemData's UUID that contains the sprite frames
	PROPERTIESTYPE_UiPanel,			// QJsonObject
};

enum PropertiesAccessType
{
	PROPERTIESACCESS_ReadOnly = 0,
	PROPERTIESACCESS_Mutable,
	PROPERTIESACCESS_ToggleChecked,
	PROPERTIESACCESS_ToggleUnchecked,
	PROPERTIESACCESS_TogglePartial,
};

struct PropertiesDef
{
	PropertiesType							eType;
	PropertiesAccessType					eAccessType;

	QString									sToolTip;

	QVariant								defaultData;
	QVariant								minRange;
	QVariant								maxRange;
	QVariant								stepAmt;
	QString									sPrefix;
	QString									sSuffix;

	QVariant								delegateBuilder;		// Some types need an additional QVariant to build their delegate widget (e.g. ComboBox uses defaultData as currently selected index, but also needs a string list to select from)

	bool									m_bIsDifferentValues;	// Defaults to false. Used in the PropertiesTreeMultiModel to indicate that the values are different across the models

	PropertiesDef() : 
		eType(PROPERTIESTYPE_Unknown),
		eAccessType(PROPERTIESACCESS_Mutable),
		m_bIsDifferentValues(false)
	{ }
	PropertiesDef(PropertiesType type, PropertiesAccessType accessType, QString toolTip, QVariant defaultData_, QVariant minRange_, QVariant maxRange_, QVariant stepAmt_, QString prefix, QString suffix, QVariant delegateBuilder_) :
		eType(type),
		eAccessType(accessType),
		sToolTip(toolTip),
		defaultData(defaultData_),
		minRange(minRange_),
		maxRange(maxRange_),
		stepAmt(stepAmt_),
		sPrefix(prefix),
		sSuffix(suffix),
		delegateBuilder(delegateBuilder_),
		m_bIsDifferentValues(false)
	{ }

	bool IsValid() const {
		return eType != PROPERTIESTYPE_Unknown;
	}
	bool IsCategory() const {
		return eType == PROPERTIESTYPE_Category;
	}
	bool IsToggleable() const {
		return eAccessType == PROPERTIESACCESS_ToggleChecked || eAccessType == PROPERTIESACCESS_ToggleUnchecked || eAccessType == PROPERTIESACCESS_TogglePartial;
	}
	HyColor GetColor() const {
		return IsCategory() ? HyGlobal::GetEditorColor(EDITORCOLOR_PropertiesCategory) : HyGlobal::GetEditorColor(EDITORCOLOR_PropertiesItem);
	}
};

class PropertiesTreeModel : public ITreeModel
{
	Q_OBJECT

	ProjectItemData *							m_pProjItemData;
	const int									m_iSTATE_INDEX;
	const QVariant								m_iSUBSTATE;

	QMap<TreeModelItem *, PropertiesDef>		m_PropertyDefMap;

public:
	explicit PropertiesTreeModel(ProjectItemData *pProjItemData, int iStateIndex, QVariant subState, QObject *pParent = nullptr);
	virtual ~PropertiesTreeModel();

	ProjectItemData *GetProjItem();
	int GetStateIndex() const;
	const QVariant &GetSubstate() const;

	PropertiesDef GetCategoryDefinition(QString sCategoryName) const;
	PropertiesDef GetDefinition(QString sCategoryName, QString sPropertyName) const;
	
	void SetToggleState(QString sCategoryName, Qt::CheckState eCheckState);
	void SetToggleState(QString sCategoryName, QString sPropertyName, Qt::CheckState eCheckState);
	void SetToggleState(const QModelIndex &indexRef, Qt::CheckState eCheckState);

	PropertiesDef GetIndexDefinition(const QModelIndex &indexRef) const;
	QString GetIndexName(const QModelIndex &indexRef) const;
	QVariant GetIndexValue(const QModelIndex &indexRef) const;
	QJsonValue GetIndexJsonValue(const QModelIndex &indexRef) const;

	int GetNumProperties(int iCategoryIndex) const;
	QString GetPropertyName(int iCategoryIndex, int iPropertyIndex) const;
	QVariant GetPropertyValue(int iCategoryIndex, int iPropertyIndex) const;
	QJsonValue GetPropertyJsonValue(int iCategoryIndex, int iPropertyIndex) const;
	bool IsPropertyDefaultValue(int iCategoryIndex, int iPropertyIndex) const;
	QVariant FindPropertyValue(QString sCategoryName, QString sPropertyName) const;
	QJsonValue FindPropertyJsonValue(QString sCategoryName, QString sPropertyName) const;
	QModelIndex FindPropertyModelIndex(QString sCategoryName, QString sPropertyName) const;

	virtual void SetPropertyValue(QString sCategoryName, QString sPropertyName, const QVariant &valueRef);
	void SetPropertyAsDifferentValues(QString sCategoryName, QString sPropertyName);

	bool DoesCategoryExist(QString sCategoryName) const;
	bool IsCategory(const QModelIndex &indexRef) const;
	bool IsCategoryEnabled(QString sCategoryName) const;
	bool IsCategoryEnabled(int iCategoryIndex) const;
	int GetNumCategories() const;
	QString GetCategoryName(const QModelIndex &indexRef) const;
	QString GetCategoryName(int iCategoryIndex) const;
	QModelIndex GetCategoryModelIndex(int iCategoryIndex) const;
	bool IsCategoryCheckable(int iCategoryIndex) const;

	bool AppendCategory(QString sCategoryName, QVariant commonDelegateBuilder = QVariant(), bool bCheckable = false, QString sToolTip = "");
	bool AppendProperty(QString sCategoryName,
						QString sName,
						PropertiesType eType,
						QVariant defaultData = QVariant(),
						QString sToolTip = QString(),
						PropertiesAccessType eAccessType = PROPERTIESACCESS_Mutable,
						QVariant minRange = QVariant(),
						QVariant maxRange = QVariant(),
						QVariant stepAmt = QVariant(),
						QString sPrefix = QString(),
						QString sSuffix = QString(),
						QVariant delegateBuilder = QVariant());

	QPair<QString, QString> GetCatPropPairName(const QModelIndex &indexRef) const;
	QList<QPair<QString, QString>> GetPropertiesList() const; // Returns a list of all properties in the form of (category, property) pairs
	void RemoveAllCategoryProperties();

	QJsonObject SerializeJson() const;
	void DeserializeJson(const QJsonObject &propertiesObj);

	void ResetValues(); // Sets all values to their default values

	virtual bool setData(const QModelIndex &indexRef, const QVariant &valueRef, int iRole = Qt::EditRole) override;
	virtual QVariant data(const QModelIndex &indexRef, int iRole = Qt::DisplayRole) const override;
	
	virtual Qt::ItemFlags flags(const QModelIndex& indexRef) const override;

	virtual void OnTreeModelItemRemoved(TreeModelItem *pTreeItem) override;

	static QVariant ConvertJsonToVariant(PropertiesType ePropType, const QJsonValue &valueRef);
	static QJsonValue ConvertVariantToJson(PropertiesType ePropType, const QVariant &valueRef);

	virtual PropertiesUndoCmd *AllocateUndoCmd(const QModelIndex &index, const QVariant &newData);

private:
	QJsonValue ConvertValueToJson(TreeModelItem *pTreeItem) const;
	QString ConvertValueToString(TreeModelItem *pTreeItem) const;
};

#endif // PROPERTIESTREEMODEL_H
