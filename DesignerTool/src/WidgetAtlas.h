#ifndef WIDGETATLAS_H
#define WIDGETATLAS_H

#include <QWidget>

#include "scriptum/imagepacker.h"

namespace Ui {
class WidgetAtlas;
}

class WidgetAtlas : public QWidget
{
    Q_OBJECT
    
    ImagePacker         m_packer;
    
    bool                m_bDirty;
    
public:
    explicit WidgetAtlas(QWidget *parent = 0);
    ~WidgetAtlas();
    
private slots:
    void on_btnTexSize256_clicked();
    
    void on_btnTexSize512_clicked();
    
    void on_btnTexSize1024_clicked();
    
    void on_btnTexSize2048_clicked();
    
    void on_btnAddFiles_clicked();
    
    void on_btnAddDir_clicked();
    
    void on_cmbHeuristic_currentIndexChanged(const QString &arg1);
    
    void on_cmbSortOrder_currentIndexChanged(const QString &arg1);
    
    void on_cmbRotationStrategy_currentIndexChanged(const QString &arg1);
    
    void on_sbFrameMarginTop_valueChanged(int arg1);
    
    void on_sbFrameMarginRight_valueChanged(int arg1);
    
    void on_sbFrameMarginBottom_valueChanged(int arg1);
    
    void on_sbFrameMarginLeft_valueChanged(int arg1);
    
    void on_tabWidget_currentChanged(int index);
    
private:
    Ui::WidgetAtlas *ui;
};

#endif // WIDGETATLAS_H
