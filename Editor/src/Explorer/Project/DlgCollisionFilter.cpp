/**************************************************************************
 *	DlgCollisionFilter.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "DlgCollisionFilter.h"
#include "ui_DlgCollisionFilter.h"
#include "Project.h"
#include "DlgInputName.h"

#include <QPushButton>

DlgCollisionFilter::DlgCollisionFilter(Project &projectRef, b2Filter initFilter, QWidget *pParent) :
	QDialog(pParent),
	ui(new Ui::DlgCollisionFilter),
	m_ProjectRef(projectRef),
	m_iRenameSelectedIndex(-1)
{
	if(m_ProjectRef.GetCollisionFilterModel()->stringList().size() != 64)
	{
		HyGuiLog("DlgCollisionFilter::DlgCollisionFilter(): Project's Collision Filter model does not have 64 entries!", LOGTYPE_Error);
		return;
	}

	ui->setupUi(this);
	setWindowIcon(QIcon(QString::fromUtf8(":/icons16x16/collision.png")));

	ui->btnAddCategory->setDefaultAction(ui->actionAddCategory);
	ui->btnAddMask->setDefaultAction(ui->actionAddMask);
	ui->btnRemoveCategory->setDefaultAction(ui->actionRemoveCategories);
	ui->btnRemoveAllCategories->setDefaultAction(ui->actionRemoveAllCategories);
	ui->btnRemoveMask->setDefaultAction(ui->actionRemoveMasks);
	ui->btnRemoveAllMasks->setDefaultAction(ui->actionRemoveAllMasks);

	ui->categoryListView->setModel(m_ProjectRef.GetCollisionFilterModel());
	ui->categoryListView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->categoryListView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(OnCategoryContextMenu(const QPoint&)));

	ui->maskListView->setModel(m_ProjectRef.GetCollisionFilterModel());
	ui->maskListView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->maskListView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(OnMaskContextMenu(const QPoint&)));

	QStringList collisionFilterNames = m_ProjectRef.GetCollisionFilterModel()->stringList();
	for(int i = 0; i < 64; ++i)
	{
		if(initFilter.categoryBits & (1ULL << i))
		{
			QListWidgetItem *pNewItem = new QListWidgetItem();
			pNewItem->setText(collisionFilterNames[i]);
			pNewItem->setData(Qt::UserRole, i);
			ui->setCategoryListWidget->addItem(pNewItem);
		}

		if(initFilter.maskBits & (1ULL << i))
		{
			QListWidgetItem *pNewItem = new QListWidgetItem();
			pNewItem->setText(collisionFilterNames[i]);
			pNewItem->setData(Qt::UserRole, i);
			ui->setMaskListWidget->addItem(pNewItem);
		}
	}
	ui->sbGroupIndex->setValue(initFilter.groupIndex);

	Refresh();
}

/*virtual*/ DlgCollisionFilter::~DlgCollisionFilter()
{
	delete ui;
}

b2Filter DlgCollisionFilter::GetB2Filter() const
{
	b2Filter filter;

	for(int i = 0; i < ui->setCategoryListWidget->count(); ++i)
	{
		QListWidgetItem *pItem = ui->setCategoryListWidget->item(i);
		int nBitIndex = pItem->data(Qt::UserRole).toInt();
		filter.categoryBits |= (1ULL << nBitIndex);
	}
	for(int i = 0; i < ui->setMaskListWidget->count(); ++i)
	{
		QListWidgetItem *pItem = ui->setMaskListWidget->item(i);
		int nBitIndex = pItem->data(Qt::UserRole).toInt();
		filter.maskBits |= (1ULL << nBitIndex);
	}
	filter.groupIndex = ui->sbGroupIndex->value();

	return filter;
}

void DlgCollisionFilter::on_categoryListView_clicked(const QModelIndex &index)
{
	Refresh();
}

void DlgCollisionFilter::on_categoryListView_doubleClicked(const QModelIndex &index)
{
	Refresh();
	ui->actionAddCategory->trigger();
}

void DlgCollisionFilter::on_maskListView_clicked(const QModelIndex &index)
{
	Refresh();
}

void DlgCollisionFilter::on_maskListView_doubleClicked(const QModelIndex &index)
{
	Refresh();
	ui->actionAddMask->trigger();
}

void DlgCollisionFilter::on_btnAddCategory_clicked()
{
	if(ui->categoryListView->currentIndex().isValid() == false)
	{
		HyGuiLog("DlgCollisionFilter::on_btnAddCategory_clicked(): No category selected to add!", LOGTYPE_Error);
		return;
	}

	QStringList collisionFilterNames = m_ProjectRef.GetCollisionFilterModel()->stringList();
	QModelIndexList selectedIndices = ui->categoryListView->selectionModel()->selectedIndexes();

	for(int i = 0; i < selectedIndices.size(); ++i)
	{
		int iIndex = selectedIndices[i].row();

		// Check if any items have this index already
		QList<QListWidgetItem *> itemList = ui->setCategoryListWidget->items(nullptr);

		bool bContains = false;
		for(auto *pItem : itemList)
		{
			if(pItem->data(Qt::UserRole).toInt() == iIndex)
			{
				bContains = true;
				break;
			}
		}
		if(bContains == false)
		{
			QListWidgetItem *pNewItem = new QListWidgetItem();
			pNewItem->setText(collisionFilterNames[iIndex]);
			pNewItem->setData(Qt::UserRole, iIndex);
			ui->setCategoryListWidget->addItem(pNewItem);
		}
	}

	Refresh();
}

void DlgCollisionFilter::on_btnAddMask_clicked()
{
	if(ui->maskListView->currentIndex().isValid() == false)
	{
		HyGuiLog("DlgCollisionFilter::on_btnAddMask_clicked(): No mask selected to add!", LOGTYPE_Error);
		return;
	}

	QStringList collisionFilterNames = m_ProjectRef.GetCollisionFilterModel()->stringList();
	QModelIndexList selectedIndices = ui->maskListView->selectionModel()->selectedIndexes();

	for(int i = 0; i < selectedIndices.size(); ++i)
	{
		int iIndex = selectedIndices[i].row();

		// Check if any items have this index already
		QList<QListWidgetItem *> itemList = ui->setMaskListWidget->items(nullptr);

		bool bContains = false;
		for(auto *pItem : itemList)
		{
			if(pItem->data(Qt::UserRole).toInt() == iIndex)
			{
				bContains = true;
				break;
			}
		}
		if(bContains == false)
		{
			QListWidgetItem *pNewItem = new QListWidgetItem();
			pNewItem->setText(collisionFilterNames[iIndex]);
			pNewItem->setData(Qt::UserRole, iIndex);
			ui->setMaskListWidget->addItem(pNewItem);
		}
	}

	Refresh();
}

void DlgCollisionFilter::on_btnRemoveCategory_clicked()
{
	QList<QListWidgetItem *> selectedItemList = ui->setCategoryListWidget->selectedItems();
	if(selectedItemList.empty())
	{
		HyGuiLog("DlgCollisionFilter::on_btnRemoveCategory_clicked(): No category selected to remove!", LOGTYPE_Error);
		return;
	}
	for(QListWidgetItem *pItem : selectedItemList)
		delete pItem;
}

void DlgCollisionFilter::on_btnRemoveAllCategories_clicked()
{
	ui->setCategoryListWidget->clear();
}

void DlgCollisionFilter::on_btnRemoveMask_clicked()
{
	QList<QListWidgetItem *> selectedItemList = ui->setMaskListWidget->selectedItems();
	if(selectedItemList.empty())
	{
		HyGuiLog("DlgCollisionFilter::on_btnRemoveMask_clicked(): No category selected to remove!", LOGTYPE_Error);
		return;
	}
	for(QListWidgetItem *pItem : selectedItemList)
		delete pItem;
}

void DlgCollisionFilter::on_btnRemoveAllMasks_clicked()
{
	ui->setMaskListWidget->clear();
}

void DlgCollisionFilter::on_actionRenameCategory_triggered()
{
	DlgInputName *pDlg = new DlgInputName("Rename Category", m_ProjectRef.GetCollisionFilterModel()->stringList()[m_iRenameSelectedIndex], HyGlobal::FreeFormValidator(), nullptr, nullptr);
	if(pDlg->exec() == QDialog::Accepted)
	{
		m_ProjectRef.SetCollisionFilter(m_iRenameSelectedIndex, pDlg->GetName());

		// Also update the widget views
		for(int i = 0; i < ui->setCategoryListWidget->count(); ++i)
		{
			QListWidgetItem *pItem = ui->setCategoryListWidget->item(i);
			if(pItem->data(Qt::UserRole).toInt() == m_iRenameSelectedIndex)
				pItem->setText(pDlg->GetName());
		}
		for(int i = 0; i < ui->setMaskListWidget->count(); ++i)
		{
			QListWidgetItem *pItem = ui->setMaskListWidget->item(i);
			if(pItem->data(Qt::UserRole).toInt() == m_iRenameSelectedIndex)
				pItem->setText(pDlg->GetName());
		}
	}
	delete pDlg;
}

void DlgCollisionFilter::OnCategoryContextMenu(const QPoint &ptLocal)
{
	QModelIndex index = ui->categoryListView->indexAt(ptLocal);
	if(index.isValid())
	{
		m_iRenameSelectedIndex = index.row();
		QMenu menu;
		menu.addAction(ui->actionRenameCategory);
		menu.exec(ui->categoryListView->mapToGlobal(ptLocal));
	}
}

void DlgCollisionFilter::OnMaskContextMenu(const QPoint &ptLocal)
{
	QModelIndex index = ui->maskListView->indexAt(ptLocal);
	if(index.isValid())
	{
		m_iRenameSelectedIndex = index.row();
		QMenu menu;
		menu.addAction(ui->actionRenameCategory);
		menu.exec(ui->maskListView->mapToGlobal(ptLocal));
	}
}

void DlgCollisionFilter::Refresh()
{
	QModelIndexList selectedIndices = ui->categoryListView->selectionModel()->selectedIndexes();
	ui->actionAddCategory->setEnabled(selectedIndices.size() > 0);

	selectedIndices = ui->maskListView->selectionModel()->selectedIndexes();
	ui->actionAddMask->setEnabled(selectedIndices.size() > 0);

	ui->actionRemoveCategories->setEnabled(ui->setCategoryListWidget->selectedItems().size() > 0);
	ui->actionRemoveAllCategories->setEnabled(ui->setCategoryListWidget->count() > 0);

	ui->actionRemoveMasks->setEnabled(ui->setMaskListWidget->selectedItems().size() > 0);
	ui->actionRemoveAllMasks->setEnabled(ui->setMaskListWidget->count() > 0);

	ErrorCheck();
}

void DlgCollisionFilter::ErrorCheck()
{
	bool bIsError = false;
	//do
	//{
	//	if(ui->listView->currentIndex().isValid() == false)
	//	{
	//		ui->lblError->setText("No surface material selected");
	//		bIsError = true;
	//		break;
	//	}
	//}while(false);

	if(bIsError)
		ui->lblError->setStyleSheet("QLabel { background-color : red; color : black; }");
	else
	{
		ui->lblError->setStyleSheet("QLabel { color : black; }");
		ui->lblError->setText("");
	}
	ui->lblError->setVisible(bIsError);
	ui->buttonBox->button(QDialogButtonBox::Ok);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!bIsError);
}
