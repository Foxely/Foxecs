#include <chrono>
#include <random>
#include <iostream>

#include "Core/World.hpp"
#include "Components/Transform.hpp"
#include "System/RenderSystem.hpp"

static bool quit = false;


void QuitHandler(Event& event)
{
	quit = true;
}

int main()
{
    World world;

	world.AddEventListener(FUNCTION_LISTENER(Events::Window::QUIT, QuitHandler));

	// auto renderSystem = world.RegisterSystem<RenderSystem>();
	// {
	// 	Signature signature;
	// 	signature.set(world.GetComponentType<Transform>());
	// 	world.SetSystemSignature<RenderSystem>(signature);
	// }
    
    auto renderSystem = world.system<Transform>();
    
    Entity entity = world.CreateEntity();
    world.AddComponent<Transform>(entity, { .position = Vec2(1, 2) });

    Entity entitys = world.CreateEntity();
    world.AddComponent<Transform>(entitys, { .position = Vec2(5, 2) });
    
    renderSystem->each<Transform>([](Entity e, Transform& transform) {
        std::cout << transform.position.x << std::endl;
    });
    // renderSystem->Update(0);
    // std::cout << world.GetComponent<Transform>(entity).position.x;
	// physicsSystem->Init();

	// std::vector<Entity> entities(MAX_ENTITIES - 1);

	// for (auto& entity : entities)
	// {
	// 	entity = gCoordinator.CreateEntity();
	// 	// gCoordinator.AddComponent(entity, Player{});

	// 	// gCoordinator.AddComponent<Gravity>(
	// 	// 	entity,
	// 	// 	{ Vec3(0.0f, randGravity(generator), 0.0f) });

	// 	// gCoordinator.AddComponent(
	// 	// 	entity,
	// 	// 	Transform {
	// 	// 		.position = Vec3(randPosition(generator), randPosition(generator), randPosition(generator)),
	// 	// 		.rotation = Vec3(randRotation(generator), randRotation(generator), randRotation(generator)),
	// 	// 		.scale = Vec3(scale, scale, scale)
	// 	// 	});
	// }

	// while (!quit)
	// {
	// 	// physicsSystem->Update(dt);
	// }
	return 0;
}
