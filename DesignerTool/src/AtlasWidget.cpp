/**************************************************************************
 *	AtlasWidget.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "AtlasWidget.h"
#include "ui_AtlasWidget.h"
#include "HyGuiGlobal.h"
#include "SpriteWidget.h"
#include "ExplorerItem.h"

#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QImage>
#include <QPainter>
#include <QMessageBox>

#include "MainWindow.h"
#include "DlgInputName.h"
#include "DlgAtlasGroupSettings.h"

const float fTRANS_DUR = 0.5f;
const int iPADDING = 2;

AtlasTreeWidget::AtlasTreeWidget(QWidget *parent /*= Q_NULLPTR*/) : QTreeWidget(parent)
{
}
void AtlasTreeWidget::SetOwner(AtlasWidget *pOwner)
{
    m_pOwner = pOwner;
}
/*virtual*/ void AtlasTreeWidget::dropEvent(QDropEvent *e)
{
    QTreeWidget::dropEvent(e);

    sortItems(0, Qt::AscendingOrder);
    m_pOwner->GetData().WriteMetaSettings();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool AtlasTreeItem::operator<(const QTreeWidgetItem &rhs) const
{
    bool bLeftIsFilter = this->data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter;
    bool bRightIsFilter = rhs.data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter;

    if(bLeftIsFilter && bRightIsFilter == false)
        return true;
    if(bLeftIsFilter == false && bRightIsFilter)
        return false;

    return this->text(0) < rhs.text(0);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AtlasWidget::AtlasWidget(QWidget *parent) :   QWidget(parent),
                                              m_pModel(nullptr),
                                              m_Draw(nullptr, nullptr),
                                              ui(new Ui::AtlasWidget)
{
    ui->setupUi(this);
    
    // NOTE: THIS CONSTRUCTOR IS INVALID TO USE. IT EXISTS FOR QT TO ALLOW Q_OBJECT TO WORK
    HyGuiLog("WidgetAtlasManager::WidgetAtlasManager() invalid constructor used", LOGTYPE_Error);
}

AtlasWidget::AtlasWidget(AtlasModel *pModel, IHyApplication *pHyApp, QWidget *parent /*= 0*/) : QWidget(parent),
                                                                                                ui(new Ui::AtlasWidget),
                                                                                                m_pModel(pModel),
                                                                                                m_Draw(pModel, pHyApp),
                                                                                                m_pMouseHoverItem(nullptr)
{
    ui->setupUi(this);

    ui->actionDeleteImages->setEnabled(false);
    ui->actionReplaceImages->setEnabled(false);

    ui->btnDeleteImages->setDefaultAction(ui->actionDeleteImages);
    ui->btnReplaceImages->setDefaultAction(ui->actionReplaceImages);
    ui->btnAddFilter->setDefaultAction(ui->actionAddFilter);

    ui->atlasList->SetOwner(this);

    ui->atlasList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->atlasList->setDragEnabled(true);
    //ui->atlasList->viewport()->setAcceptDrops(true);
    ui->atlasList->setDropIndicatorShown(true);
    ui->atlasList->setDragDropMode(QAbstractItemView::InternalMove);

    QList<AtlasTreeItem *> atlasTreeItemList = m_pModel->GetTopLevelTreeItemList();
    for(int i = 0; i < atlasTreeItemList.size(); ++i)
        ui->atlasList->addTopLevelItem(atlasTreeItemList[i]);

    ui->atlasList->sortByColumn(0, Qt::AscendingOrder);
    ui->atlasList->setSortingEnabled(true);
    ui->atlasList->sortItems(0, Qt::AscendingOrder);

    RefreshLcds();

    ui->atlasList->collapseAll();

    ui->atlasList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->atlasList, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(OnContextMenu(const QPoint&)));
}

AtlasWidget::~AtlasWidget()
{
    delete ui;
}

AtlasModel &AtlasWidget::GetData()
{
    return *m_pModel;
}

QTreeWidget *AtlasWidget::GetFramesTreeWidget()
{
    return ui->atlasList;
}

void AtlasWidget::DrawUpdate(IHyApplication &hyApp)
{
    m_Draw.SetHover(ui->atlasList->itemAt(ui->atlasList->mapFromGlobal(QCursor::pos())));
    m_Draw.Update(hyApp);
}

void AtlasWidget::StashTreeWidgets()
{
    QList<AtlasTreeItem *> stashedTreeItemList;
    while(ui->atlasList->topLevelItemCount())
        stashedTreeItemList.append(static_cast<AtlasTreeItem *>(ui->atlasList->takeTopLevelItem(0)));

    m_pModel->TakeTreeWidgets(stashedTreeItemList);
}

void AtlasWidget::RefreshLcds()
{
    ui->lcdNumTextures->display(m_pModel->GetNumTextures());
    ui->lcdTexWidth->display(m_pModel->GetAtlasDimensions().width());
    ui->lcdTexHeight->display(m_pModel->GetAtlasDimensions().height());
}

/*virtual*/ void AtlasWidget::enterEvent(QEvent *pEvent) /*override*/
{
    m_Draw.Show();
    QWidget::enterEvent(pEvent);
}

/*virtual*/ void AtlasWidget::leaveEvent(QEvent *pEvent) /*override*/
{
    m_Draw.Hide();
    QWidget::leaveEvent(pEvent);
}

/*virtual*/ void AtlasWidget::resizeEvent(QResizeEvent *event) /*override*/
{
    QWidget::resizeEvent(event);

    if(ui->atlasList == NULL)
        return;

    int iTotalWidth = ui->atlasList->size().width();
    ui->atlasList->setColumnWidth(0, iTotalWidth - 60);
}

void AtlasWidget::on_btnAddImages_clicked()
{
    QFileDialog dlg(this);
    dlg.setFileMode(QFileDialog::ExistingFile);
    dlg.setViewMode(QFileDialog::Detail);
    dlg.setWindowModality(Qt::ApplicationModal);
    dlg.setModal(true);

    QString sSelectedFilter(tr("PNG (*.png)"));
    QStringList sImportImgList = QFileDialog::getOpenFileNames(this,
                                                               "Import image(s) into atlases",
                                                               QString(),
                                                               tr("All files (*.*);;PNG (*.png)"),
                                                               &sSelectedFilter);

    if(sImportImgList.empty() == false)
        m_pModel->Repack(QSet<int>(), m_pModel->ImportImages(sImportImgList));
    
    RefreshLcds();
}

void AtlasWidget::on_btnAddDir_clicked()
{
    QFileDialog dlg(this);
    dlg.setFileMode(QFileDialog::Directory);
    dlg.setOption(QFileDialog::ShowDirsOnly, true);
    dlg.setViewMode(QFileDialog::Detail);
    dlg.setWindowModality(Qt::ApplicationModal);
    dlg.setModal(true);

    if(dlg.exec() == QDialog::Rejected)
        return;

    QStringList sDirs = dlg.selectedFiles();
    QStringList sImportImgList;
    for(int iDirIndex = 0; iDirIndex < sDirs.size(); ++iDirIndex)
    {
        QDir dirEntry(sDirs[iDirIndex]);
        HyGlobal::RecursiveFindOfFileExt("png", sImportImgList, dirEntry);
    }

    if(sImportImgList.empty() == false)
        m_pModel->Repack(QSet<int>(), m_pModel->ImportImages(sImportImgList));
    
    RefreshLcds();
}

void AtlasWidget::on_btnSettings_clicked()
{
    DlgAtlasGroupSettings *pDlg = new DlgAtlasGroupSettings(m_pModel->GetPackerSettings());
    if(pDlg->GetName().isEmpty())
        pDlg->SetName("Atlas Group");

    pDlg->DataToWidgets();
    if(QDialog::Accepted == pDlg->exec())
    {
        pDlg->WidgetsToData();  // Save the changes

        if(pDlg->IsSettingsDirty())
            m_pModel->RepackAll();
        else if(pDlg->IsNameChanged())
            m_pModel->WriteMetaSettings();
    }
    else
        pDlg->DataToWidgets();  // Reverts changes made
    
    RefreshLcds();
}

void AtlasWidget::on_actionDeleteImages_triggered()
{
    QList<QTreeWidgetItem *> selectedTreeItemList = ui->atlasList->selectedItems();

    QList<QTreeWidgetItem *> selectedFrameList;
    QList<QTreeWidgetItem *> selectedFilterList;

    GetSelectedItemsRecursively(selectedTreeItemList, selectedFrameList, selectedFilterList);
    
    // First loop through and check to see if any links are present, and abort if dependecies are found
    for(int i = 0; i < selectedFrameList.count(); ++i)
    {
        AtlasFrame *pFrame = selectedFrameList[i]->data(0, Qt::UserRole).value<AtlasFrame *>();
        QSet<ProjectItem *> sLinks = pFrame->GetLinks();
        if(sLinks.empty() == false)
        {
            QString sMessage = "'" % pFrame->GetName() % "' image cannot be deleted because it is in use by the following items: \n\n";
            for(QSet<ProjectItem *>::iterator LinksIter = sLinks.begin(); LinksIter != sLinks.end(); ++LinksIter)
                sMessage.append(HyGlobal::ItemName(HyGlobal::GetCorrespondingDirItem((*LinksIter)->GetType())) % "/" % (*LinksIter)->GetName(true) % "\n");

            HyGuiLog(sMessage, LOGTYPE_Warning);
            return;
        }
    }

    if(selectedFrameList.size() > 0)
    {
        if(QMessageBox::No == QMessageBox::question(MainWindow::GetInstance(), "Confirm delete", "Do you want to delete " % QString::number(selectedFrameList.size()) % " frames?", QMessageBox::Yes, QMessageBox::No))
            return;
    }

    // No dependencies found, resume with deleting
    QSet<int> affectedTextureIndexSet;
    for(int i = 0; i < selectedFrameList.count(); ++i)
    {
        AtlasFrame *pFrame = selectedFrameList[i]->data(0, Qt::UserRole).value<AtlasFrame *>();
        affectedTextureIndexSet.insert(pFrame->GetTextureIndex());

        m_pModel->RemoveFrame(pFrame);
        delete selectedFrameList[i];
    }

    if(affectedTextureIndexSet.empty() == false)
        m_pModel->Repack(affectedTextureIndexSet, QSet<AtlasFrame *>());

    // Delete the selected filters. When deleting a parent of another filter, it will delete its children too.
    // Make sure to just delete children filters first or else it will crash, so process 'selectedFilterList' backwards
    for(int i = selectedFilterList.size() - 1; i >= 0; --i)
        delete selectedFilterList[i];
    
    RefreshLcds();
}

void AtlasWidget::on_actionReplaceImages_triggered()
{
    QSet<int> affectedTextureIndexSet;

    QList<QTreeWidgetItem *> selectedAtlasTreeItemList = ui->atlasList->selectedItems();

    // Store a list of the frames, since 'selectedAtlasTreeItemList' will become invalid within Refresh()
    QList<AtlasFrame *> selectedFrameList;
    for(int i = 0; i < selectedAtlasTreeItemList.count(); ++i)
        selectedFrameList.append(selectedAtlasTreeItemList[i]->data(0, Qt::UserRole).value<AtlasFrame *>());

    QFileDialog dlg(this);

    if(selectedFrameList.count() == 1)
    {
        dlg.setFileMode(QFileDialog::ExistingFile);
        dlg.setWindowTitle("Select an image as the replacement");
    }
    else
    {
        dlg.setFileMode(QFileDialog::ExistingFiles);
        dlg.setWindowTitle("Select " % QString::number(selectedFrameList.count()) % " images as replacements");
    }
    dlg.setWindowModality(Qt::ApplicationModal);
    dlg.setModal(true);
    QStringList sFilterList;
    sFilterList << "*.png" << "*.*";
    dlg.setNameFilters(sFilterList);

    QStringList sImportImgList;
    do
    {
        if(dlg.exec() == QDialog::Rejected)
            return;

        sImportImgList = dlg.selectedFiles();

        if(sImportImgList.count() != selectedFrameList.count())
            HyGuiLog("You must select " % QString::number(selectedFrameList.count()) % " images", LOGTYPE_Warning);
    }
    while(sImportImgList.count() != selectedFrameList.count());

    for(int i = 0; i < selectedFrameList.count(); ++i)
    {
        HyGuiLog("Replacing: " % selectedFrameList[i]->GetName() % " -> " % sImportImgList[i], LOGTYPE_Info);

        QFileInfo fileInfo(sImportImgList[i]);
        QImage newImage(fileInfo.absoluteFilePath());

        affectedTextureIndexSet.insert(selectedFrameList[i]->GetTextureIndex());

        m_pModel->ReplaceFrame(selectedFrameList[i], fileInfo.fileName(), newImage, false);
    }

    m_pModel->Repack(affectedTextureIndexSet, QSet<AtlasFrame *>());

//    for(int i = 0; i < selectedFrameList.count(); ++i)
//    {
//        QSet<ProjectItem *> sLinks = selectedFrameList[i]->GetLinks();
//        for(QSet<ProjectItem *>::iterator LinksIter = sLinks.begin(); LinksIter != sLinks.end(); ++LinksIter)
//            (*LinksIter)->RelinkFrame(selectedFrameList[i]);
//    }
    
    RefreshLcds();
}

void AtlasWidget::on_actionAddFilter_triggered()
{
    AtlasTreeItem *pNewTreeItem = new AtlasTreeItem(ui->atlasList);

    DlgInputName *pDlg = new DlgInputName("Enter Atlas Group Filter Name", "New Filter");
    if(pDlg->exec() == QDialog::Accepted)
        pNewTreeItem->setText(0, pDlg->GetName());
    else
    {
        delete pDlg;
        return;
    }

    delete pDlg;

    pNewTreeItem->setIcon(0, HyGlobal::ItemIcon(ITEM_Prefix));
    pNewTreeItem->setData(0, Qt::UserRole, QVariant(QString(HYTREEWIDGETITEM_IsFilter)));

    ui->atlasList->sortItems(0, Qt::AscendingOrder);

    m_pModel->WriteMetaSettings();
}

void AtlasWidget::on_atlasList_itemSelectionChanged()
{
    m_Draw.SetSelected(ui->atlasList->selectedItems());
    int iNumSelected = ui->atlasList->selectedItems().count();

    ui->actionRename->setEnabled(iNumSelected == 1);

    ui->actionDeleteImages->setEnabled(iNumSelected != 0);
    ui->actionReplaceImages->setEnabled(iNumSelected != 0);
    
    QList<QTreeWidgetItem *> selectedItemList = ui->atlasList->selectedItems();
    for(int i = 0; i < selectedItemList.size(); ++i)
    {
        if(selectedItemList[i]->data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter)
        {
            ui->actionReplaceImages->setEnabled(false);
            break;
        }
    }
}

void AtlasWidget::OnContextMenu(const QPoint &pos)
{
    QPoint globalPos = ui->atlasList->mapToGlobal(pos);
    QTreeWidgetItem *pTreeNode = ui->atlasList->itemAt(pos);

    QMenu contextMenu;
    if(pTreeNode == NULL)
    {
        contextMenu.addAction(ui->actionAddFilter);
    }
    else
    {
        contextMenu.addAction(ui->actionDeleteImages);
        contextMenu.addAction(ui->actionReplaceImages);
        contextMenu.addAction(ui->actionRename);
    }

    QAction* selectedItem = contextMenu.exec(globalPos);
    if (selectedItem)
    {
        // which returns a QTreeWidgetItem.
       // something was chosen, do stuff
    }
    else
    {
       // nothing was chosen
    }
}

void AtlasWidget::GetSelectedItemsRecursively(QList<QTreeWidgetItem *> selectedTreeItems, QList<QTreeWidgetItem *> &frameListRef, QList<QTreeWidgetItem *> &filterListRef)
{
    for(int i = 0; i < selectedTreeItems.count(); ++i)
    {
        // First determine if any selected QTreeWidgetItem is a filter, and dig into its children if so
        if(selectedTreeItems[i]->data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter)
        {
            filterListRef.append(selectedTreeItems[i]);

            QList<QTreeWidgetItem *> filterContentsList;
            for(int j = 0; j < selectedTreeItems[i]->childCount(); ++j)
                filterContentsList.append(selectedTreeItems[i]->child(j));

            GetSelectedItemsRecursively(filterContentsList, frameListRef, filterListRef);
        }
        else
            frameListRef.append(selectedTreeItems[i]);
    }
}

void AtlasWidget::on_actionRename_triggered()
{
    QTreeWidgetItem *pSelectedItem = ui->atlasList->selectedItems()[0];

    DlgInputName *pDlg = new DlgInputName("Rename " % pSelectedItem->text(0), pSelectedItem->text(0));
    if(pDlg->exec() == QDialog::Accepted)
    {
        if(pSelectedItem->data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter)
            pSelectedItem->setText(0, pDlg->GetName());
        else
        {
            AtlasFrame *pFrame = pSelectedItem->data(0, Qt::UserRole).value<AtlasFrame *>();
            pFrame->SetName(pDlg->GetName());

            m_pModel->WriteMetaSettings();
        }
    }

    delete pDlg;
}
