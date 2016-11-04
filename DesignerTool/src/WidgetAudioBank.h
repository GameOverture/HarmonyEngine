#ifndef WIDGEAUDIOBANK_H
#define WIDGEAUDIOBANK_H

#include <QWidget>

namespace Ui {
class WidgetAudioBank;
}

class WidgetAudioBank : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetAudioBank(QWidget *parent = 0);
    ~WidgetAudioBank();

private:
    Ui::WidgetAudioBank *ui;
};

#endif // WIDGEAUDIOBANK_H
