/**************************************************************************
 *	AtlasPacker.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	NOTE: Original bin-packing algorithms from Pavel Roschin's (https://github.com/scriptum) Cheetah-Texture-Packer
 * 
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ATLASPACKER_H
#define ATLASPACKER_H

#include "Global.h"

enum HeuristicType
{
	// NOTE: Order matters (is serialized as int)
	HEURISTIC_None = 0,
	HEURISTIC_TopLeft,
	HEURISTIC_BestArea,
	HEURISTIC_BestShortSide,
	HEURISTIC_BestLongSide,
	HEURISTIC_MinimumWidth,
	HEURISTIC_MimimumHeight,
};

class AtlasFrame;
struct BankData;

struct PackFrame
{
	quint32								m_uiChecksum;
	int									textureId;
	AtlasFrame *						id = nullptr;
	AtlasFrame *						duplicateId = nullptr;
	QPoint								pos;
	QSize								size, sizeCurrent;
	QRect								crop;
	QString								path;

	bool								cropped;
};

class AtlasPacker
{
	QList<PackFrame>					m_PackFramesList;
	QList<QSize>						m_AtlasBinsList;

	quint64								m_uiAreaPixels;
	quint64								m_uiNeededAreaPixels;
	
	int									m_iNumInvalid;
	int									m_iNumDuplicates;

	bool								m_bCropUnusedSpace;
	bool								m_bSquareTextures;
	bool								m_bAggressiveResize;

	HyMargins<int>						m_FrameMargins;

	enum SortType
	{
		// NOTE: Order matters (is serialized as int)
		SORTTYPE_NoSorting = -1,
		SORTTYPE_LargestEdge = 0,
		SORTTYPE_TotalArea,
		SORTTYPE_Width,
		SORTTYPE_Height,
	};
	SortType							m_eSortType;

	int									m_iMinFillPercent;

	class FramesAssembler
	{
		struct MaxRectsNode
		{
			QRect						r;
			//    QSize *i; //image
			//trbl b; //border
		};
		QList<MaxRectsNode>				F;
		QList<QRect>					R;

		HeuristicType					m_eHeuristicType;
		int								m_iWidth;
		int								m_iHeight;

	public:
		FramesAssembler(HeuristicType eHeuristic, int iWidth, int iHeight) :
			m_eHeuristicType(eHeuristic),
			m_iWidth(iWidth),
			m_iHeight(iHeight)
		{
			MaxRectsNode mrn;
			mrn.r = QRect(0, 0, m_iWidth, m_iHeight);
			F << mrn;
		}

		QPoint InsertPackFrame(PackFrame *pPackFrame);
	};

public:
	AtlasPacker();
	~AtlasPacker();

	void ClearFrames();
	void AddFrame(QSize imageSize, QRect alphaCrop, quint32 uiChecksum, AtlasFrame *pData, QString sPath);
	void PackFramesToBins(BankData *pBankData);

	HyMargins<int> GetFrameMargins() const;
	int GetNumFrames() const;
	int GetNumBins() const;

	PackFrame &GetPackFrame(int iIndex);
	QSize GetBinDimensions(int iIndex) const;

private:
	unsigned AddImgesToBins(HeuristicType eHeuristic, int w, int h);

	void CropLastImage(HeuristicType eHeuristic, int w, int h, bool wh);
	void DivideLastImage(HeuristicType eHeuristic, int w, int h, bool wh);

	void UpdateCrop();

	float GetFillRate();

	void ClearBin(int binIndex);

	int FillBin(HeuristicType eHeuristic, int w, int h, int binIndex);

	void removeId(void *);

	void SortImages(int w, int h);

	const PackFrame *FindPackFrame(void *data);
};

#endif // ATLASPACKER_H
