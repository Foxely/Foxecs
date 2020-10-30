#pragma once

#include <set>
#include <functional>
#include <type_traits>
#include "Types.hpp"


class World;

class System
{
public:
	World& m_oWorld;
	std::set<Entity> mEntities;

	explicit System(World& oWorld) : m_oWorld(oWorld)
	{
	}

	// .each([](flecs::entity e, Position& p, const Velocity& v) {    
    //         p.x += v.x;
    //         p.y += v.y;

    //         std::cout << "Moved " << e.name() << " to {" <<
    //             p.x << ", " << p.y << "}" << std::endl;
    //     });

	template<typename... Types>
	void each(typename std::common_type<std::function<void(Entity, Types&...)>>::type viewFunc);
};
