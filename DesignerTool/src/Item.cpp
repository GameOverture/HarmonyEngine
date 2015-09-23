#include "Item.h"

#include "MainWindow.h"

#include <QFileInfo>

Item::Item(eItemType eType, const QString sPath) :  m_eType(ITEM_Unknown),
                m_pTreeItemPtr(NULL)
{
    Initialize(eType, sPath);
}

Item::Item(const Item &other)
{
    Initialize(other.GetType(), other.GetPath());
}

Item::~Item()
{
    MainWindow::CloseItem(this);
    
    delete m_pTreeItemPtr;
}

void Item::Initialize(eItemType eType, const QString sPath)
{
    m_eType = eType;
    
    if(m_eType == ITEM_Unknown)
    {
        HYLOG("Setting path of unknown item (" % m_sPath % ")", LOGTYPE_Error);
        return;
    }
    
    m_sPath = sPath;
    m_sPath.replace(QChar('\\'), QChar('/'));
            
    QString sExt = HyGlobal::ItemExt(m_eType);
    if(m_sPath.right(sExt.size()) != sExt)
        m_sPath.append(sExt);
}

QString Item::GetName() const
{
    // NOTE: We must remove the extension because dir items use "/", which doesn't work with QFileInfo::baseName()
    QString sPathWithoutExt = m_sPath;
    sPathWithoutExt.truncate(m_sPath.size() - HyGlobal::ItemExt(m_eType).size());
    
    QFileInfo itemInfo;
    itemInfo.setFile(sPathWithoutExt);
    
    return itemInfo.baseName();
}

/*virtual*/ void Item::Hide()
{
    HYLOG("Tried to Hide() a non-derived item: " % GetName(), LOGTYPE_Error);
}

/*virtual*/ void Item::Show()
{
    HYLOG("Tried to Show() a non-derived item: " % GetName(), LOGTYPE_Error);
}

/*virtual*/ void Item::Draw(HyGuiApp *pHyApp)
{
    HYLOG("Tried to draw a non-derived item: " % GetName(), LOGTYPE_Error);
}

/*virtual*/ void Item::Save()
{
    HYLOG("Tried to save a non-derived item: " % GetName(), LOGTYPE_Error);
}
