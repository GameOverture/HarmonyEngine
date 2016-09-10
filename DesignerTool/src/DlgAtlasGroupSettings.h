/**************************************************************************
 *	DlgAtlasGroupSettings.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
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
    bool        m_bNameChanged;

public:
    explicit DlgAtlasGroupSettings(QWidget *parent = 0);
    ~DlgAtlasGroupSettings();
    
    bool IsSettingsDirty()      { return m_bSettingsDirty; }
    bool IsNameChanged()        { return m_bNameChanged; }

    void WidgetsToData();
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
