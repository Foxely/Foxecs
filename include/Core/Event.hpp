#pragma once

#include "Types.hpp"
#include <any>
#include <unordered_map>


class Event
{
public:
	Event() = delete;

	explicit Event(EventId type)
		: mType(type)
	{}

	template<typename T>
	void SetParam(EventId id, T value)
	{
		m_Data[id] = value;
	}

	template<typename T>
	T GetParam(EventId id)
	{
		return std::any_cast<T>(m_Data[id]);
	}

	EventId GetType() const
	{
		return mType;
	}

private:
	EventId mType{};
	std::unordered_map<EventId, std::any> m_Data{};
};
