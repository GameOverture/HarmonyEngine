#ifndef HYGUIATLASGROUP_H
#define HYGUIATLASGROUP_H

#include <QTreeWidgetItem>
#include <QFileInfo>

#include "DlgAtlasGroupSettings.h"
#include "scriptum/imagepacker.h"

class HyGuiAtlasGroup
{
    QFileInfo               m_SettingsFile;
    DlgAtlasGroupSettings   m_dlgSettings;

    QTreeWidgetItem *       m_pTreeItem;
    ImagePacker             m_Packer;
    
public:
    HyGuiAtlasGroup();
    ~HyGuiAtlasGroup();
    
    void SaveSettings();
    void LoadSettings();
};

#endif // HYGUIATLASGROUP_H
