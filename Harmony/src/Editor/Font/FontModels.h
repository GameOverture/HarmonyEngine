/**************************************************************************
 *	FontModels.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef FONTMODELS_H
#define FONTMODELS_H

#include "ProjectItem.h"
#include "IModel.h"
#include "AtlasFrame.h"
#include "GlobalWidgetMappers.h"
#include "PropertiesTreeModel.h"

#include "freetype-gl/freetype-gl.h"

#include <QJsonArray>

class FontTypeface;

class FontStateLayersModel : public QAbstractTableModel
{
	Q_OBJECT

		struct FontLayer
	{
		const int           iUNIQUE_ID;
		FontTypeface *      pReference;

		rendermode_t        eMode;
		int                 iSize;
		float               fOutlineThickness;

		glm::vec4           vTopColor;
		glm::vec4           vBotColor;

		FontLayer(int iUniqueId, rendermode_t eRenderMode, int iSize, float fOutlineThickness) :    iUNIQUE_ID(iUniqueId),
			pReference(nullptr),
			eMode(eRenderMode),
			iSize(iSize),
			fOutlineThickness(fOutlineThickness)
		{ }
	};

	static int                      sm_iUniqueIdCounter;
	QList<FontLayer *>              m_LayerList;
	QList<QPair<int, FontLayer *> > m_RemovedLayerList; // QPair = <rowIndex, fontPtr>

	QStringList                     m_sRenderModeStringList;

public:
	enum eColumn
	{
		COLUMN_Type = 0,
		COLUMN_Thickness,
		COLUMN_DefaultColor,

		NUMCOLUMNS
	};

	FontStateLayersModel(QObject *parent);
	virtual ~FontStateLayersModel();

	QString GetRenderModeString(rendermode_t eMode) const;

	int AddNewLayer(rendermode_t eRenderMode, int iSize, float fOutlineThickness);
	void RemoveLayer(int iId);
	void ReAddLayer(int iId);

	int GetLayerId(int iRowIndex) const;
	FontTypeface *GetStageRef(int iRowIndex);

	rendermode_t GetLayerRenderMode(int iRowIndex) const;
	void SetLayerRenderMode(int iId, rendermode_t eMode);

	float GetLayerOutlineThickness(int iRowIndex) const;
	void SetLayerOutlineThickness(int iId, float fThickness);

	QColor GetLayerTopColor(int iRowIndex) const;
	QColor GetLayerBotColor(int iRowIndex) const;
	void SetLayerColors(int iId, QColor topColor, QColor botColor);

	float GetLineHeight();
	float GetLineAscender();
	float GetLineDescender();
	float GetLeftSideNudgeAmt(QString sAvailableTypefaceGlyphs);

	void MoveRowUp(int iIndex);
	void MoveRowDown(int iIndex);

	void SetFontSize(int iSize);

	void SetFontStageReference(int iRowIndex, FontTypeface *pStageRef);

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;
	virtual Qt::ItemFlags flags(const QModelIndex & index) const override;
};

struct FontTypeface
{
	uint					uiReferenceCount;

	QString					sFontPath;
	texture_font_t *		pTextureFont;
	rendermode_t			eMode;
	float					fSize;
	float					fOutlineThickness;

	FontTypeface(QString sFontFilePath, rendermode_t eRenderMode, float fSize, float fOutlineThickness) :
		uiReferenceCount(0),
		sFontPath(sFontFilePath),
		pTextureFont(nullptr),
		eMode(eRenderMode),
		fSize(fSize),
		fOutlineThickness(fOutlineThickness)
	{ }

	~FontTypeface()
	{
		if(pTextureFont)
			texture_font_delete(pTextureFont);
	}
};

class FontStateData : public IStateData
{
	FontStateLayersModel *		m_pLayersModel;
	
	SpinBoxMapper *				m_pSbMapper_Size;
	ComboBoxMapper *			m_pCmbMapper_Fonts;
	
public:
	FontStateData(int iStateIndex, IModel &modelRef, QJsonObject stateObj);
	virtual ~FontStateData();

	FontStateLayersModel *GetFontLayersModel();

	SpinBoxMapper *GetSizeMapper();
	ComboBoxMapper *GetFontMapper();

	void GetStateInfo(QJsonObject &stateObjOut);

	QString GetFontFilePath();
	float GetSize();
	
	virtual int AddFrame(AtlasFrame *pFrame) override { return 0; }
	virtual void RelinquishFrame(AtlasFrame *pFrame) override { }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FontModel : public IModel
{
	Q_OBJECT

	QDir						m_FontMetaDir;

	PropertiesTreeModel			m_TypeFacePropertiesModel;

	AtlasFrame *				m_pTrueAtlasFrame;

	QJsonArray					m_TypefaceArray;

	
	QList<FontTypeface *>		m_MasterLayerList;

	texture_atlas_t *			m_pFtglAtlas;
	unsigned char *				m_pSubAtlasPixelData;
	uint						m_uiSubAtlasBufferSize;

public:
	FontModel(ProjectItem &itemRef, QJsonObject fontObj);
	virtual ~FontModel();

	QString GetAvailableTypefaceGlyphs() const;

	QDir GetMetaDir();

	PropertiesTreeModel *GetTypefaceModel();

	QList<FontTypeface *> GetMasterStageList();

	QJsonObject GetTypefaceObj(int iTypefaceIndex);
	
	texture_atlas_t *GetFtglAtlas();
	AtlasFrame *GetAtlasFrame();
	unsigned char *GetAtlasPreviewPixelData();
	uint GetAtlasPreviewPixelDataSize();
	
	void GeneratePreview();

	virtual void OnSave() override;
	virtual QJsonObject PopStateAt(uint32 uiIndex) override;
	virtual QJsonValue GetJson() const override;
	virtual QList<AtlasFrame *> GetAtlasFrames() const override;
	virtual QStringList GetFontUrls() const override;
	virtual void Refresh() override;

private:
	QSize GetAtlasGrpSize();
};

#endif // FONTMODELS_H
