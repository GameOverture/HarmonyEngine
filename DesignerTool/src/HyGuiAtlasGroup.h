#ifndef HYGUIATLASGROUP_H
#define HYGUIATLASGROUP_H

#include <QTreeWidgetItem>

#include "scriptum/imagepacker.h"

class HyGuiAtlasGroup
{
    QTreeWidgetItem *       m_pTreeItem;
    ImagePacker             m_Packer;
    
public:
    HyGuiAtlasGroup();
    ~HyGuiAtlasGroup();
    
    void SaveSettings();
    void LoadSettings();
};

#endif // HYGUIATLASGROUP_H
