#include <chrono>
#include <random>
#include <iostream>
#include <string>

#include "Core/World.hpp"
#include "Components/Transform.hpp"

struct Message
{
    Message(std::string str) : msg(str) {}
    std::string msg;
};

int main()
{
    fox::World world;

    world.RegisterComponent<Message>();
    world.RegisterComponent<Transform>();

    world.system<Message>()
        .kind(fox::ecs::OnAdd)
        .each([](fox::Entity& e, Option<Message&> msg) {
            std::cout << e.get_id() << ": A new Message Component !!" << std::endl;
        });

    world.system<Message>()
        .kind(fox::ecs::OnRemove)
        .each([](fox::Entity& e, Option<Message&> msg) {
            std::cout << e.get_id() << ": A remove Message Component !!" << std::endl;
        });

    world.system<Transform>()
        .kind(fox::ecs::OnAdd)
        .each([](fox::Entity& e, Option<Transform&> transform) {
            std::cout << e.get_id() << ": A new Transform Component !!" << std::endl;
        });

    fox::Entity entity = world.new_entity();
    entity.add<Transform>(Vec2(1, 2));

    fox::Entity entitys = world.new_entity();
    world.add_component<Transform>(entitys, Vec2(5, 2));

    fox::Entity ent = world.new_entity();
    ent.add<Transform>(Vec2(7, 2));
    ent.add<Message>("Hello !");
    ent.remove<Message>();

    return 0;
}
