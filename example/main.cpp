#include <chrono>
#include <random>
#include <iostream>
#include <string>

#include "Core/World.hpp"
#include "Components/Transform.hpp"

static bool bRun = true;

struct Message
{
    std::string msg;
};

int main()
{
    World world;

    world.system<Message>()->kind(Foxecs::System::OnAdd, [](World& w, Entity& e) {
        std::cout << e << ": A new Message Component !!" << std::endl;
    });

    world.system<Message>()->kind(Foxecs::System::OnRemove, [](World& w, Entity& e) {
        std::cout << e << ": A remove Message Component !!" << std::endl;
    });

    world.system<Transform>()->kind(Foxecs::System::OnAdd, [](World& w, Entity& e) {
        std::cout << e << ": A new Transform Component !!" << std::endl;
    });
    
    Entity entity = world.CreateEntity();
    world.AddComponent<Transform>(entity, { .position = Vec2(1, 2) });

    Entity entitys = world.CreateEntity();
    world.AddComponent<Transform>(entitys, { .position = Vec2(5, 2) });

    Entity ent = world.CreateEntity();
    world.AddComponent<Transform>(ent, { .position = Vec2(7, 2) });
    world.AddComponent<Message>(ent, { "Hello !" });
    world.RemoveComponent<Message>(ent);
    
    world.Update();
    return 0;
}
