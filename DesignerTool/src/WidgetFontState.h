#ifndef WIDGETFONTSTATE_H
#define WIDGETFONTSTATE_H

#include "WidgetFontModelView.h"

#include <QWidget>

namespace Ui {
class WidgetFontState;
}

class WidgetFontState : public QWidget
{
    Q_OBJECT
    
    WidgetSprite *              m_pOwner;
    WidgetFontModel *           m_pFontModel;

    int                         m_iPrevFontCmbIndex;

public:
    explicit WidgetFontState(QWidget *parent = 0, QList<QAction *> stateActionList);
    ~WidgetFontState();
    
private slots:
    void on_cmbFontList_currentIndexChanged(int index);

private:
    Ui::WidgetFontState *ui;
};

#endif // WIDGETFONTSTATE_H
