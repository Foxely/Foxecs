
#include "World.hpp"
#include "System.hpp"

template<typename... Types>
void System::each(typename std::common_type<std::function<void(Entity, Types&...)>>::type viewFunc)
{
    for (auto ent : mEntities)
    {
        viewFunc(ent, m_oWorld.GetComponent<Types...>(ent));
    }
}