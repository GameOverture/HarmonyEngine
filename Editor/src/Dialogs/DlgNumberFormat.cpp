/**************************************************************************
 *	DlgNumberFormat.cpp
 *
 *	Harmony Engine - HyEditor
 *	Copyright (c) 2026 Jason Knobler
 *
 *	GPL/LGPL License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
 *************************************************************************/
#include "Global.h"
#include "DlgNumberFormat.h"
#include "ui_DlgNumberFormat.h"

DlgNumberFormat::DlgNumberFormat(QString sTitle, uint32 uiSerializedFormat, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DlgNumberFormat),
	m_NumFormat(uiSerializedFormat)
{
	ui->setupUi(this);
	setWindowTitle(sTitle);

	ui->cmbDecimalSeparator->setCurrentIndex(m_NumFormat.GetDecimalSeparator());
	ui->cmbPosNegSign->setCurrentIndex(m_NumFormat.GetSign());
	ui->cmbDigitGroupings->setCurrentIndex(m_NumFormat.GetGrouping());
	ui->cmbRounding->setCurrentIndex(m_NumFormat.GetRounding());
	ui->sbFractionPercisionMin->setValue(m_NumFormat.GetFractionPrecisionMin());
	ui->sbFractionPercisionMax->setValue(m_NumFormat.GetFractionPrecisionMax());
	ui->chkUseCurrencySymbol->setChecked(m_NumFormat.IsUsingCurrencySymbol());
	ui->chkUseMinorCurrencySymbol->setChecked(m_NumFormat.IsUsingMinorCurrencySymbol());
	ui->chkUseScientificNotation->setChecked(m_NumFormat.IsUsingScientificNotation());
	ui->sbIntPadding->setValue(m_NumFormat.GetIntegerPaddingWidth());
}

/*virtual*/ DlgNumberFormat::~DlgNumberFormat()
{
	delete ui;
}

void DlgNumberFormat::SyncFormatToWidgets()
{
	m_NumFormat.SetDecimalSeparator(static_cast<HyNumDecimalSeparator>(ui->cmbDecimalSeparator->currentIndex()));
	m_NumFormat.SetSign(static_cast<HyNumFmtSign>(ui->cmbPosNegSign->currentIndex()));
	m_NumFormat.SetGrouping(static_cast<HyNumFmtGrouping>(ui->cmbDigitGroupings->currentIndex()));
	m_NumFormat.SetRounding(static_cast<HyNumFmtRounding>(ui->cmbRounding->currentIndex()));
	m_NumFormat.SetFractionPrecision(ui->sbFractionPercisionMin->value(), ui->sbFractionPercisionMax->value());
	m_NumFormat.SetUsingCurrencySymbol(ui->chkUseCurrencySymbol->isChecked());
	m_NumFormat.SetUsingMinorCurrencySymbol(ui->chkUseMinorCurrencySymbol->isChecked());
	m_NumFormat.SetUsingScientificNotation(ui->chkUseScientificNotation->isChecked());
	m_NumFormat.SetIntegerPaddingWidth(ui->sbIntPadding->value());
}

HyNumberFormat DlgNumberFormat::GetNumberFormat() const
{
	return m_NumFormat;
}
