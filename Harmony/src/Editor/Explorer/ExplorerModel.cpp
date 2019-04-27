#include "Global.h"
#include "ExplorerModel.h"
#include "ExplorerItem.h"
#include "Project/Project.h"
#include "Project/ProjectItem.h"

ExplorerModel::ExplorerModel()
{
}


ExplorerModel::~ExplorerModel()
{
}

Project *ExplorerModel::AddProject(const QString sNewProjectFilePath)
{
	Project *pNewProject = new Project(sNewProjectFilePath);
	if(pNewProject->HasError())
	{
		HyGuiLog("Project: " % pNewProject->GetAbsPath() % " had an error and will not be opened", LOGTYPE_Error);
		delete pNewProject;
		return nullptr;
	}

	m_ProjectList.push_back(pNewProject);

	HyGuiLog("Opening project: " % pNewProject->GetAbsPath(), LOGTYPE_Info);
	return pNewProject;

	// BELOW BREAKS QTABBAR and UNDOSTACK SIGNAL/SLOT CONNECTIONS (I guess because QObject must be created on main thread?.. fucking waste of time)
	//
	//MainWindow::StartLoading(MDI_Explorer);
	//ExplorerLoadThread *pNewLoadThread = new ExplorerLoadThread(sNewProjectFilePath, this);
	//connect(pNewLoadThread, &ExplorerLoadThread::LoadFinished, this, &DataExplorerWidget::OnProjectLoaded);
	//connect(pNewLoadThread, &ExplorerLoadThread::finished, pNewLoadThread, &QObject::deleteLater);
	//pNewLoadThread->start();
}

void ExplorerModel::AddItem(Project *pProj, HyGuiItemType eNewItemType, const QString sPrefix, const QString sName, bool bOpenAfterAdd, QJsonValue importValue)
{
	if(pProj == nullptr)
	{
		HyGuiLog("Could not find associated project for item: " % sPrefix % "/" % sName, LOGTYPE_Error);
		return;
	}
	if(eNewItemType == ITEM_Project)
	{
		HyGuiLog("Do not use WidgetExplorer::AddItem for projects... use AddProjectItem instead", LOGTYPE_Error);
		return;
	}

	QTreeWidgetItem *pParentTreeItem = pProj->GetTreeItem();

	if(sPrefix.isEmpty() == false)
	{
		QStringList sPathSplitList = sPrefix.split(QChar('/'));
		// Traverse down the tree and add any prefix TreeItem that doesn't exist, and finally adding this item's TreeItem
		for(int i = 0; i < sPathSplitList.size(); ++i)
		{
			bool bFound = false;
			for(int j = 0; j < pParentTreeItem->childCount(); ++j)
			{
				if(QString::compare(sPathSplitList[i], pParentTreeItem->child(j)->text(0), Qt::CaseInsensitive) == 0)
				{
					pParentTreeItem = pParentTreeItem->child(j);
					bFound = true;
					break;
				}
			}

			if(bFound == false)
			{
				// Still more directories to dig thru, so this means we're at a prefix. Add the prefix TreeItem here and continue traversing down the tree
				ExplorerItem *pPrefixItem = new ExplorerItem(*pProj, ITEM_Prefix, sPathSplitList[i], pParentTreeItem);
				pParentTreeItem = pPrefixItem->GetTreeItem();
			}
		}
	}

	if(eNewItemType == ITEM_Prefix)
	{
		/*ExplorerItem *pNewPrefixItem = */new ExplorerItem(*pProj, ITEM_Prefix, sName, pParentTreeItem);
		//pProj->SaveGameData(ITEM_Prefix, pNewPrefixItem->GetName(true), pNewPrefixItem->GetName(true));
		//pProj->WriteGameData();
	}
	else
	{
		ProjectItem *pItem = new ProjectItem(*pProj, eNewItemType, pParentTreeItem, sName, importValue, true);
		pItem->SetTreeItemSubIcon(SUBICON_New);

		if(bOpenAfterAdd)
		{
			QTreeWidgetItem *pExpandItem = pItem->GetTreeItem();
			while(pExpandItem->parent() != nullptr)
			{
				ui->treeWidget->expandItem(pExpandItem->parent());
				pExpandItem = pExpandItem->parent();
			}

			MainWindow::OpenItem(pItem);
		}

		// New items that are considered "imported" should be saved immediately since they have direct references into the atlas manager
		if(importValue.isNull() == false)
			pItem->Save();
	}

	ui->treeWidget->sortItems(0, Qt::AscendingOrder);
}

void ExplorerModel::RemoveItem(ExplorerItem *pItem)
{
	if(pItem == nullptr)
		return;

	ui->treeWidget->clearSelection();
	ui->treeWidget->blockSignals(true);
	RecursiveRemoveItem(pItem);
	ui->treeWidget->blockSignals(false);
}

/*virtual*/ Qt::DropActions ExplorerModel::supportedDropActions() const /*override*/
{
	return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}

/*virtual*/ QVariant ExplorerModel::data(const QModelIndex &index, int role = Qt::DisplayRole) const /*override*/
{
	ExplorerItem *pItem = static_cast<ExplorerItem *>(index.internalPointer());
	if(pItem == nullptr)
		return QVariant();

	switch(role)
	{
	case Qt::DisplayRole:		// The key data to be rendered in the form of text. (QString)
	case Qt::EditRole:			// The data in a form suitable for editing in an editor. (QString)
		return QVariant(pItem->GetName(false));

	case Qt::DecorationRole:	// The data to be rendered as a decoration in the form of an icon. (QColor, QIcon or QPixmap)
		if(pItem->IsProjectItem())
		{
			ProjectItem *pProjItem = static_cast<ProjectItem *>(pItem);
			if(pProjItem->IsExistencePendingSave())
				return QVariant(pItem->GetIcon(SUBICON_Pending));
			else if(pProjItem->IsSaveClean() == false)
				return QVariant(pItem->GetIcon(SUBICON_Dirty));
		}
		return QVariant(pItem->GetIcon(SUBICON_None));

	case Qt::ToolTipRole:		// The data displayed in the item's tooltip. (QString)
		return QVariant();

	case Qt::StatusTipRole:		// The data displayed in the status bar. (QString)
		return QVariant(pItem->GetName(true));

	default:
		return QAbstractItemModel::data(index, role);
	}
}

/*virtual*/ QModelIndex	ExplorerModel::index(int row, int column, const QModelIndex &parent = QModelIndex()) const /*override*/
{
	if(parent.isValid() == false)
		return createIndex(row, column, m_ProjectList[row]);
	else
	{
		ExplorerItem *pParentItem = static_cast<ExplorerItem *>(parent.internalPointer());
		return createIndex(row, column, pParentItem->GetTreeItem m_ProjectList[row]);
	}
}

// An insertRows() implementation must call beginInsertRows() before inserting new rows into the data structure, and endInsertRows() immediately afterwards.
/*virtual*/ bool ExplorerModel::insertRows(int iRow, int iCount, const QModelIndex &parentRef = QModelIndex()) /*override*/
{
	beginInsertRows(parentRef, iRow, iRow + iCount - 1);

	endInsertRows();
}

// An insertColumns() implementation must call beginInsertColumns() before inserting new columns into the data structure, and endInsertColumns() immediately afterwards.
/*virtual*/ bool ExplorerModel::insertColumns(int iColumn, int iCount, const QModelIndex &parentRef = QModelIndex()) /*override*/
{
}

// A removeRows() implementation must call beginRemoveRows() before the rows are removed from the data structure, and endRemoveRows() immediately afterwards.
/*virtual*/ bool ExplorerModel::removeRows(int iRow, int iCount, const QModelIndex &parentRef = QModelIndex()) /*override*/
{
}

// A removeColumns() implementation must call beginRemoveColumns() before the columns are removed from the data structure, and endRemoveColumns() immediately afterwards.
/*virtual*/ bool ExplorerModel::removeColumns(int iColumn, int iCount, const QModelIndex &parentRef = QModelIndex()) /*override*/
{
}

///*virtual*/ void ExplorerModel::OnSave() /*override*/
//{
//}
//
///*virtual*/ QJsonObject ExplorerModel::PopStateAt(uint32 uiIndex) /*override*/
//{
//}
//
///*virtual*/ QJsonValue ExplorerModel::GetJson() const /*override*/
//{
//}
//
///*virtual*/ QList<AtlasFrame *> ExplorerModel::GetAtlasFrames() const /*override*/
//{
//}
//
///*virtual*/ QStringList ExplorerModel::GetFontUrls() const /*override*/
//{
//}
//
///*virtual*/ void ExplorerModel::Refresh() /*override*/
//{
//}
