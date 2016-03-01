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
						m_txtName(NULL, NULL),
						m_testSprite(1)
{ }

Player::~Player(void)
{
}

void Player::Initialize()
{
	m_primBox.Load();
	m_primBox.color.Set(0.0f, 0.0f, 1.0f, 1.0f);
	m_primBox.SetAsQuad(15.0f, 15.0f, false);
	m_primBox.SetDisplayOrder(100);

	
	//m_spineBoy.Load();
	//m_spineBoy.SetDisplayOrder(1.0f);

	//m_txtName.Load();
	//m_txtName.SetString(L"Testing");11
	//m_txtName.Pos().Offset(0.0f, 0.0f);
	
	//m_spineBoy.AddChild(m_txtName);
	//m_spineBoy.AddChild(m_primBox);

	//m_spineBoy.Pos().Animate(2.0f, 0.0f, 5.0f, HyEase::linear);
	//m_primBox.Pos().Animate(-1.0f, 0.0f, 5.0f, HyEase::linear);

	m_testSprite.Load();
	m_testSprite.SetTextureSource(8, 0, 0, 180, 162);


	//m_testSprite.SetUsingLocalCoordinates(true);
	m_testSprite.SetDisplayOrder(150);
}
