/**************************************************************************
 *	Player.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Player.h"


Player::Player(void) :	m_spineBoy(NULL, "SpineBoy"),
						m_primBox(),
						m_txtName(NULL, NULL)
{ }

Player::~Player(void)
{
}

void Player::Initialize()
{
	//m_primBox.Load();
	//m_primBox.Color().Set(1.0f, 0.0f, 0.0f, 1.0f);
	//m_primBox.SetAsQuad(15.0f, 15.0f, true);
	//m_primBox.SetDisplayOrder(1.2f);
	
	m_spineBoy.Load();
	m_spineBoy.SetDisplayOrder(1.0f);

	//m_txtName.Load();
	//m_txtName.SetString(L"Testing");
	//m_txtName.Pos().Offset(0.0f, -100.0f);
	
	//m_spineBoy.AddChild(m_txtName);
	//m_spineBoy.AddChild(m_primBox);

	//m_spineBoy.Pos().Animate(2.0f, 0.0f, 5.0f, HyEase::linear);
	//m_primBox.Pos().Animate(-1.0f, 0.0f, 5.0f, HyEase::linear);
}
