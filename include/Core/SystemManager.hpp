#pragma once

#include <iostream>
#include <cassert>
#include <memory>
#include <unordered_map>

#include "System.hpp"
#include "Types.hpp"
#include "World.hpp"

class World;

class SystemManager
{
public:
	template<typename T>
	std::shared_ptr<T> RegisterSystem(World* pWorld)
	{
		const char* typeName = typeid(T).name();

		assert(mSystems.find(typeName) == mSystems.end() && "Registering system more than once.");

		auto system = std::make_shared<T>(*pWorld);
		mSystems.insert({typeName, system});
		return system;
	}

	template<typename T>
	void SetSignature(Signature signature)
	{
		const char* typeName = typeid(T).name();

		assert(mSystems.find(typeName) != mSystems.end() && "System used before registered.");

		mSignatures.insert({typeName, signature});
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
			{
				system->mEntities.insert(entity);
			}
			else
			{
				system->mEntities.erase(entity);
			}
		}
	}

private:
	std::unordered_map<const char*, Signature> mSignatures{};
	std::unordered_map<const char*, std::shared_ptr<System>> mSystems{};
};