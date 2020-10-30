#pragma once

#include <iostream>
#include <memory>
#include <set>
#include <functional>
#include <type_traits>
#include <string>

#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "EventManager.hpp"
#include "Types.hpp"

class World;

template<typename Func>
class Invoker
{
public:
	World& m_oWorld;
	std::set<Entity> mEntities;

	explicit Invoker(World& oWorld) : m_oWorld(oWorld) {	}

    template<typename... Types>
	void each(typename std::common_type<std::function<void(Entity, Types&...)>>::type viewFunc);

private:
    Func m_func;
    //std::function<void(Entity, Types&...)> updateFunc;
};

class System
{
public:
	World& m_oWorld;
	std::set<Entity> mEntities;

	explicit System(World& oWorld) : m_oWorld(oWorld) {	}

    template<typename... Types>
	void each(typename std::common_type<std::function<void(Entity, Types&...)>>::type viewFunc);

private:
    //std::function<void(Entity, Types&...)> updateFunc;
};


class SystemManager
{
public:
	std::shared_ptr<System> RegisterSystem(World* pWorld, std::string& name)
	{
        if (name.empty())
            name = "System_" + std::to_string(mSystems.size());

		assert(mSystems.find(name.c_str()) == mSystems.end() && "Registering system more than once.");

		auto system = std::make_shared<System>(*pWorld);
		mSystems.insert({name.c_str(), system});
		return system;
	}

	void SetSignature(Signature signature, std::string& name)
	{
		assert(mSystems.find(name.c_str()) != mSystems.end() && "System used before registered.");

		mSignatures.insert({name.c_str(), signature});
	}

	template<typename T>
	void GetSignature()
	{
		const char* typeName = typeid(T).name();
		if (mSystems.find(typeName) != mSystems.end())
			mSignatures[typeName];
		else
			std::cerr << "The System '" << typeName << "' have no signature.";
	}

	void EntityDestroyed(Entity entity)
	{
		for (auto const& pair : mSystems)
		{
			auto const& system = pair.second;

			system->mEntities.erase(entity);
		}
	}

	void EntitySignatureChanged(Entity entity, Signature entitySignature)
	{
		for (auto const& pair : mSystems)
		{
			auto const& type = pair.first;
			auto const& system = pair.second;
			auto const& systemSignature = mSignatures[type];

			if ((entitySignature & systemSignature) == systemSignature)
				system->mEntities.insert(entity);
			else
				system->mEntities.erase(entity);
		}
	}

private:
	std::unordered_map<const char*, Signature> mSignatures{};
	std::unordered_map<const char*, std::shared_ptr<System>> mSystems{};
};

class World
{
public:
	explicit World()
	{
		mComponentManager = std::make_unique<ComponentManager>();
		mEntityManager = std::make_unique<EntityManager>();
		mEventManager = std::make_unique<EventManager>();
		mSystemManager = std::make_unique<SystemManager>();
	}

	// Entity methods
	Entity CreateEntity()
	{
		return mEntityManager->CreateEntity();
	}

	void DestroyEntity(Entity entity)
	{
		mEntityManager->DestroyEntity(entity);
		mComponentManager->EntityDestroyed(entity);
		mSystemManager->EntityDestroyed(entity);
	}


	// Component methods
	template<typename T>
	void RegisterComponent()
	{
		mComponentManager->RegisterComponent<T>();
	}

	template<typename T>
	void AddComponent(Entity entity, T component)
	{
		if (!mComponentManager->ComponentIsRegistered<T>())
		{
			RegisterComponent<T>();
		}

		mComponentManager->AddComponent<T>(entity, component);

		auto signature = mEntityManager->GetSignature(entity);
		signature.set(mComponentManager->GetComponentType<T>(), true);
		mEntityManager->SetSignature(entity, signature);

		mSystemManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T>
	void RemoveComponent(Entity entity)
	{
		mComponentManager->RemoveComponent<T>(entity);

		auto signature = mEntityManager->GetSignature(entity);
		signature.set(mComponentManager->GetComponentType<T>(), false);
		mEntityManager->SetSignature(entity, signature);
		mSystemManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T>
	T& GetComponent(Entity entity)
	{
		return mComponentManager->GetComponent<T>(entity);
	}

	template<typename T>
	ComponentType GetComponentType()
	{
		if (!mComponentManager->ComponentIsRegistered<T>())
			RegisterComponent<T>();

		return mComponentManager->GetComponentType<T>();
	}


	// System methods
	std::shared_ptr<System> RegisterSystem(std::string name)
	{
		return mSystemManager->RegisterSystem(this, name);
	}


	template<typename... Comps>
	std::shared_ptr<System> system(std::string name = "")
	{
		auto system = mSystemManager->RegisterSystem(this, name);

		Signature signature;
		signature.set(GetComponentType<Comps...>());
		SetSystemSignature(signature, name);
		return system;
	}

	void SetSystemSignature(Signature signature, std::string& name)
	{
		mSystemManager->SetSignature(signature, name);
	}


	// Event methods
	void AddEventListener(EventId eventId, std::function<void(Event&)> const& listener)
	{
		mEventManager->AddListener(eventId, listener);
	}

	void SendEvent(Event& event)
	{
		mEventManager->SendEvent(event);
	}

	void SendEvent(EventId eventId)
	{
		mEventManager->SendEvent(eventId);
	}

    void Update(float dt = 0.0f)
	{

	}

private:
	std::unique_ptr<ComponentManager> mComponentManager;
	std::unique_ptr<EntityManager> mEntityManager;
	std::unique_ptr<EventManager> mEventManager;
	std::unique_ptr<SystemManager> mSystemManager;
};


// -----------------------------------
// --       System Functions        --
// -----------------------------------
template<typename... Types>
inline void System::each(typename std::common_type<std::function<void(Entity, Types&...)>>::type viewFunc)
{
    for (auto ent : mEntities)
    {
        viewFunc(ent, m_oWorld.GetComponent<Types...>(ent));
    }
}