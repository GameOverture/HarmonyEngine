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

    PROPERTIESTYPE_bool,
    PROPERTIESTYPE_int,
    PROPERTIESTYPE_double,
    PROPERTIESTYPE_ivec2,
    PROPERTIESTYPE_vec2,
};

struct PropertiesDef
{
    PropertiesType  eType;

    QVariant        defaultValue;
    QVariant        minRange;
    QVariant        maxRange;
    QVariant        stepAmt;
    QString         sPrefix;
    QString         sSuffix;

    PropertiesDef()
    { }

    PropertiesDef(PropertiesType eType_,
                  QVariant defaultValue_,
                  QVariant minRange_,
                  QVariant maxRange_,
                  QVariant stepAmt_,
                  QString sPrefix_,
                  QString sSuffix_) :   eType(eType_),
                                        defaultValue(defaultValue_),
                                        minRange(minRange_),
                                        maxRange(maxRange_),
                                        stepAmt(stepAmt_),
                                        sPrefix(sPrefix_),
                                        sSuffix(sSuffix_)
    { }
};

class PropertiesTreeItem : public IModelTreeItem
{
protected:
    const QString                               m_sNAME;
    PropertiesTreeModel *                       m_pTreeModel;

    QVariant                                    m_Data;
    PropertiesDef                               m_DataDef;

    QColor                                      m_Color;

public:
    explicit PropertiesTreeItem(QString sName, PropertiesTreeModel *pTreeModel, const PropertiesDef &propertiesDef, QColor color);
    virtual ~PropertiesTreeItem();

    PropertiesType GetType();
    QString GetName();
    QString GetValue();

    QVariant GetData();
    void SetData(const QVariant &newData);
    const PropertiesDef &GetDataDef();

    QColor GetColor();

    virtual QString GetToolTip() const override;
};

#endif // PROPERTIESTREEITEM_H
