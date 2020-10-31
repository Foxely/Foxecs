#pragma once

#include "ComponentArray.hpp"
#include "Types.hpp"
#include <any>
#include <memory>
#include <unordered_map>


class ComponentManager
{
public:
	template<typename T>
	inline void RegisterComponent()
	{
		const char* typeName = typeid(T).name();

		assert(!ComponentIsRegistered<T>() && "Registering component type more than once.");

		m_ComponentTypes.insert({typeName, mNextComponentType});
		mComponentArrays.insert({typeName, std::make_shared<ComponentArray<T>>()});

		++mNextComponentType;
	}

	template<typename T>
	inline ComponentType GetComponentType()
	{
		const char* typeName = typeid(T).name();

		assert(ComponentIsRegistered<T>() && "Component not registered before use.");

		return m_ComponentTypes[typeName];
	}

	template<typename T>
	inline bool ComponentIsRegistered()
	{
		const char* typeName = typeid(T).name();
		return m_ComponentTypes.find(typeName) != m_ComponentTypes.end();
	}

	template<typename T>
	inline void AddComponent(Entity entity, T component)
	{
		GetComponentArray<T>()->InsertData(entity, component);
	}

	template<typename T>
	inline void RemoveComponent(Entity entity)
	{
		GetComponentArray<T>()->RemoveData(entity);
	}

	template<typename T>
	inline T& GetComponent(Entity entity)
	{
		return GetComponentArray<T>()->GetData(entity);
	}

	inline void EntityDestroyed(Entity entity)
	{
		for (auto const& pair : mComponentArrays)
		{
			auto const& component = pair.second;

			component->EntityDestroyed(entity);
		}
	}

private:
	std::unordered_map<const char*, ComponentType> m_ComponentTypes{};
	std::unordered_map<const char*, std::shared_ptr<IComponentArray>> mComponentArrays{};
	ComponentType mNextComponentType{};


	template<typename T>
	inline std::shared_ptr<ComponentArray<T>> GetComponentArray()
	{
		const char* typeName = typeid(T).name();
		if (!ComponentIsRegistered<T>())
		{
			std::cerr << "Component '" << typeName << "' not registered before use.";
			assert(false);
		}

		return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[typeName]);
	}
};
