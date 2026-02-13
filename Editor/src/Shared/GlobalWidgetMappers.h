/**************************************************************************
*	GlobalWidgetMappers.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef GLOBALWIDGETMAPPERS_H
#define GLOBALWIDGETMAPPERS_H

#include <QAbstractListModel>
#include <QDataWidgetMapper>
#include <QLineEdit>
#include <QCheckBox>
#include <QDoubleSpinBox>

class SpinBoxMapper : public QDataWidgetMapper
{
	class ModelSpinBox : public QAbstractListModel
	{
		int          m_iValue;

	public:
		ModelSpinBox(QObject *pParent = nullptr) : QAbstractListModel(pParent), m_iValue(0) {
		}

		virtual ~ModelSpinBox() {
		}

		int GetValue() {
			return m_iValue;
		}

		void SetValue(int iValue) {
			m_iValue = iValue;
		}

		virtual int rowCount(const QModelIndex &parent /*= QModelIndex()*/) const override {
			return 1;
		}

		virtual QVariant data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const override {
			return m_iValue;
		}
	};

public:
	SpinBoxMapper(QObject *pParent = nullptr) : QDataWidgetMapper(pParent)
	{
		setModel(new ModelSpinBox(this));
	}
	virtual ~SpinBoxMapper()
	{ }

	void AddSpinBoxMapping(QSpinBox *pSpinBox)
	{
		pSpinBox->blockSignals(true);

		addMapping(pSpinBox, 0);
		this->setCurrentIndex(0);

		if(GetValue() < pSpinBox->minimum())
			SetValue(pSpinBox->minimum());
		if(GetValue() > pSpinBox->maximum())
			SetValue(pSpinBox->maximum());

		pSpinBox->setValue(GetValue());

		pSpinBox->blockSignals(false);
	}

	int GetValue()
	{
		return static_cast<ModelSpinBox *>(model())->GetValue();
	}

	void SetValue(int iValue)
	{
		static_cast<ModelSpinBox *>(model())->SetValue(iValue);
		setCurrentIndex(0);
	}
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

	void AddSpinBoxMapping(QDoubleSpinBox *pSpinBox)
	{
		clearMapping();
		
		pSpinBox->blockSignals(true);

		addMapping(pSpinBox, 0);
		this->setCurrentIndex(0);

		if(GetValue() < pSpinBox->minimum())
			SetValue(pSpinBox->minimum());
		if(GetValue() > pSpinBox->maximum())
			SetValue(pSpinBox->maximum());

		pSpinBox->setValue(GetValue());

		pSpinBox->blockSignals(false);
	}

	double GetValue()
	{
		return static_cast<ModelDoubleSpinBox *>(model())->GetValue();
	}

	void SetValue(double dValue)
	{
		if(mappedWidgetAt(0))
			mappedWidgetAt(0)->blockSignals(true);
	
		static_cast<ModelDoubleSpinBox *>(model())->SetValue(dValue);
		setCurrentIndex(0);

		if(mappedWidgetAt(0))
			mappedWidgetAt(0)->blockSignals(false);
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

	void AddLineEditMapping(QLineEdit *pLineEdit)
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
class ComboBoxMapper : public QDataWidgetMapper
{
	class ModelComboBox : public QAbstractListModel
	{
		QStringList         m_sItemList;
		QVariantList        m_DataList;

	public:
		ModelComboBox(QObject *pParent = nullptr) : QAbstractListModel(pParent) {
		}

		virtual ~ModelComboBox() {
		}

		void AddItem(QString sName, QVariant data) {
			m_sItemList.append(sName);
			m_DataList.append(data);
		}

		QString GetItem(int iIndex) {
			return m_sItemList[iIndex];
		}

		QVariant GetData(int iIndex) {
			return m_DataList[iIndex];
		}

		int FindItemIndex(QString sName) {
			for(int i = 0; i < m_sItemList.size(); ++i)
			{
				if(sName == m_sItemList[i])
					return i;
			}

			return -1;
		}

		virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override {
			return m_sItemList.size();
		}

		virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
			if(role == Qt::UserRole)
				return m_DataList[index.row()];
			else if(role == Qt::DisplayRole)
				return m_sItemList[index.row()];
			else
				return QVariant();
		}
	};

public:
	ComboBoxMapper(QObject *pParent = nullptr) : QDataWidgetMapper(pParent)
	{
		setModel(new ModelComboBox(this));
	}
	virtual ~ComboBoxMapper()
	{ }

	void AddComboBoxMapping(QComboBox *pComboBox)
	{
		pComboBox->blockSignals(true);

		pComboBox->setModel(model());
		addMapping(pComboBox, 0, "currentIndex");

		if(this->currentIndex() == -1 && static_cast<ModelComboBox *>(model())->rowCount() != 0)
			this->SetIndex(0);

		pComboBox->setCurrentIndex(this->currentIndex());

		pComboBox->blockSignals(false);
	}

	void AddItem(QString sName, QVariant data)
	{
		static_cast<ModelComboBox *>(model())->AddItem(sName, data);
	}

	QString GetCurrentItem()
	{
		if(currentIndex() < 0)
			return QString();

		return static_cast<ModelComboBox *>(model())->GetItem(currentIndex());
	}

	QVariant GetCurrentData()
	{
		if(currentIndex() < 0)
			return QVariant();

		return static_cast<ModelComboBox *>(model())->GetData(currentIndex());
	}

	void SetIndex(int iIndex)
	{
		setCurrentIndex(iIndex);

		int iTest = currentIndex();
		iTest = iTest;
	}

	void SetIndex(QString sName)
	{
		int iFoundIndex = static_cast<ModelComboBox *>(model())->FindItemIndex(sName);
		setCurrentIndex(iFoundIndex);
	}
};

#endif // GLOBALWIDGETMAPPERS_H
