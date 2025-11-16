/**************************************************************************
 *	WgtTileSetTerrainSet.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WgtTileSetTerrainSet_H
#define WgtTileSetTerrainSet_H

#include "Global.h"
#include "IWgtTileSetItem.h"

#include <QLineEdit>
#include <QLabel>
#include <QFrame>
#include <QToolButton>
#include <QPushButton>

namespace Ui {
class WgtTileSetTerrainSet;
}

class WgtTileSetTerrain;

class WgtTileSetTerrainSet : public IWgtTileSetItem
{
	Q_OBJECT

	Ui::WgtTileSetTerrainSet *		ui;

	QList<WgtTileSetTerrain *>		m_TerrainList;

public:
	WgtTileSetTerrainSet(AuxTileSet *pAuxTileSet, QJsonObject initDataObj, QWidget* pParent = nullptr);
	virtual ~WgtTileSetTerrainSet();

	virtual void Init(QJsonObject serializedObj) override;
	virtual QJsonObject SerializeCurrentWidgets() override;

	void CmdSet_AllocTerrain(QJsonObject initDataObj);
	void CmdSet_DeleteTerrain(QUuid uuid);
	void CmdSet_OrderTerrain(QUuid uuid, int iNewIndex);

	QList<WgtTileSetTerrain *> GetTerrains() const;

	void SetOrderBtns(bool bUpEnabled, bool bDownEnabled);

protected:
	virtual QFrame *GetBorderFrame() const override;

private Q_SLOTS:
	void on_actionDelete_triggered();
	void on_actionUpward_triggered();
	void on_actionDownward_triggered();
	void on_btnAddTerrain_clicked();
};

class WgtTileSetTerrain : public IWgtTileSetItem
{
	WgtTileSetTerrainSet *m_pParentTerrainSet;
	QFrame *m_pBorderFrame;

	QLabel *m_pLblName;
	QLineEdit *m_pTxtName;
	QPushButton *m_pBtnColor;
	QSpacerItem *m_pSpacer;
	QToolButton *m_pBtnMoveUp;
	QToolButton *m_pBtnMoveDown;
	QToolButton *m_pBtnDelete;

public:
	WgtTileSetTerrain(AuxTileSet *pAuxTileSet, WgtTileSetTerrainSet *pParentTerrainSet, QJsonObject initDataObj, QWidget *pParent = nullptr) :
		IWgtTileSetItem(TILESETWGT_Terrain, initDataObj, pAuxTileSet, pParent),
		m_pParentTerrainSet(pParentTerrainSet)
	{
		m_pBorderFrame = new QFrame(this);
		m_pBorderFrame->setFrameStyle(QFrame::Box | QFrame::Plain);
		m_pBorderFrame->setLineWidth(1);
		m_pBorderFrame->setEnabled(false);
		QHBoxLayout *pMainLayout = new QHBoxLayout(m_pBorderFrame);
		pMainLayout->setContentsMargins(2, 2, 2, 2);
		pMainLayout->setSpacing(4);

		m_pLblName = new QLabel("Terrain Name", m_pBorderFrame);
		pMainLayout->addWidget(m_pLblName);
		m_pTxtName = new QLineEdit(m_pBorderFrame);
		pMainLayout->addWidget(m_pTxtName);
		m_pTxtName->setText(initDataObj["name"].toString());

		m_pBtnColor = new QPushButton(m_pBorderFrame);
		m_pBtnColor->setFixedSize(24, 24);
		HyColor btnColor(initDataObj["color"].toVariant().toLongLong());
		m_pBtnColor->setPalette(QPalette(QColor(btnColor.GetRed(), btnColor.GetGreen(), btnColor.GetBlue())));
		pMainLayout->addWidget(m_pBtnColor);

		m_pSpacer = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
		pMainLayout->addItem(m_pSpacer);
		m_pBtnMoveUp = new QToolButton(m_pBorderFrame);
		m_pBtnMoveUp->setIcon(QIcon(QString::fromUtf8(":/icons16x16/generic-up.png")));
		pMainLayout->addWidget(m_pBtnMoveUp);
		m_pBtnMoveDown = new QToolButton(m_pBorderFrame);
		m_pBtnMoveDown->setIcon(QIcon(QString::fromUtf8(":/icons16x16/generic-down.png")));
		pMainLayout->addWidget(m_pBtnMoveDown);
		m_pBtnDelete = new QToolButton(m_pBorderFrame);
		m_pBtnDelete->setIcon(QIcon(QString::fromUtf8(":/icons16x16/edit-delete.png")));
		pMainLayout->addWidget(m_pBtnDelete);
		QVBoxLayout *pOuterLayout = new QVBoxLayout(this);
		pOuterLayout->setContentsMargins(0, 0, 0, 0);
		pOuterLayout->addWidget(m_pBorderFrame);
	}
	virtual ~WgtTileSetTerrain() {}

	virtual void Init(QJsonObject serializedObj) override
	{
		m_pTxtName->setText(serializedObj["name"].toString());
		HyColor color(serializedObj["color"].toVariant().toLongLong());
		m_pBtnColor->setPalette(QPalette(QColor(color.GetRed(), color.GetGreen(), color.GetBlue())));

		m_SerializedJsonObj = serializedObj;
	}

	virtual QJsonObject SerializeCurrentWidgets() override
	{
		m_SerializedJsonObj["UUID"] = m_Uuid.toString();
		m_SerializedJsonObj["terrainSetUUID"] = m_pParentTerrainSet->GetUuid().toString();
		m_SerializedJsonObj["name"] = m_pTxtName->text();
		HyColor btnColor(m_pBtnColor->palette().button().color().red(),
			m_pBtnColor->palette().button().color().green(),
			m_pBtnColor->palette().button().color().blue());
		m_SerializedJsonObj["color"] = static_cast<qint64>(btnColor.GetAsHexCode());

		return m_SerializedJsonObj;
	}

	void SetOrderBtns(bool bUpEnabled, bool bDownEnabled)
	{
		m_pBtnMoveUp->setEnabled(bUpEnabled);
		m_pBtnMoveDown->setEnabled(bDownEnabled);
	}

protected:
	virtual QFrame *GetBorderFrame() const override { return m_pBorderFrame; }
};

#endif // WgtTileSetTerrainSet_H
