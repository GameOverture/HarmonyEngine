/**************************************************************************
 *	WidgetGlyphCreator.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetGlyphCreator.h"
#include "ui_WidgetGlyphCreator.h"

#include <QStandardPaths>
#include <QDir>

WidgetGlyphCreator::WidgetGlyphCreator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetGlyphCreator)
{
    ui->setupUi(this);
    
    QStringList sFontPaths = QStandardPaths::standardLocations(QStandardPaths::FontsLocation);
    foreach(QString sPath, sFontPaths)
    {
        QDir fontDir(sPath);
        if(fontDir.exists() == false)
            return;
    }
    
    //ui->fontComboBox->font().family
}

WidgetGlyphCreator::~WidgetGlyphCreator()
{
    delete ui;
}
