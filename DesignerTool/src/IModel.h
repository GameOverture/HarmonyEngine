/**************************************************************************
 *	IModel.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IMODEL_H
#define IMODEL_H

#include "ProjectItem.h"

#include <QAbstractListModel>
#include <QDataWidgetMapper>
#include <QLineEdit>
#include <QCheckBox>

class IModel;

class DoubleSpinBoxMapper : public QDataWidgetMapper
{
    class ModelDoubleSpinBox : public QAbstractListModel
    {
        double          m_dValue;
        
    public:
        ModelDoubleSpinBox(QObject *pParent = nullptr) : QAbstractListModel(pParent), m_dValue(0.0) {
        }

        virtual ~ModelDoubleSpinBox() {
        }
        
        double GetValue() {
            return m_dValue;
        }
        
        void SetValue(double dValue) {
            m_dValue = dValue;
        }

        virtual int rowCount(const QModelIndex &parent /*= QModelIndex()*/) const override {
            return 1;
        }

        virtual QVariant data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const override {
            return m_dValue;
        }
    };
    
public:
    DoubleSpinBoxMapper(QObject *pParent = nullptr) : QDataWidgetMapper(pParent)
    {
        setModel(new ModelDoubleSpinBox(this));
    }
    virtual ~DoubleSpinBoxMapper()
    { }

    void AddCheckBoxMapping(QLineEdit *pLineEdit)
    {
        addMapping(pLineEdit, 0);
        this->setCurrentIndex(0);
    }

    double GetValue()
    {
        return static_cast<ModelDoubleSpinBox *>(model())->GetValue();
    }

    void SetValue(double dValue)
    {
        static_cast<ModelDoubleSpinBox *>(model())->SetValue(dValue);
        setCurrentIndex(0);
    }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class LineEditMapper : public QDataWidgetMapper
{
    class ModelLineEdit : public QAbstractListModel
    {
        QString     m_sString;
        
    public:
        ModelLineEdit(QObject *pParent = nullptr) : QAbstractListModel(pParent) {
        }

        virtual ~ModelLineEdit() {
        }
        
        QString GetString() {
            return m_sString;
        }
        
        void SetString(QString sString) {
            m_sString = sString;
        }

        virtual int rowCount(const QModelIndex &parent /*= QModelIndex()*/) const override {
            return 1;
        }

        virtual QVariant data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const override {
            return m_sString;
        }
    };
    
public:
    LineEditMapper(QObject *pParent = nullptr) : QDataWidgetMapper(pParent)
    {
        setModel(new ModelLineEdit(this));
    }
    virtual ~LineEditMapper()
    { }

    void AddCheckBoxMapping(QLineEdit *pLineEdit)
    {
        addMapping(pLineEdit, 0);
        this->setCurrentIndex(0);
    }

    QString GetString()
    {
        return static_cast<ModelLineEdit *>(model())->GetString();
    }

    void SetString(QString sString)
    {
        static_cast<ModelLineEdit *>(model())->SetString(sString);
        setCurrentIndex(0);
    }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CheckBoxMapper : public QDataWidgetMapper
{
    class ModelCheckBox : public QAbstractListModel
    {
    public:
        ModelCheckBox(QObject *pParent = nullptr) : QAbstractListModel(pParent) {
        }

        virtual ~ModelCheckBox() {
        }

        virtual int rowCount(const QModelIndex &parent /*= QModelIndex()*/) const override {
            return 2;
        }

        virtual QVariant data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const override {
            return index.row() == 0 ? false : true;
        }
    };

public:
    CheckBoxMapper(QObject *pParent = nullptr) : QDataWidgetMapper(pParent)
    {
        setModel(new ModelCheckBox(this));
    }
    virtual ~CheckBoxMapper()
    { }

    void AddCheckBoxMapping(QCheckBox *pCheckBox)
    {
        addMapping(pCheckBox, 0);
        this->setCurrentIndex(this->currentIndex());
    }

    bool IsChecked()
    {
        return currentIndex() == 0 ? false : true;
    }

    void SetChecked(bool bChecked)
    {
        setCurrentIndex(bChecked ? 1 : 0);
    }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IStateData
{
protected:
    IModel &        m_ModelRef;
    QString         m_sName;
    
public:
    IStateData(IModel &modelRef, QString sName);
    virtual ~IStateData();
    
    QString GetName();
    void SetName(QString sNewName);
    
    virtual void AddFrame(AtlasFrame *pFrame) = 0;
    virtual void RelinquishFrame(AtlasFrame *pFrame) = 0;
    virtual void RefreshFrame(AtlasFrame *pFrame) = 0;
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IModel : public QAbstractListModel
{
protected:
    ProjectItem *               m_pItem;
    QList<IStateData *>         m_StateList;
    
public:
    IModel(ProjectItem *pItem);
    virtual ~IModel();
    
    int GetNumStates();
    IStateData *GetStateData(int iStateIndex);
    
    QString SetStateName(int iStateIndex, QString sNewName);
    void MoveStateBack(int iStateIndex);
    void MoveStateForward(int iStateIndex);
    
    QList<AtlasFrame *> RequestFrames(IStateData *pState, QList<quint32> requestList);
    QList<AtlasFrame *> RequestFrames(int iStateIndex, QList<AtlasFrame *> requestList);
    void RelinquishFrames(int iStateIndex, QList<AtlasFrame *> relinquishList);
    void RefreshFrame(AtlasFrame *pFrame);
    
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    
    template<typename STATEDATA>
    int AppendState(QJsonObject stateObj)
    {
        int iIndex = m_StateList.size();
        InsertState<STATEDATA>(iIndex, stateObj);
    
        return iIndex;
    }
    
    template<typename STATEDATA>
    void InsertState(int iStateIndex, QJsonObject stateObj)
    {
        STATEDATA *pNewState = new STATEDATA(*this, stateObj);
    
        beginInsertRows(QModelIndex(), iStateIndex, iStateIndex);
        m_StateList.insert(iStateIndex, pNewState);
        endInsertRows();
    
        QVector<int> roleList;
        roleList.append(Qt::DisplayRole);
        dataChanged(createIndex(0, 0), createIndex(m_StateList.size() - 1, 0), roleList);
    }
    
    virtual QJsonObject PopStateAt(uint32 uiIndex) = 0;
    virtual QJsonValue GetSaveInfo() = 0;
};

#endif // IMODEL_H
