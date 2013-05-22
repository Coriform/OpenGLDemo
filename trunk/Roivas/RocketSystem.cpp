#include "CommonLibs.h"
#include "RocketSystem.h"

namespace Roivas
{
	RocketSystem::RocketSystem()
	{
		start_time = (float)SDL_GetTicks();
	}

	float RocketSystem::GetElapsedTime()
	{
		return (SDL_GetTicks() - start_time);
	}
}