/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** ComponentArray.hpp
*/

#pragma once

#include "common.hpp"
#include <array>
#include <any>
#include <cassert>
#include <unordered_map>
#include <exception>
#include "Utils/Option.hpp"
#include <memory>

namespace fox
{
    class IComponentArray
    {
    public:
        virtual ~IComponentArray() = default;

        /**
         * @brief Called when a entity is destroyed
         *
         * @param entity the entity
         */
        virtual void EntityDestroyed(EntityId entity) = 0;

        /**
         * @brief Clear the component array
         */
        virtual void clear() = 0;
    };

    class ComponentArray
    {
    public:
         /**
          * @brief Insert a new Component at the entity index
          * @tparam Args the Arguments types for constructor of the component
          * @param entity the index in the array
          * @param args the arguments for constructor of the component
          */
        template <typename T, typename... Args>
        inline void Add(EntityId entity, Args&&... args)
        {
            m_vComponents.emplace(
                std::piecewise_construct, std::forward_as_tuple(entity),
                std::forward_as_tuple(std::make_any<T>(std::forward<Args>(args)...)));
        }

        template <typename T>
        inline void Add(EntityId entity)
        {
            m_vComponents.emplace(
                std::piecewise_construct, std::forward_as_tuple(entity),
                std::forward_as_tuple(std::make_any<T>()));
        }

        /**
         * @brief Remove a Component at the entity index
         * @param entity the index
         */
        inline void Remove(EntityId entity)
        {
            auto it = m_vComponents.find(entity);
            if (it != m_vComponents.end()) {
                // delete it->second;
                m_vComponents.erase(it);
            }
        }

        /**
         * @brief Get the Component object at the entity index
         *
         * @param entity the index
         * @return fox::Option<T&> optionnal value
         */
        template<typename T>
        inline Option<T&> Get(EntityId entity)
        {
            // static_assert(std::is_base_of<Component, T>::value, "Invalid component type.");

            auto it = m_vComponents.find(entity);

            if (it != m_vComponents.end()) {
                try {
                    return fox::just(*std::any_cast<T>(&it->second));
                } catch (std::bad_any_cast) {
                }
            }
            return fox::null<T&>();
        }

        /**
         * @brief Get the Component object at the entity index
         *
         * @param entity the index
         * @return fox::Option<T&> optionnal value
         */
//        template<typename T>
//        inline Option<T&> Get(EntityId entity) const
//        {
//            // static_assert(std::is_base_of<Component, T>::value, "Invalid component type.");
//
//            auto it = m_vComponents.find(entity);
//
//            if (it != m_vComponents.end()) {
//                try {
//                    return fox::just(*std::any_cast<T>(&it->second));
//                } catch (std::bad_any_cast) {
//                }
//            }
//            return fox::nullopt;
//        }

        /**
         * @brief Called when a entity is destroyed
         *
         * @param entity the entity
         */
        inline void EntityDestroyed(EntityId entity)
        {
            Remove(entity);
        }

        /**
         * @brief Clear the component array
         */
        void clear()
        {
            if (m_vComponents.empty())
                return;
            m_vComponents.clear();
        }

    private:
        std::unordered_map<EntityId, std::any> m_vComponents;
    };
}
