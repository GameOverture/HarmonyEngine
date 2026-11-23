/**************************************************************************
 *	DlgNumberFormat.h
 *
 *	Harmony Engine - HyEditor
 *	Copyright (c) 2026 Jason Knobler
 *
 *	GPL/LGPL License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DlgNumberFormat_H
#define DlgNumberFormat_H

#include "Global.h"
#include "HyEngine.h"
#include <QDialog>

namespace Ui {
class DlgNumberFormat;
}

class DlgNumberFormat : public QDialog
{
	Q_OBJECT
	
	HyNumberFormat		m_NumFormat;

public:
	explicit DlgNumberFormat(QString sTitle, uint32 uiSerializedFormat, QWidget *parent = 0);
	virtual ~DlgNumberFormat();

	void SyncFormatToWidgets();
	
	HyNumberFormat GetNumberFormat() const;
	
private:
	Ui::DlgNumberFormat *ui;
};

#endif // DlgNumberFormat_H
