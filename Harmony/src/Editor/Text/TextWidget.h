#pragma once

#include <QWidget>
#include "ui_TextWidget.h"

class TextWidget : public QWidget
{
	Q_OBJECT

public:
	TextWidget(QWidget *parent = Q_NULLPTR);
	~TextWidget();

private:
	Ui::TextWidget ui;
};
