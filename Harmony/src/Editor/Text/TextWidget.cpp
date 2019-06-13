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

TextWidget::TextWidget(ProjectItem &itemRef, QWidget *parent)
	: IWidget(itemRef, parent)
{
	ui.setupUi(this);
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
