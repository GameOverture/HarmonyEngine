/**************************************************************************
 *	AtlasPacker.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	NOTE: Original bin-packing algorithms from Pavel Roschin (https://github.com/scriptum) Cheetah-Texture-Packer
 * 
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "AtlasPacker.h"
#include "AtlasFrame.h"
#include "BanksModel.h"

#define ATLASPACKER_MIN_AUTOSIZE 32

AtlasPacker::AtlasPacker() :
	m_iMinFillPercent(80)
{
}

AtlasPacker::~AtlasPacker()
{
}

void AtlasPacker::ClearFrames()
{
	m_PackFramesList.clear();
}

void AtlasPacker::AddFrame(QSize imageSize, QRect alphaCrop, quint32 uiChecksum, AtlasFrame *pData, QString sPath)
{
	if(imageSize.width() == 0 || imageSize.height() == 0)
	{
		HyGuiLog("AtlasPacker::AddPackFrame - Image size is invalid", LOGTYPE_Error);
		return;
	}

	PackFrame packFrame;
	packFrame.m_uiChecksum = uiChecksum;//rc_crc32(0, img.bits(), img.byteCount());
	packFrame.crop = alphaCrop; // crop(img);
	packFrame.size = imageSize; // img.size();
	packFrame.id = pData;
	packFrame.path = sPath;

	m_PackFramesList << packFrame;
}

void AtlasPacker::PackFramesToBins(BankData *pBankData)
{
	m_eSortType = static_cast<SortType>(pBankData->m_MetaObj["cmbSortOrder"].toInt());
	m_FrameMargins.Set(pBankData->m_MetaObj["sbFrameMarginLeft"].toInt(),
						pBankData->m_MetaObj["sbFrameMarginBottom"].toInt(),
						pBankData->m_MetaObj["sbFrameMarginRight"].toInt(),
						pBankData->m_MetaObj["sbFrameMarginTop"].toInt());
	m_bSquareTextures = pBankData->m_MetaObj["squareTexturesOnly"].toBool();
	m_bCropUnusedSpace = pBankData->m_MetaObj["cropUnusedSpace"].toBool();
	m_bAggressiveResize = pBankData->m_MetaObj["aggressiveResizing"].toBool();
	m_iMinFillPercent = pBankData->m_MetaObj["minimumFillRate"].toInt(80);

	HeuristicType eHeuristic = static_cast<HeuristicType>(pBankData->m_MetaObj["cmbHeuristic"].toInt());
	int iAtlasMaxWidth = pBankData->m_MetaObj["maxWidth"].toInt();
	int iAtlasMaxHeight = pBankData->m_MetaObj["maxHeight"].toInt();

	SortImages(iAtlasMaxWidth, iAtlasMaxHeight);

	m_iNumInvalid = 1;
	m_iNumDuplicates = 0;
	m_uiAreaPixels = 0;
	m_AtlasBinsList.clear();

	unsigned areaBuf = AddImgesToBins(eHeuristic, iAtlasMaxWidth, iAtlasMaxHeight);

	if(m_bCropUnusedSpace && areaBuf && !m_iNumInvalid)
	{
		CropLastImage(eHeuristic, iAtlasMaxWidth, iAtlasMaxHeight, false);
	}

	// Handle duplicate frames
	for(int i = 0; i < m_PackFramesList.size(); i++)
	{
		if(m_PackFramesList.at(i).duplicateId != NULL)
		{
			m_PackFramesList.operator [](i).pos = FindPackFrame(m_PackFramesList.at(i).duplicateId)->pos;
			m_PackFramesList.operator [](i).textureId = FindPackFrame(m_PackFramesList.at(i).duplicateId)->textureId;
			m_iNumDuplicates++;
		}
	}
}

HyMargins<int> AtlasPacker::GetFrameMargins() const
{
	return m_FrameMargins;
}

int AtlasPacker::GetNumFrames() const
{
	return m_PackFramesList.size();
}

int AtlasPacker::GetNumBins() const
{
	return m_AtlasBinsList.size();
}

PackFrame &AtlasPacker::GetPackFrame(int iIndex)
{
	return m_PackFramesList[iIndex];
}

QSize AtlasPacker::GetBinDimensions(int iIndex) const
{
	return m_AtlasBinsList[iIndex];
}

void AtlasPacker::UpdateCrop()
{
	for(int i = 0; i < m_PackFramesList.size(); ++i)
	{
		m_PackFramesList[i].crop = HyGlobal::AlphaCropImage(QImage(m_PackFramesList[i].path));
	}
}

void AtlasPacker::removeId(void *data)
{
	for(int k = 0; k < m_PackFramesList.count(); k++)
	{
		if(m_PackFramesList.at(k).id == data)
		{
			m_PackFramesList.removeAt(k);
			break;
		}
	}
}

void AtlasPacker::SortImages(int w, int h)
{
	// Clear all duplicates
	for(int i = 0; i < m_PackFramesList.count(); ++i)
		m_PackFramesList[i].duplicateId = nullptr;
	
	// Recalculate duplicates
	for(int i = 0; i < m_PackFramesList.count(); ++i)
	{
		for(int k = i + 1; k < m_PackFramesList.count(); k++)
		{
			if(m_PackFramesList[k].duplicateId == nullptr &&
			   m_PackFramesList[i].m_uiChecksum == m_PackFramesList[k].m_uiChecksum &&
			   m_PackFramesList[i].size == m_PackFramesList[k].size &&
			   m_PackFramesList[i].crop == m_PackFramesList[k].crop)
			{
				m_PackFramesList[k].duplicateId = m_PackFramesList[i].id;
			}
		}
	}

	m_uiNeededAreaPixels = 0;

	QSize size;
	for(int i = 0; i < m_PackFramesList.size(); i++)
	{
		m_PackFramesList.operator [](i).pos = QPoint(999999, 999999);
		size = m_PackFramesList.at(i).crop.size();
		
		if(size.width() != 0 && size.height() != 0)
		{
			if(size.width() == w)
				size.setWidth(size.width() - m_FrameMargins.left - m_FrameMargins.right - 2);

			if(size.height() == h)
				size.setHeight(size.height() - m_FrameMargins.top - m_FrameMargins.bottom - 2);

			size += QSize(m_FrameMargins.left + m_FrameMargins.right + 2,
						  m_FrameMargins.top + m_FrameMargins.bottom + 2);
		}
	
		m_PackFramesList.operator [](i).sizeCurrent = size;
		if(m_PackFramesList.at(i).duplicateId == nullptr)
			m_uiNeededAreaPixels += size.width() * size.height();
	}

	// Sort packer frames
	switch(m_eSortType)
	{
	case SORTTYPE_LargestEdge:
		std::sort(m_PackFramesList.begin(), m_PackFramesList.end(),
			[](const PackFrame &i1, const PackFrame &i2)
			{
				int first = i1.sizeCurrent.height() > i1.sizeCurrent.width() ? i1.sizeCurrent.height() : i1.sizeCurrent.width();
				int second = i2.sizeCurrent.height() > i2.sizeCurrent.width() ? i2.sizeCurrent.height() : i2.sizeCurrent.width();
				if(first == second)
					return i1.sizeCurrent.height() * i1.sizeCurrent.width() > i2.sizeCurrent.height() * i2.sizeCurrent.width(); // Fall back to 'area compare'

				return first > second;
			});
		break;

	case SORTTYPE_TotalArea:
		std::sort(m_PackFramesList.begin(), m_PackFramesList.end(),
			[](const PackFrame &i1, const PackFrame &i2)
			{
				return i1.sizeCurrent.height() * i1.sizeCurrent.width() > i2.sizeCurrent.height() * i2.sizeCurrent.width();
			});
		break;

	case SORTTYPE_Width:
		std::sort(m_PackFramesList.begin(), m_PackFramesList.end(),
			[](const PackFrame &i1, const PackFrame &i2)
			{
				return (i1.sizeCurrent.width() << 10) + i1.sizeCurrent.height() > (i2.sizeCurrent.width() << 10) + i2.sizeCurrent.height();
			});
		break;

	case SORTTYPE_Height:
		std::sort(m_PackFramesList.begin(), m_PackFramesList.end(),
			[](const PackFrame &i1, const PackFrame &i2)
			{
				return (i1.sizeCurrent.height() << 10) + i1.sizeCurrent.width() > (i2.sizeCurrent.height() << 10) + i2.sizeCurrent.width();
			});
		break;

	case SORTTYPE_NoSorting:
		break;

	default:
		HyGuiLog("AtlasPacker::SortImages() - Unknown sort type", LOGTYPE_Error);
		break;
	}
}

int AtlasPacker::FillBin(HeuristicType eHeuristic, int w, int h, int binIndex)
{
	int areaBuf = 0;
	FramesAssembler framesAssembler(eHeuristic, w, h);

	for(int i = 0; i < m_PackFramesList.size(); i++)
	{
		if(QPoint(999999, 999999) != m_PackFramesList.at(i).pos)
		{
			continue;
		}
		if(m_PackFramesList.at(i).duplicateId == nullptr)
		{
			m_PackFramesList.operator [](i).pos = framesAssembler.InsertPackFrame(&m_PackFramesList.operator [](i));
			m_PackFramesList.operator [](i).textureId = binIndex;
			if(QPoint(999999, 999999) == m_PackFramesList.at(i).pos)
			{
				m_iNumInvalid++;
			}
			else
			{
				areaBuf += m_PackFramesList.at(i).sizeCurrent.width() * m_PackFramesList.at(i).sizeCurrent.height();
				m_uiAreaPixels += m_PackFramesList.at(i).sizeCurrent.width() * m_PackFramesList.at(i).sizeCurrent.height();
			}
		}
	}
	return areaBuf;
}

void AtlasPacker::ClearBin(int binIndex)
{
	for(int i = 0; i < m_PackFramesList.size(); i++)
	{
		if(m_PackFramesList.at(i).textureId == binIndex)
		{
			m_uiAreaPixels -= m_PackFramesList.at(i).sizeCurrent.width() * m_PackFramesList.at(i).sizeCurrent.height();
			m_PackFramesList.operator [](i).pos = QPoint(999999, 999999);
		}
	}
}

unsigned AtlasPacker::AddImgesToBins(HeuristicType eHeuristic, int w, int h)
{
	int binIndex = m_AtlasBinsList.count() - 1;
	unsigned areaBuf = 0;
	unsigned lastAreaBuf = 0;
	do
	{
		m_iNumInvalid = 0;
		m_AtlasBinsList << QSize(w, h);
		lastAreaBuf = FillBin(eHeuristic, w, h, ++binIndex);
		if(!lastAreaBuf)
		{
			m_AtlasBinsList.removeLast();
		}
		areaBuf += lastAreaBuf;
	} while(m_iNumInvalid && lastAreaBuf);
	return areaBuf;
}

void AtlasPacker::CropLastImage(HeuristicType eHeuristic, int w, int h, bool wh)
{
	m_iNumInvalid = 0;
	QList<PackFrame> last_images = m_PackFramesList;
	QList<QSize> last_bins = m_AtlasBinsList;
	quint64 last_area = m_uiAreaPixels;

	m_AtlasBinsList.removeLast();
	ClearBin(m_AtlasBinsList.count());

	if(m_bSquareTextures)
	{
		w /= 2;
		h /= 2;
	}
	else
	{
		if(wh)
			w /= 2;
		else
			h /= 2;
		wh = !wh;
	}

	int binIndex = m_AtlasBinsList.count();
	m_iNumInvalid = 0;
	m_AtlasBinsList << QSize(w, h);
	FillBin(eHeuristic, w, h, binIndex);
	if(m_iNumInvalid)
	{
		m_PackFramesList = last_images;
		m_AtlasBinsList = last_bins;
		m_uiAreaPixels = last_area;
		m_iNumInvalid = 0;
		if(m_bSquareTextures)
		{
			w *= 2;
			h *= 2;
		}
		else
		{
			if(!wh)
				w *= 2;
			else
				h *= 2;

			wh = !wh;
		}
		if(m_bAggressiveResize)
		{
			float rate = GetFillRate();
			if((rate < (static_cast<float>(m_iMinFillPercent) / 100.f)) &&
				((w > ATLASPACKER_MIN_AUTOSIZE) && (h > ATLASPACKER_MIN_AUTOSIZE)))
			{
				DivideLastImage(eHeuristic, w, h, wh);
				if(GetFillRate() <= rate)
				{
					m_PackFramesList = last_images;
					m_AtlasBinsList = last_bins;
					m_uiAreaPixels = last_area;
				}
			}
		}
	}
	else
	{
		CropLastImage(eHeuristic, w, h, wh);
	}
}

void AtlasPacker::DivideLastImage(HeuristicType eHeuristic, int w, int h, bool wh)
{
	m_iNumInvalid = 0;
	QList<PackFrame> last_images = m_PackFramesList;
	QList<QSize> last_bins = m_AtlasBinsList;
	quint64 last_area = m_uiAreaPixels;

	m_AtlasBinsList.removeLast();
	ClearBin(m_AtlasBinsList.count());

	if(m_bSquareTextures)
	{
		w /= 2;
		h /= 2;
	}
	else
	{
		if(wh)
		{
			w /= 2;
		}
		else
		{
			h /= 2;
		}
		wh = !wh;
	}
	AddImgesToBins(eHeuristic, w, h);
	if(m_iNumInvalid)
	{
		m_PackFramesList = last_images;
		m_AtlasBinsList = last_bins;
		m_uiAreaPixels = last_area;
		m_iNumInvalid = 0;
	}
	else
	{
		CropLastImage(eHeuristic, w, h, wh);
	}
}

float AtlasPacker::GetFillRate()
{
	quint64 binArea = 0;
	for(int i = 0; i < m_AtlasBinsList.count(); i++)
	{
		binArea += m_AtlasBinsList.at(i).width() * m_AtlasBinsList.at(i).height();
	}
	return (float)((double)m_uiAreaPixels / (double)binArea);
}

const PackFrame *AtlasPacker::FindPackFrame(void *data)
{
	for(int i = 0; i < m_PackFramesList.count(); i++)
	{
		if(data == m_PackFramesList.at(i).id)
		{
			return &m_PackFramesList.at(i);
		}
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QPoint AtlasPacker::FramesAssembler::InsertPackFrame(PackFrame *pPackFrame)
{
	int i;
	int min = 999999999, mini = -1, m;
	QSize img = pPackFrame->sizeCurrent;
	//    if(img.width() == w) img.setWidth(img.width() - border->l - border->r);
	//    if(img.height() == h) img.setHeight(img.height() - border->t - border->b);
	if(img.width() <= 0 || img.height() <= 0)
	{
		return QPoint(0, 0);
	}
	bool leftNeighbor = false, rightNeighbor = false;
	bool _leftNeighbor = false, _rightNeighbor = false;
	for(i = 0; i < F.size(); i++)
	{
		if((F.at(i).r.width() >= img.width() && F.at(i).r.height() >= img.height()) ||
			(F.at(i).r.width() >= img.height() && F.at(i).r.height() >= img.width()))
		{
			m = 0;
			if((F.at(i).r.width() >= img.height() && F.at(i).r.height() >= img.width()) &&
				!(F.at(i).r.width() >= img.width() && F.at(i).r.height() >= img.height()))
			{
				//if(rotation == 0)
				{
					continue;
				}
				//img.transpose();
				//rotated = true;
				//m += img.height();
			}
			switch(m_eHeuristicType)
			{
			case HEURISTIC_None:
				mini = i;
				i = F.size();
				continue;

			case HEURISTIC_TopLeft:
				m += F.at(i).r.y();
				_leftNeighbor = _rightNeighbor = false;
				for(int k = 0; k < R.size(); k++)
				{
					if(qAbs(R.at(k).y() + R.at(k).height() / 2 - F.at(i).r.y() - F.at(
						i).r.height() / 2) <
						qMax(R.at(k).height(), F.at(i).r.height()) / 2)
					{
						if(R.at(k).x() + R.at(k).width() == F.at(i).r.x())
						{
							m -= 5;
							_leftNeighbor = true;
						}
						if(R.at(k).x() == F.at(i).r.x() + F.at(i).r.width())
						{
							m -= 5;
							_rightNeighbor = true;
						}
					}
				}
				if(_leftNeighbor || _rightNeighbor == false)
				{
					if(F.at(i).r.x() + F.at(i).r.width() == m_iWidth)
					{
						m -= 1;
						_rightNeighbor = true;
					}
					if(F.at(i).r.x() == 0)
					{
						m -= 1;
						_leftNeighbor = true;
					}
				}
				break;
			case HEURISTIC_BestArea:
				m += F.at(i).r.width() * F.at(i).r.height();
				break;
			case HEURISTIC_BestShortSide:
				m += qMin(F.at(i).r.width() - img.width(), F.at(i).r.height() - img.height());
				break;
			case HEURISTIC_BestLongSide:
				m += qMax(F.at(i).r.width() - img.width(), F.at(i).r.height() - img.height());
				break;
			case HEURISTIC_MinimumWidth:
				m += F.at(i).r.width();
				break;
			case HEURISTIC_MimimumHeight:
				m += F.at(i).r.height();
			}
			if(m < min)
			{
				min = m;
				mini = i;
				leftNeighbor = _leftNeighbor;
				rightNeighbor = _rightNeighbor;
			}
		}
	}

	if(mini >= 0)
	{
		i = mini;
		MaxRectsNode n0;
		QRect buf(F.at(i).r.x(), F.at(i).r.y(), img.width(), img.height());
		if(m_eHeuristicType == HEURISTIC_TopLeft)
		{
			if(!leftNeighbor && F.at(i).r.x() != 0 &&
				F.at(i).r.width() + F.at(i).r.x() == m_iWidth)
			{
				buf = QRect(m_iWidth - img.width(), F.at(i).r.y(), img.width(), img.height());
			}
			if(!leftNeighbor && rightNeighbor)
			{
				buf = QRect(F.at(i).r.x() + F.at(i).r.width() - img.width(), F.at(i).r.y(),
					img.width(), img.height());
			}
		}
		n0.r = buf;
		R << buf;
		if(F.at(i).r.width() > img.width())
		{
			MaxRectsNode n;
			n.r = QRect(F.at(i).r.x() + (buf.x() == F.at(i).r.x() ? img.width() : 0),
				F.at(i).r.y(), F.at(i).r.width() - img.width(), F.at(i).r.height());
			//            n.i = NULL;
			F << n;
		}
		if(F.at(i).r.height() > img.height())
		{
			MaxRectsNode n;
			n.r = QRect(F.at(i).r.x(), F.at(i).r.y() + img.height(), F.at(i).r.width(),
				F.at(i).r.height() - img.height());
			//            n.i = NULL;
			F << n;
		}

		F.removeAt(i);
		//intersect
		for(i = 0; i < F.size(); i++)
		{
			if(F.at(i).r.intersects(n0.r))
			{
				if(n0.r.x() + n0.r.width() < F.at(i).r.x() + F.at(i).r.width())
				{
					MaxRectsNode n;
					n.r = QRect(n0.r.width() + n0.r.x(), F.at(i).r.y(),
						F.at(i).r.width() + F.at(i).r.x() - n0.r.width() - n0.r.x(),
						F.at(i).r.height());
					//                    n.i = NULL;
					F << n;
				}
				if(n0.r.y() + n0.r.height() < F.at(i).r.y() + F.at(i).r.height())
				{
					MaxRectsNode n;
					n.r = QRect(F.at(i).r.x(), n0.r.height() + n0.r.y(),
						F.at(i).r.width(), F.at(i).r.height() + F.at(i).r.y() - n0.r.height() -
						n0.r.y());
					//                    n.i = NULL;
					F << n;
				}
				if(n0.r.x() > F.at(i).r.x())
				{
					MaxRectsNode n;
					n.r = QRect(F.at(i).r.x(), F.at(i).r.y(), n0.r.x() - F.at(i).r.x(),
						F.at(i).r.height());
					//                    n.i = NULL;
					F << n;
				}
				if(n0.r.y() > F.at(i).r.y())
				{
					MaxRectsNode n;
					n.r = QRect(F.at(i).r.x(), F.at(i).r.y(), F.at(i).r.width(),
						n0.r.y() - F.at(i).r.y());
					//                    n.i = NULL;
					F << n;
				}
				F.removeAt(i);
				i--;
			}
		}

		for(i = 0; i < F.size(); i++)
		{
			for(int j = i + 1; j < F.size(); j++)
			{
				if(i != j && F.at(i).r.contains(F.at(j).r))
				{
					F.removeAt(j);
					j--;
				}
			}
		}
		return QPoint(n0.r.x(), n0.r.y());
	}
	return QPoint(999999, 999999);
}
