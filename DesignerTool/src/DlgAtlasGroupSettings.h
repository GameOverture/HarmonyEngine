#ifndef DLGATLASGROUPSETTINGS_H
#define DLGATLASGROUPSETTINGS_H

#include <QDialog>

#include "scriptum/imagepacker.h"

namespace Ui {
class DlgAtlasGroupSettings;
}

class DlgAtlasGroupSettings : public QDialog
{
    Q_OBJECT

    QString     m_sName;
    
    int         m_iTextureWidth;
    int         m_iTextureHeight;
    int         m_iHeuristicIndex;
    int         m_iSortOrderIndex;
    int         m_iFrameMarginTop;
    int         m_iFrameMarginLeft;
    int         m_iFrameMarginRight;
    int         m_iFrameMarginBottom;
    int         m_iExtrude;
    bool        m_bMerge;
    bool        m_bAutoSize;
    bool        m_bSquare;
    int         m_iFillRate;
    int         m_iRotationStrategyIndex;

    bool        m_bSettingsDirty;

public:
    explicit DlgAtlasGroupSettings(QWidget *parent = 0);
    ~DlgAtlasGroupSettings();

    void DataFromWidgets();
    void DataToWidgets();

    QString GetName();
    void SetName(QString sName);
    
    int TextureWidth();
    int TextureHeight();

    int GetHeuristic();

    void SetPackerSettings(ImagePacker *pPacker);

    QJsonObject GetSettings();
    void LoadSettings(QJsonObject settings);

private slots:
    void on_cmbSortOrder_currentIndexChanged(int index);
    void on_cmbRotationStrategy_currentIndexChanged(int index);
    void on_cmbHeuristic_currentIndexChanged(int index);
    void on_alphaThreshold_valueChanged(int arg1);
    void on_extrude_valueChanged(int arg1);
    void on_minFillRate_valueChanged(int arg1);
    void on_chkMerge_stateChanged(int arg1);
    void on_chkAutosize_stateChanged(int arg1);
    void on_chkSquare_stateChanged(int arg1);
    void on_sbFrameMarginTop_valueChanged(int arg1);
    void on_sbFrameMarginLeft_valueChanged(int arg1);
    void on_sbFrameMarginRight_valueChanged(int arg1);
    void on_sbFrameMarginBottom_valueChanged(int arg1);
    void on_sbTextureWidth_valueChanged(int arg1);
    void on_sbTextureHeight_valueChanged(int arg1);
    void on_btnMatchTextureWidthHeight_clicked();
    void on_btnTexSize256_clicked();
    void on_btnTexSize512_clicked();
    void on_btnTexSize1024_clicked();
    void on_btnTexSize2048_clicked();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::DlgAtlasGroupSettings *ui;
};

#endif // DLGATLASGROUPSETTINGS_H
