/**************************************************************************
*	TextWidget.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "global.h"
#include "TextWidget.h"
#include "Project.h"

#include <QStandardPaths>
#include <QComboBox>
#include <QFontDatabase>

#define TEXTWIDGET_FontFilterList {"*.ttf", "*.otf"}

TextWidget::TextWidget(ProjectItem &itemRef, QWidget *parent) :
	IWidget(itemRef, parent),
	m_FontListModel(this)
{
	ui.setupUi(this);

	// Remove and re-add the main layout that holds everything. This makes the Qt Designer (.ui) files work with the base class 'IWidget'. Otherwise it jumbles them together.
	layout()->removeItem(ui.verticalLayout_3);
	layout()->addItem(ui.verticalLayout_3);


	QStringList sFilterList(TEXTWIDGET_FontFilterList);
	QMap<QString,QString> fontsMap;

	QStringList sSystemFontPaths = QStandardPaths::standardLocations(QStandardPaths::FontsLocation);
	for(int i = 0; i < sSystemFontPaths.count(); ++i)
	{
		QDir fontDir(sSystemFontPaths[i]);
		QFileInfoList fontFileInfoList = fontDir.entryInfoList(sFilterList);

		for(int i = 0; i < fontFileInfoList.count(); ++i)
			fontsMap[fontFileInfoList[i].fileName()] = fontFileInfoList[i].absoluteFilePath();
	}

	int iRow = 0;
	for(auto iter = fontsMap.begin(); iter != fontsMap.end(); ++iter, ++iRow)
	{
		QStandardItem *pFontItem = new QStandardItem(iter.key());
		pFontItem->setData(iter.value());

		m_FontListModel.setItem(iRow, pFontItem);
	}

	ScanMetaFontDir();
	ui.cmbFont->setModel(&m_FontListModel);

	// Set font size combobox with the standard sizes.
	QFontDatabase fontDatabase;
	QList<int> sizeList = fontDatabase.standardSizes();
	QStringList sSizeList;
	for(int i = 0; i < sizeList.size(); ++i)
		sSizeList.append(QString::number(sizeList[i]));

	ui.cmbSize->setFixedSize(64, 20);
	ui.cmbSize->setValidator(HyGlobal::NumbersValidator());
	ui.cmbSize->clear();
	ui.cmbSize->insertItems(0, sSizeList);
}

TextWidget::~TextWidget()
{
}

/*virtual*/ void TextWidget::OnGiveMenuActions(QMenu *pMenu) /*override*/
{
}

/*virtual*/ void TextWidget::OnUpdateActions() /*override*/
{
}

/*virtual*/ void TextWidget::OnFocusState(int iStateIndex, QVariant subState) /*override*/
{
}

void TextWidget::on_cmbFont_currentIndexChanged(int index)
{
	
}

void TextWidget::ScanMetaFontDir()
{
	QStringList sFilterList(TEXTWIDGET_FontFilterList);
	QDir metaDir(m_ItemRef.GetProject().GetMetaDataAbsPath() % HYMETA_FontsDir);
	QFileInfoList metaFontFileInfoList = metaDir.entryInfoList(sFilterList);

	QMap<QString,QString> metaFontsMap;
	for(int i = 0; i < metaFontFileInfoList.count(); ++i)
	{
		auto foundItemList = m_FontListModel.findItems(metaFontFileInfoList[i].fileName());
		while(foundItemList.empty() == false)
		{
			// Remove the font since we're gonna add it again as a meta dir font
			m_FontListModel.removeRow(foundItemList[0]->index().row(), foundItemList[0]->index().parent());
			foundItemList = m_FontListModel.findItems(metaFontFileInfoList[i].fileName());
		};
		
		metaFontsMap[metaFontFileInfoList[i].fileName()] = metaFontFileInfoList[i].absoluteFilePath();
	}

	for(auto iter = metaFontsMap.begin(); iter != metaFontsMap.end(); ++iter)
	{
		QStandardItem *pFontItem = new QStandardItem(HyGlobal::ItemIcon(ITEM_Font, SUBICON_None), iter.key());
		pFontItem->setData(iter.value());

		m_FontListModel.setItem(0, pFontItem);
	}
}
