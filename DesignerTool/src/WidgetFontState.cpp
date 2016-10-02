#include "WidgetFontState.h"
#include "ui_WidgetFontState.h"

#include "ItemFontCmds.h"

#include <QStandardPaths>

WidgetFontState::WidgetFontState(QWidget *parent, QList<QAction *> stateActionList) :   QWidget(parent),
                                                                                        m_iPrevFontCmbIndex(0),
                                                                                        ui(new Ui::WidgetFontState)
{
    ui->setupUi(this);

    ui->btnAddLayer->setDefaultAction(FindAction(stateActionList, "actionAddLayer"));
    ui->btnRemoveLayer->setDefaultAction(FindAction(stateActionList, "actionRemoveLayer"));
    ui->btnOrderLayerUp->setDefaultAction(FindAction(stateActionList, "actionOrderLayerUpwards"));
    ui->btnOrderLayerDown->setDefaultAction(FindAction(stateActionList, "actionOrderLayerDownwards"));

    m_pFontModel = new WidgetFontModel(this);

    ui->stagesView->setModel(m_pFontModel);
    ui->stagesView->resize(ui->stagesView->size());
    ui->stagesView->setItemDelegate(new WidgetFontDelegate(m_pOwner, this));

    // Populate the font list combo box
    QStringList sFontPaths = QStandardPaths::standardLocations(QStandardPaths::FontsLocation);
    sFontPaths.append(m_FontMetaDir.absolutePath());
    ui->cmbFontList->clear();
    ui->cmbFontList->blockSignals(true);
    for(int i = 0; i < sFontPaths.count(); ++i)
    {
        QDir fontDir(sFontPaths[i]);
        QStringList sFilterList;
        sFilterList << "*.ttf" << "*.otf";
        QFileInfoList fontFileInfoList = fontDir.entryInfoList(sFilterList);

        for(int j = 0; j < fontFileInfoList.count(); ++j)
        {
            ui->cmbFontList->findText(fontFileInfoList[j].fileName(), Qt::MatchFixedString);
            ui->cmbFontList->addItem(fontFileInfoList[j].fileName(), QVariant(fontFileInfoList[j].absoluteFilePath()));
        }
    }

    // Try to find Arial as default font
    int iArialIndex = ui->cmbFontList->findText("Arial.ttf", Qt::MatchFixedString);
    if(iArialIndex != -1)
        ui->cmbFontList->setCurrentIndex(iArialIndex);

    m_iPrevFontCmbIndex = ui->cmbFontList->currentIndex();
}

WidgetFontState::~WidgetFontState()
{
    delete ui;
}

void WidgetFontState::on_cmbFontList_currentIndexChanged(int index)
{
    if(ui->cmbFontList->currentIndex() == index)
        return;

    QUndoCommand *pCmd = new ItemFontCmd_FontSelection(*this, ui->cmbFontList, m_iPrevFontCmbIndex, index, m_FontMetaDir);
    m_pItemFont->GetUndoStack()->push(pCmd);

    m_iPrevFontCmbIndex = index;
}
