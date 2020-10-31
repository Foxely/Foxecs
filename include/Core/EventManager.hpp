#pragma once

#include "Event.hpp"
#include "Types.hpp"
#include <functional>
#include <list>
#include <unordered_map>


class EventManager
{
public:
	void AddListener(EventId eventId, std::function<void(Event&)> const& listener)
	{
		listeners[eventId].push_back(listener);
	}

	template <class A>
	void process_one_type(std::string& strEvent)
	{
		strEvent += typeid(A).name();
	}

	// template <typename... Args>
	// void Subscribe(EventId eventId, std::function<void(Event&)> const& listener)
	// {
	// 	std::string strEvent;
	// 	int _[] = {0, (process_one_type<Args>(strEvent), 0)...};
	// 	(void)_;
	// 	EventId event = fnv1a_32(strEvent.c_str(), strEvent.size()) + eventId;
	// 	std::cout << strEvent << std::endl;
	// 	std::cout << event << std::endl;
	// 	listeners[event].push_back(listener);
	// }

	void SendEvent(Event& event)
	{
		uint32_t type = event.GetType();

		for (auto const& listener : listeners[type])
		{
			listener(event);
		}
	}

	void SendEvent(EventId eventId)
	{
		Event event(eventId);

		for (auto const& listener : listeners[eventId])
		{
			listener(event);
		}
	}

private:
	std::unordered_map<EventId, std::list<std::function<void(Event&)>>> listeners;
};
