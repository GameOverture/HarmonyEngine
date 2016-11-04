#ifndef WIDGETAUDIOMANAGER_H
#define WIDGETAUDIOMANAGER_H

#include <QWidget>

namespace Ui {
class WidgetAudioManager;
}

class WidgetAudioManager : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetAudioManager(QWidget *parent = 0);
    ~WidgetAudioManager();

private:
    Ui::WidgetAudioManager *ui;
};

#endif // WIDGETAUDIOMANAGER_H
