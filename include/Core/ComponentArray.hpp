#pragma once

#include "Types.hpp"
#include <array>
#include <cassert>
#include <unordered_map>


class IComponentArray
{
public:
	virtual ~IComponentArray() = default;
	virtual void EntityDestroyed(Entity entity) = 0;
};


template<typename T>
class ComponentArray : public IComponentArray
{
public:
	inline void InsertData(Entity entity, T component)
	{
		if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end())
		{
			std::cerr << "Component added to same entity more than once." << std::endl;
			return;
		}

		// Put new entry at end
		size_t newIndex = mSize;
		mEntityToIndexMap[entity] = newIndex;
		mIndexToEntityMap[newIndex] = entity;
		mComponentArray[newIndex] = component;
		++mSize;
	}

	inline void RemoveData(Entity entity)
	{
		if (mEntityToIndexMap.find(entity) == mEntityToIndexMap.end())
		{
			std::cerr << "'" << typeid(T).name() << "': Removing non-existent component." << std::endl;
			return;
		}

		// Copy element at end into deleted element's place to maintain density
		size_t indexOfRemovedEntity = mEntityToIndexMap[entity];
		size_t indexOfLastElement = mSize - 1;
		mComponentArray[indexOfRemovedEntity] = mComponentArray[indexOfLastElement];

		// Update map to point to moved spot
		Entity entityOfLastElement = mIndexToEntityMap[indexOfLastElement];
		mEntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
		mIndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

		mEntityToIndexMap.erase(entity);
		mIndexToEntityMap.erase(indexOfLastElement);

		--mSize;
	}

	inline T& GetData(Entity entity)
	{
		if(mEntityToIndexMap.find(entity) == mEntityToIndexMap.end())
		{
			std::cerr << "'" << typeid(T).name() << "': Retrieving non-existent component." << std::endl;
			assert(false);
		}

		return mComponentArray[mEntityToIndexMap[entity]];
	}

	inline void EntityDestroyed(Entity entity) override
	{
		if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end())
		{
			RemoveData(entity);
		}
	}

private:
	std::array<T, MAX_ENTITIES> mComponentArray;
	std::unordered_map<Entity, size_t> mEntityToIndexMap;
	std::unordered_map<size_t, Entity> mIndexToEntityMap;
	size_t mSize{};
};
