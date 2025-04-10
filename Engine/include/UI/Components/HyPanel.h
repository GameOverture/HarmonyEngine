/**************************************************************************
*	HyPanel.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyPanel_h__
#define HyPanel_h__

#include "Afx/HyStdAfx.h"
#include "Assets/Nodes/HyNodePath.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "Scene/Nodes/Loadables/Bodies/IHyBody2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyPrimitive2d.h"

struct HyUiTextInit
{
	HyNodePath					m_NodePath;
	HyMargins<float>			m_Margins;

	HyUiTextInit() : m_NodePath(HyNodePath()), m_Margins(0.0f, 0.0f, 0.0f, 0.0f)
	{ }
	HyUiTextInit(const HyNodePath &textNodePath) : m_NodePath(textNodePath), m_Margins(0.0f, 0.0f, 0.0f, 0.0f)
	{ }
	HyUiTextInit(const HyNodePath &textNodePath, const HyMargins<float> &textMargins) : m_NodePath(textNodePath), m_Margins(textMargins)
	{ }
};

struct HyUiPanelInit
{
	HyType						m_eNodeType;

	uint32						m_uiWidth;
	uint32						m_uiHeight;

	HyNodePath					m_NodePath;

	uint32						m_uiFrameSize;
	HyColor						m_PanelColor;
	HyColor						m_FrameColor;
	HyColor						m_TertiaryColor;

	// Constructs a 'BoundingVolume' panel with 0 width/height. The widget will ignore this panel and not use it for sizing
	HyUiPanelInit();

	// Constructs a 'BoundingVolume' panel. A bounding volume is not visible, but the widget will use it to size itself or it's ignored if either width/height is 0
	HyUiPanelInit(uint32 uiWidth, uint32 uiHeight);

	// Constructs a 'NodeItem' panel. The widget may use the node's width/height to size itself or it's ignored if nodePath is invalid
	HyUiPanelInit(HyType eNodeType, const HyNodePath &nodePath);
	HyUiPanelInit(HyType eNodeType, const HyNodePath &nodePath, uint32 uiWidth, uint32 uiHeight);

	// Constructs a 'Primitive' panel. Default HyColor values of 0xDEADBEEF will be set to a default color determined by the widget
	// Passing '0' for width/height will try to auto-size based on the widget if applicable (or it will be hidden)
	HyUiPanelInit(uint32 uiWidth, uint32 uiHeight, uint32 uiFrameSize, HyColor panelColor = HyColor(0xDE,0xAD,0xBE,0xEF), HyColor frameColor = HyColor(0xDE, 0xAD, 0xBE, 0xEF), HyColor tertiaryColor = HyColor(0xDE, 0xAD, 0xBE, 0xEF));
};

// Internal class component of UI widgets. Is a visual representation of single panel.
// Not exposed because IHyWidget's handles modifications. IHyWidget's may have multiple HyPanel's, but they all have one main one 'm_Panel'
class HyPanel : public HyEntity2d
{
	HyType						m_eNodeType;

	glm::ivec2					m_vSizeHint;	// When BV or Primitive, holds the specified size this panel should be
	glm::ivec2					m_vSizeActual;	// The current size of this panel, potentially after scaling, etc.

	struct PrimParts
	{
		uint32					m_uiFrameSize;
		
		HyColor					m_PanelColor;
		HyColor					m_FrameColor;
		HyColor					m_TertiaryColor;
		
		HyPrimitive2d			m_Frame1;
		HyPrimitive2d			m_Frame2;
		HyPrimitive2d			m_Body;

		bool					m_bIsContainer; // TODO: Construct panels differently?

		PrimParts(const HyUiPanelInit &initRef, HyPanel *pParent) :
			m_uiFrameSize(initRef.m_uiFrameSize),
			m_PanelColor(initRef.m_PanelColor),
			m_FrameColor(initRef.m_FrameColor),
			m_TertiaryColor(initRef.m_TertiaryColor),
			m_Frame1(pParent),
			m_Frame2(pParent),
			m_Body(pParent),
			m_bIsContainer(false)
		{ }
	};
	union PanelData
	{
		PrimParts *				m_pPrimParts;	// Only dynamically allocated when 'Primitive' panel type. Otherwise nullptr
		IHyBody2d *				m_pNodeItem;	// Only dynamically allocated when 'NodeItem' panel type. Otherwise nullptr
		HyShape2d *				m_pBoundingVolumeShape;
		PanelData() : m_pPrimParts(nullptr)
		{ }
	};
	PanelData					m_PanelData;

public:
	HyPanel(HyEntity2d *pParent);
	virtual ~HyPanel();

	void Setup(const HyUiPanelInit &initRef);
	HyUiPanelInit CloneInit(); // Uses currently set properties of this panel to create an equivalent HyPanelInit struct

	virtual uint32 GetState() const override;
	virtual bool SetState(uint32 uiStateIndex) override;
	virtual uint32 GetNumStates() override;

	virtual float GetWidth(float fPercent = 1.0f) override;
	virtual float GetHeight(float fPercent = 1.0f) override;
	float GetSizeDimension(int32 iDimensionIndex, float fPercent = 1.0f);

	bool IsAutoSize() const;
	glm::ivec2 GetPanelSizeHint() const;

	void SetSize(uint32 uiWidth, uint32 uiHeight);
	void SetSizeDimension(int32 iDimensionIndex, uint32 uiSize);	// This is for widgets who programmatically choose between vertical or horizontal sizing

	bool IsBoundingVolume() const;
	HyShape2d *GetBvShape() const;

	bool IsNode() const;
	IHyBody2d *GetNode();

	bool IsPrimitive() const;
	uint32 GetFrameStrokeSize() const;
	HyColor GetPanelColor() const;
	HyColor GetFrameColor() const;
	HyColor GetTertiaryColor() const;

	glm::vec2 GetBotLeftOffset();

protected:
	void ConstructPrimitives();

	void DeleteData();
};

#endif /* HyPanel_h__ */
