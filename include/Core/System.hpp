#pragma once

#include <set>
// #include <vector>
#include "Types.hpp"


class World;

// template<typename... Comps>
class System
{
public:
	World& m_oWorld;
	std::set<Entity> mEntities;

	explicit System(World& oWorld) : m_oWorld(oWorld)
	{
	}


	// explicit System(World* pWorld)
	// {
	// 	Signature signature;
	// 	std::vector<std::type_info> vec = {typeid(Comps) ...};
	// 	for (unsigned i = 0; i < vec.size(); ++i) {
	// 		signature.set(world->GetComponentType<vec[i]>());
	// 	}
	// 	world->SetSystemSignature<RenderSystem>(signature);
	// }
};
