/**************************************************************************
 *	WgtSpineAnims.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WgtSpineAnims_H
#define WgtSpineAnims_H

#include <QWidget>

namespace Ui {
class WgtSpineAnims;
}

class WgtSpineAnims : public QWidget
{
	Q_OBJECT
	
public:
	explicit WgtSpineAnims(QWidget *pParent = nullptr);
	virtual ~WgtSpineAnims();

	
private Q_SLOTS:

private:
	Ui::WgtSpineAnims *ui;
};

#endif // WgtSpineAnims_H
