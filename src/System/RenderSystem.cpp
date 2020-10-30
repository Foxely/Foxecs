#include "System/RenderSystem.hpp"

#include "Components/Transform.hpp"
#include "Core/World.hpp"
#include <cmath>

RenderSystem::RenderSystem(World& oWorld) : System(oWorld)
{
	// gCoordinator.AddEventListener(METHOD_LISTENER(Events::Window::RESIZED, RenderSystem::WindowSizeListener));
}


void RenderSystem::Update(float dt)
{
	// for (auto const& entity : mEntities)
	// {
    // 	std::cout << m_oWorld.GetComponent<Transform>(entity).position.x;
	// }
}
