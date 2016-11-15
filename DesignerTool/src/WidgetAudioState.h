#ifndef WIDGETAUDIOSTATE_H
#define WIDGETAUDIOSTATE_H

#include <QWidget>

namespace Ui {
class WidgetAudioState;
}

class WidgetAudioState : public QWidget
{
    Q_OBJECT
    
public:
    explicit WidgetAudioState(QWidget *parent = 0);
    ~WidgetAudioState();
    
private:
    Ui::WidgetAudioState *ui;
};

#endif // WIDGETAUDIOSTATE_H
