#pragma once

#include <iostream>
#include <memory>
#include <set>
#include <functional>
#include <type_traits>
#include <string>
#include <vector>
#include <any>

#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "EventManager.hpp"
#include "Types.hpp"

class World;

using SystemFn = std::function<void(World&, Entity&)>;

// @brief The System is used to perform the logic
class System
{
public:
	World& m_oWorld;
	std::string name;											// The name of the system, used to generate unique Id for the different Event Phases like OnAdd or OnRemove
	std::set<Entity> m_Entities;								// List of all entities who match with the key signature of the System

	explicit System(World& oWorld) : m_oWorld(oWorld) {	}

	// @brief This function store a function for this System that will be called on the OnUpdate Phase
	void each(SystemFn updateFunc);

	// @brief This function allows to add a listener to an event
	System& kind(EventId id, SystemFn func);

private:
	void UpdatePhase(Event& event)
	{
		for(auto& ent : m_Entities)
			kinds[event.GetType()](m_oWorld, (Entity &) ent);
	}

	void Phase(Event& event)
	{
		Entity ent = event.GetParam<Entity>(Foxecs::System::ENTITY);
		kinds[event.GetType()](m_oWorld, ent);
	}

	SystemFn m_UpdateFunc;
	std::unordered_map<EventId, SystemFn> kinds;
};


// @brief This is the Manager of all System class
class SystemManager
{
public:
	// @brief This function allows to add a new System
	std::shared_ptr<System> RegisterSystem(World* pWorld, std::string& name)
	{
        if (name.empty())
            name = "System_" + std::to_string(m_vSystems.size());

		assert(m_vSystems.find(name.c_str()) == m_vSystems.end() && "Registering system more than once.");

		auto system = std::make_shared<System>(*pWorld);
		system->name = name;
		m_vSystems.insert({name, system});
		return system;
	}

	// @brief This function allows to add a Signature to the System who match with the name given in parameter
	void SetSignature(Signature signature, std::string& name)
	{
		assert(m_vSystems.find(name.c_str()) != m_vSystems.end() && "System used before registered.");

		m_vSignatures.insert({name.c_str(), signature});
	}

	// @brief This function allows to get the Signature who match with the name given in parameter
	void GetSignature(std::string& name)
	{
		if (m_vSystems.find(name) != m_vSystems.end())
			m_vSignatures[name];
		else
			std::cerr << "The System '" << name << "' have no signature.";
	}

	// @brief This function allows to delete a entity in the system list
	void EntityDestroyed(Entity entity)
	{
		for (auto const& pair : m_vSystems)
		{
			auto const& system = pair.second;

			system->m_Entities.erase(entity);
		}
	}

	// @brief This function remove/add the entity, given in parameter, in the system list and it trigger a OnAdd/OnRemove event
	void EntitySignatureChanged(Entity entity, Signature entitySignature);

	void ForEachSystem()
	{
		// for(auto& system : m_vSystems)
		// 	for (auto ent : system.second->m_Entities)
		// 		system.second->update(ent);
	}

private:
	std::unordered_map<std::string, Signature> m_vSignatures{};
	std::unordered_map<std::string, std::shared_ptr<System>> m_vSystems{};
};

class World
{
public:
	explicit World()
	{
		m_ComponentManager = std::make_unique<ComponentManager>();
		m_EntityManager = std::make_unique<EntityManager>();
		m_EventManager = std::make_unique<EventManager>();
		m_SystemManager = std::make_unique<SystemManager>();
	}

	// @brief This function allows to create a new entity
	Entity CreateEntity()
	{
		return m_EntityManager->CreateEntity();
	}

	// @brief This function allows to destroy the entity, given in parameter
	void DestroyEntity(Entity entity)
	{
		m_EntityManager->DestroyEntity(entity);
		m_ComponentManager->EntityDestroyed(entity);
		m_SystemManager->EntityDestroyed(entity);
	}


	// @brief This function allows to register an new Component
	template<typename T>
	void RegisterComponent()
	{
		m_ComponentManager->RegisterComponent<T>();
	}

	// @brief This function allows to add an Component in the entity
	template<typename T>
	void AddComponent(Entity entity, T component)
	{
		if (!m_ComponentManager->ComponentIsRegistered<T>())
			RegisterComponent<T>();

		m_ComponentManager->AddComponent<T>(entity, component);

		auto signature = m_EntityManager->GetSignature(entity);
		signature.set(m_ComponentManager->GetComponentType<T>(), true);
		m_EntityManager->SetSignature(entity, signature);

		m_SystemManager->EntitySignatureChanged(entity, signature);
	}

	// @brief This function allows to remove an Component in the entity
	template<typename T>
	void RemoveComponent(Entity entity)
	{
		m_ComponentManager->RemoveComponent<T>(entity);

		auto signature = m_EntityManager->GetSignature(entity);
		signature.set(m_ComponentManager->GetComponentType<T>(), false);
		m_EntityManager->SetSignature(entity, signature);
		m_SystemManager->EntitySignatureChanged(entity, signature);
	}

	// @brief This function allows to get the Component in the entity
	template<typename T>
	T& GetComponent(Entity entity)
	{
		return m_ComponentManager->GetComponent<T>(entity);
	}

	// @brief This function allows to get the type of a Component
	template<typename T>
	ComponentType GetComponentType()
	{
		if (!m_ComponentManager->ComponentIsRegistered<T>())
			RegisterComponent<T>();

		return m_ComponentManager->GetComponentType<T>();
	}


	// System methods
	// std::shared_ptr<System> RegisterSystem(std::string name = "")
	// {
	// 	return m_SystemManager->RegisterSystem(this, name);
	// }

	template <class A>
	void process_one_type(Signature& signature)
	{
		signature.set(GetComponentType<A>());
	}
	
	// @brief This function allows to register and create a new System
	template<typename... Comps>
	std::shared_ptr<System> system(std::string name = "")
	{
		auto system = m_SystemManager->RegisterSystem(this, name);

		Signature signature;
		int _[] = {0, (process_one_type<Comps>(signature), 0)...};
		(void)_;
		m_SystemManager->SetSignature(signature, name);
		return system;
	}

	// Event methods
	// @brief This function allows to add a new Listener
	void AddEventListener(EventId eventId, std::function<void(Event&)> const& listener)
	{
		m_EventManager->AddListener(eventId, listener);
	}

	// @brief This function allows to trigger an event
	void SendEvent(Event& event)
	{
		m_EventManager->SendEvent(event);
	}

	// @brief This function allows to trigger an event
	void SendEvent(EventId eventId)
	{
		m_EventManager->SendEvent(eventId);
	}

	// @brief This function allows to trigger an event
    void Update(float dt = 0.0f)
	{
		(void) dt;
		m_EventManager->SendEvent(Foxecs::System::OnUpdate);
		// m_SystemManager->ForEachSystem();
	}

private:
	std::unique_ptr<ComponentManager> m_ComponentManager;
	std::unique_ptr<EntityManager> m_EntityManager;
	std::unique_ptr<EventManager> m_EventManager;
	std::unique_ptr<SystemManager> m_SystemManager;
};

inline void SystemManager::EntitySignatureChanged(Entity entity, Signature entitySignature)
{
	for (auto const& pair : m_vSystems)
	{
		auto const& type = pair.first;
		auto const& system = pair.second;
		auto const& systemSignature = m_vSignatures[type];

		if ((entitySignature & systemSignature) == systemSignature)
		{
			bool bIsExist = system->m_Entities.find(entity) != system->m_Entities.end();
			system->m_Entities.insert(entity);

			// Si il n'existe pas, Envoyez l'event OnAdd
			if (!bIsExist)
			{
				Event event(fnv1a_32(system->name.c_str(), system->name.size()) + Foxecs::System::OnAdd);
				event.SetParam<Entity>(Foxecs::System::ENTITY, entity);
				system->m_oWorld.SendEvent(event);
			}
		}
		else
		{
			bool bIsExist = system->m_Entities.find(entity) != system->m_Entities.end();
			if (bIsExist)
			{
				Event event(fnv1a_32(system->name.c_str(), system->name.size()) + Foxecs::System::OnRemove);
				event.SetParam<Entity>(Foxecs::System::ENTITY, entity);
				system->m_oWorld.SendEvent(event);
			}
			system->m_Entities.erase(entity);
		}
	}
}


// -----------------------------------
// --       System Functions        --
// -----------------------------------

inline void System::each(SystemFn updateFunc)
{
	// m_UpdateFunc = updateFunc;
	// m_oWorld.AddEventListener(METHOD_LISTENER(Foxecs::System::OnUpdate, System::UpdatePhase));
	kind(Foxecs::System::OnUpdate, updateFunc);
}

inline System& System::kind(EventId id, SystemFn func)
{
	EventId event = id;
	if (id == Foxecs::System::OnAdd || id == Foxecs::System::OnRemove)
	{
		event = fnv1a_32(name.c_str(), name.size()) + id;
		m_oWorld.AddEventListener(METHOD_LISTENER(event, System::Phase));
	}
	else
		m_oWorld.AddEventListener(METHOD_LISTENER(event, System::UpdatePhase));
	kinds.insert({ event, func });
	return *this;
}