/**************************************************************************
 *	WidgetSprite.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETSPRITE_H
#define WIDGETSPRITE_H

#include <QWidget>
#include <QMenu>
#include <QUndoStack>
#include <QComboBox>

namespace Ui {
class WidgetSprite;
}

class ItemSprite;

class WidgetSprite : public QWidget
{
    Q_OBJECT
    
    ItemSprite *        m_pItemSprite;

    QUndoStack *        m_pUndoStack;
    QMenu *             m_pEditMenu;

    struct State
    {
        QComboBox       m_p;
    };

public:
    explicit WidgetSprite(ItemSprite *pItemSprite, QWidget *parent = 0);
    ~WidgetSprite();
    
private slots:
    void on_btnAddState_clicked();

private:
    Ui::WidgetSprite *ui;
};

#endif // WIDGETSPRITE_H
