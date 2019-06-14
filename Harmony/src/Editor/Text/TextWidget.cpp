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

#include <QComboBox>
#include <QFontDatabase>

TextWidget::TextWidget(ProjectItem &itemRef, QWidget *parent) :
	IWidget(itemRef, parent)
{
	ui.setupUi(this);

	// Remove and re-add the main layout that holds everything. This makes the Qt Designer (.ui) files work with the base class 'IWidget'. Otherwise it jumbles them together.
	layout()->removeItem(ui.verticalLayout_3);
	layout()->addItem(ui.verticalLayout_3);

	static_cast<TextModel *>(m_ItemRef.GetModel())->MapFontComboBox(ui.cmbFont);

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

	ui.layersTableView->setModel(static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(GetCurStateIndex()));
	ui.glyphsTreeView->setModel(static_cast<TextModel *>(m_ItemRef.GetModel())->GetGlyphsModel());
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
	ui.layersTableView->setModel(static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(iStateIndex));
}

void TextWidget::on_cmbFont_currentIndexChanged(int index)
{
	
}
