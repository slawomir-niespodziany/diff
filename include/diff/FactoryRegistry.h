#pragma once

/**
 * @file FactoryRegistry.h
 * @author Slawomir Niespodziany (sniespod@gmail.com, slawomir.niespodziany@pw.edu.pl)
 * @brief Single point of access to all the factories available in the binary.
 * @brief Defines FactoryRegistry singleton class used to aggregate Factory<> objects.
 * @version 0.1
 * @date 2024-11-20
 *
 * @copyright Copyright (c) 2024
 */

#include <diff/Exception.h>
#include <diff/Factory.h>
#include <algorithm>
#include <map>
#include <set>
#include <stdexcept>

using namespace std::string_literals;

namespace diff {

/**
 * @brief A singleton class. Aggregation point of all Factory<> class objects available within the binary.
 */
class FactoryRegistry final {
public:
    ~FactoryRegistry() = default;

    /**
     * @brief Return singleton instance.
     *
     * @return Singleton instance reference.
     */
    static FactoryRegistry &getInstance() {
        static FactoryRegistry instance;
        return instance;
    }

    /**
     * @brief Return component type names, for all component factory objects available.
     *
     * @return Vector of type names.
     */
    std::vector<std::reference_wrapper<const std::string>> all() const noexcept {
        std::vector<std::reference_wrapper<const std::string>> result;

        result.reserve(factories_.size());
        std::transform(factories_.cbegin(), factories_.cend(), std::back_inserter(result),
                       [](const Factory<> &factory) { return std::cref(factory.type()); });

        return result;
    }

    /**
     * @brief Indicate whether a factory of the given component type name is registered.
     *
     * @param type Component type name.
     * @return True if factory is registered, false otherwise.
     */
    bool has(const std::string &type) const noexcept { return (0u != factories_.count(type)); }

    /**
     * @brief Return reference to a factory for the given component type name.
     * @exception FactoryNotFound If no factory for the given component type name is registered.

     *
     * @param type Component type name.
     * @return Factory reference.
     */
    Factory<> &get(const std::string &type) const {
        const auto it = factories_.find(type);
        if (factories_.cend() == it) {
            throw FactoryNotFound(type);
        }
        return it->get();
    }

private:
    template <typename T>
    friend class FactoryRegisterer;

    struct FactoryComparator {
        using is_transparent = void;

        bool operator()(const std::reference_wrapper<Factory<>> &first, const std::reference_wrapper<Factory<>> &second) const {
            return first.get().type() < second.get().type();
        }
        bool operator()(const std::reference_wrapper<Factory<>> &factory, const std::string &type) const { return factory.get().type() < type; }
        bool operator()(const std::string &type, const std::reference_wrapper<Factory<>> &factory) const { return type < factory.get().type(); }
    };

    FactoryRegistry() = default;

    bool add(Factory<> &factory) noexcept { return factories_.emplace(factory).second; }

    void remove(const Factory<> &factory) noexcept {
        const auto it = factories_.find(factory.type());
        if (factories_.cend() != it) {
            factories_.erase(it);
        }
    }

    std::set<std::reference_wrapper<Factory<>>, FactoryComparator> factories_;
};

}   // namespace diff
