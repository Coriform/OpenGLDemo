#include "CommonLibs.h"
#include "Player.h"

namespace Roivas
{
	Player::Player() : Component(CT_Player)
	{

	}

	Player::Player(const Player& p) : Component(CT_Player)
	{

	}

	void Player::Initialize()
	{

	}

	Player* Player::Clone()
	{
		return new Player(*this);
	}
}