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
    
    QList<AtlasFrame *> RequestFrames(QList<quint32> requestList);
    QList<AtlasFrame *> RequestFrames(QList<AtlasFrame *> requestList);
    void RelinquishFrames(QList<AtlasFrame *> relinquishList);
    void RefreshFrame(AtlasFrame *pFrame);
    
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    
    
    virtual int AppendState(QJsonObject stateObj) = 0;
    virtual void InsertState(int iStateIndex, QJsonObject stateObj) = 0;
    virtual QJsonObject PopStateAt(uint32 uiIndex) = 0;
    
    virtual QJsonValue GetSaveInfo() = 0;
};

#endif // IMODEL_H
