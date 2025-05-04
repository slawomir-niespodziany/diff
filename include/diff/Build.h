#pragma once

/**
 * @file Build.h
 * @author Slawomir Niespodziany (sniespod@gmail.com, slawomir.niespodziany@pw.edu.pl)
 * @brief Defines Build class used to instantiate components as defined by Topology object.
 * @version 0.1
 * @date 2025-03-03
 * @copyright Copyright (c) 2025 Slawomir Niespodziany
 */

#include <diff/Factory.h>
#include <diff/FactoryRegistry.h>
#include <diff/Topology.h>
#include <iostream>
#include <stack>

namespace diff {

/**
 * @brief Build object instantiates and owns a set of components as defined by the injected Topology object. Its constructor instantiates components
 * and performs dependency injection. The resulting dependencies are available for external use.
 */
class Build final {
public:
    /**
     * @brief Construct a Build object. Instantiate components as defined by the Topology object.
     * @exception FactoryNotFound If factory of a requested component type is not registered within FactoryRegistry.
     *
     * @param topology Topology object defining components to be instantiated.
     */
    Build(Topology& topology) {
        for (TopologyEntry& topologyEntry : topology) {
            Factory<>& factory = FactoryRegistry::getInstance().get(topologyEntry.type);
            std::unique_ptr<Component<>> pComponent = factory.build(topologyEntry.id,              //
                                                                    topologyEntry.dependencyIds,   //
                                                                    topologyEntry.config,          //
                                                                    dependencyRegistry_);
            componentStack_.stack.emplace(std::move(pComponent));
        }
    }
    ~Build() = default;

    /**
     * @brief Return information (component type name and instance id) about all available dependencies.
     *
     * @return Vector of pairs of std::string references. One pair for each dependency. Each pair consists of a reference to dependency type name and
     * dependency instance id.
     */
    std::vector<std::pair<std::reference_wrapper<const std::string>, std::reference_wrapper<const std::string>>> all() const noexcept {
        return dependencyRegistry_.all();
    }

    /**
     * @brief Indicate whether a dependency of the given type and id is available.
     *
     * @tparam T Dependency type. Dependency type is its abstract interface, not the underlying implementing type.
     * @param id Dependency id.
     * @return True if dependency is available, false otherwise.
     */
    template <typename T>
    bool has(const std::string& id) const noexcept {
        return dependencyRegistry_.has<T>(id);
    }

    /**
     * @brief Return references to all available dependencies of the given type.
     *
     * @tparam T Dependencies type. Dependency type is its abstract interface, not the underlying implementing type.
     * @return Vector of references.
     */
    template <typename T>
    std::vector<std::reference_wrapper<T>> get() const noexcept {
        return dependencyRegistry_.get<T>();
    }

    /**
     * @brief Return reference to the dependency of a given type and id.
     *
     * @exception DependencyRegisterNotFound If no dependencies of the requested type are available.
     * @exception DependencyNotFound If no dependency of the requested type and id is available.
     *
     * @tparam T Dependency type. Dependency type is its abstract interface, not the underlying implementing type.
     * @param id Dependency id.
     * @return Dependency reference.
     */
    template <typename T>
    T& get(const std::string& id) const {
        return dependencyRegistry_.get<T>(id);
    }

private:
    struct ComponentStack final {
        ~ComponentStack() {
            while (!stack.empty()) {
                stack.pop();
            }
        }
        std::stack<std::unique_ptr<Component<>>> stack;
    };

    DependencyRegistry dependencyRegistry_;
    ComponentStack componentStack_;
};

}   // namespace diff
