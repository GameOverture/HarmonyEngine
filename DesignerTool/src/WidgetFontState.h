/**************************************************************************
 *	WidgetFontState.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETFONTSTATE_H
#define WIDGETFONTSTATE_H

#include "WidgetFontModelView.h"

#include <QWidget>

namespace Ui {
class WidgetFontState;
}

class WidgetFont;

class WidgetFontState : public QWidget
{
    Q_OBJECT
    
    WidgetFont *                m_pOwner;
    WidgetFontModel *           m_pFontModel;
    
    QString                     m_sName;

    int                         m_iPrevFontCmbIndex;

public:
    explicit WidgetFontState(WidgetFont *pOwner, QList<QAction *> stateActionList, QWidget *parent = 0);
    virtual ~WidgetFontState();
    
    QString GetName();
    void SetName(QString sNewName);
    
    WidgetFontModel *GetFontModel();
    
    QString GetFontFilePath();
    
private slots:
    void on_cmbFontList_currentIndexChanged(int index);

private:
    Ui::WidgetFontState *ui;
};
Q_DECLARE_METATYPE(WidgetFontState *)

#endif // WIDGETFONTSTATE_H
