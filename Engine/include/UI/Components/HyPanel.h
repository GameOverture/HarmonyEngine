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
#include "UI/IHyEntityUi.h"
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

	// Constructs a 'Primitive' panel. Default HyColor values of 0xDEADBE will be set to a default color determined by the widget
	// Passing '0' for width/height will try to auto-size based on the widget if applicable (or it will be hidden)
	HyUiPanelInit(uint32 uiWidth, uint32 uiHeight, uint32 uiFrameSize, HyColor panelColor = HyColor(0xDE,0xAD,0xBE), HyColor frameColor = HyColor(0xDE,0xAD,0xBE), HyColor tertiaryColor = HyColor(0xDE,0xAD,0xBE));
};

// A flexible visual representation of single panel that can be composed with node items or programmatically
class HyPanel// : public IHyEntityUi
{
	HyType						m_eNodeType;

	struct PrimParts : public HyEntity2d
	{
		uint32					m_uiFrameSize;
		
		HyColor					m_PanelColor;
		HyColor					m_FrameColor;
		HyColor					m_TertiaryColor;
		
		HyPrimitive2d			m_Frame1;
		HyPrimitive2d			m_Frame2;
		HyPrimitive2d			m_Body;

		bool					m_bIsContainer; // TODO: Construct panels differently?

		PrimParts(const HyUiPanelInit &initRef, HyEntity2d *pParent) :
			HyEntity2d(pParent),
			m_uiFrameSize(initRef.m_uiFrameSize),
			m_PanelColor(initRef.m_PanelColor),
			m_FrameColor(initRef.m_FrameColor),
			m_TertiaryColor(initRef.m_TertiaryColor),
			m_Frame1(this),
			m_Frame2(this),
			m_Body(this),
			m_bIsContainer(false)
		{ }
	};
	union PanelData
	{
		glm::vec2				m_BoundingVolumeSize;	// For 'BoundingVolume' panel type.
		PrimParts *				m_pPrimParts;			// For 'Primitive' panel type.
		IHyBody2d *				m_pNodeItem;			// For 'NodeItem' panel type.
		PanelData() : m_pPrimParts(nullptr)
		{ }
	};
	PanelData					m_PanelData;

public:
	HyPanel();
	HyPanel(const HyUiPanelInit &initRef, HyEntity2d *pParent);
	~HyPanel();

	bool IsNull() const;
	bool IsBvForPanel() const;
	bool IsItemForPanel() const;
	bool IsPrimForPanel() const;

	void Setup(const HyUiPanelInit &initRef, HyEntity2d *pParent);

	float GetWidth(float fPercent = 1.0f) const;
	float GetHeight(float fPercent = 1.0f) const;
	glm::vec2 GetSize() const;
	void SetSize(float fWidth, float fHeight);

	bool SetState(uint32 uiWidgetState);
	uint32 GetNumStates();

	bool IsVisible() const;
	void SetVisible(bool bVisible);
	HyAnimFloat *Alpha();
	
	uint32 GetFrameStrokeSize() const;
	HyColor GetPanelColor() const;
	HyColor GetFrameColor() const;
	HyColor GetTertiaryColor() const;

	IHyBody2d *GetPanelNode() const;
	HyUiPanelInit ClonePanelInit() const; // Uses currently set properties of this panel to create an equivalent HyPanelInit struct

	glm::vec2 GetBotLeftOffset();		// What offset is needed to get *this oriented to its bottom left

#ifdef HY_PLATFORM_GUI
	void GuiOverridePanelNodeData(HyType eNodeType, HyJsonObj itemDataObj, bool bUseGuiOverrideName, HyEntity2d *pParent);
#endif

private:
	void ConstructPrimitives(float fWidth, float fHeight);
	void DeleteData();
};

#endif /* HyPanel_h__ */
