/**************************************************************************
*	TreeModelItemData.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef TREEMODELITEMDATA_H
#define TREEMODELITEMDATA_H

#include "Global.h"

#include <QObject>
#include <QTreeWidget>

class TreeModelItemData : public QObject
{
	Q_OBJECT

protected:
	const ItemType									m_eTYPE;
	const QUuid										m_UUID;
	QString											m_sName;
	bool											m_bIsProjectItem;
	bool											m_bIsAssetItem;

	// Dependant References : int = count
	QMap<TreeModelItemData *, int>					m_DependantMap;		// What ItemDatas rely on 'this'
	QMap<TreeModelItemData *, int>					m_DependeeMap;		// What ItemDatas 'this' relies on

public:
	TreeModelItemData();
	TreeModelItemData(ItemType eType, const QUuid &uuid, const QString sText);
	virtual ~TreeModelItemData();

	ItemType GetType() const;
	const QUuid &GetUuid() const;
	QString GetText() const;
	void SetText(QString sText);
	QIcon GetIcon(SubIcon eSubIcon) const;

	bool IsProjectItem() const;
	bool IsAssetItem() const;

	QList<TreeModelItemData *> GetDependants() const;
	QList<TreeModelItemData *> GetDependees() const;
	void AddDependantRef(TreeModelItemData *pDependant);
	void SubtractDependantRef(TreeModelItemData *pDependant);
	void RelinquishDependees(); // Inform all dependees of 'this' that they no longer need to consider it a dependant

private:
	void AddDependeeRef(TreeModelItemData *pDependee);
	void SubtractDependeeRef(TreeModelItemData *pDependee);
};
Q_DECLARE_METATYPE(TreeModelItemData *)
Q_DECLARE_METATYPE(const TreeModelItemData *)

QDataStream &operator<<(QDataStream &out, TreeModelItemData *const &rhs);
QDataStream &operator>>(QDataStream &in, TreeModelItemData *rhs);

#endif // TREEMODELITEMDATA_H
