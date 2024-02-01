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

struct HyPanelInit
{
	// Whatever PanelType is chosen will affect UI layout positioning and metrics
	enum PanelType
	{
		PANELTYPE_Invalid = -1,
		PANELTYPE_BoundingVolume = 0,	// The panel isn't visibly seen, but can still have a width and height
		PANELTYPE_HyBody,				// The panel metrics are dictated by the IHyBody instance
		PANELTYPE_Primitive				// The panel is assembled in this class using primitives
	};
	PanelType					m_ePanelType;

	uint32						m_uiWidth;
	uint32						m_uiHeight;

	HyType						m_eBodyType;
	HyNodePath					m_HyBodyPath;

	uint32						m_uiFrameSize;
	HyColor						m_PanelColor;
	HyColor						m_FrameColor;
	HyColor						m_TertiaryColor;

	// Constructs a 'BoundingVolume' panel with 0 width/height
	HyPanelInit();

	// Constructs a 'BoundingVolume' panel
	HyPanelInit(uint32 uiWidth, uint32 uiHeight);

	// Constructs a 'HyBody' panel
	HyPanelInit(HyType eBodyType, const HyNodePath &nodePath);

	// Constructs a 'Primitive' panel. Colors of HyColor(0,0,0,0) will be set to a default color determined by the panel's usage
	HyPanelInit(uint32 uiWidth, uint32 uiHeight, uint32 uiFrameSize, HyColor panelColor = HyColor(0,0,0,0), HyColor frameColor = HyColor(0,0,0,0), HyColor tertiaryColor = HyColor(0,0,0,0));
};

class HyPanel : public HyEntity2d
{
	HyPanelInit::PanelType		m_ePanelType;

	// Only used when PANELTYPE_HyBody
	IHyBody2d *					m_pHyBody;

	// Only used when PANELTYPE_Primitive
	struct Primitive
	{
		uint32					m_uiFrameSize;
		HyPrimitive2d			m_Frame1;
		HyPrimitive2d			m_Frame2;	// When thicc enough, a frame can have two tones to it
		HyPrimitive2d			m_Panel;
		HyColor					m_PanelColor;
		HyColor					m_FrameColor;
		HyColor					m_TertiaryColor;
	};
	Primitive *					m_pPrimitive;

public:
	HyAnimVec2					size;

public:
	HyPanel(const HyPanelInit &initRef, HyEntity2d *pParent);
	virtual ~HyPanel();

	virtual float GetWidth(float fPercent = 1.0f) override;
	virtual float GetHeight(float fPercent = 1.0f) override;

	void Setup(const HyPanelInit &initRef);
	bool IsValid();

	bool IsBoundingVolume() const;
	bool IsHyBody() const;
	bool IsPrimitive() const;

	void ApplyPanelState(HyPanelState ePanelState);

	IHyBody2d *GetHyBody();

	glm::ivec2 GetSizeHint();
	uint32 GetFrameStrokeSize() const;
	glm::vec2 GetBotLeftOffset();

	HyColor GetPanelColor() const;
	void SetPanelColor(HyColor color);
	HyColor GetFrameColor() const;
	void SetFrameColor(HyColor color);

protected:
	virtual void OnUpdate() override;
	void ConstructPrimitives();
};

#endif /* HyPanel_h__ */
