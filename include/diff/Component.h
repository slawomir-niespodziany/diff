#pragma once

/**
 * @file Component.h
 * @author Slawomir Niespodziany (sniespod@gmail.com, slawomir.niespodziany@pw.edu.pl)
 * @brief Generic component base class implementing all the common functionalities required by the framework or the user.
 * @version 0.1
 * @date 2024-11-15
 * @copyright Copyright (c) 2024 Slawomir Niespodziany
 */

#include <diff/Config.h>
#include <diff/Demangler.h>
#include <diff/DependencyRegistry.h>
#include <diff/Exception.h>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <typeindex>
#include <vector>

namespace diff {

/**
 * @brief Generic Component<...> template. The intended use of this template is via its specialized templates only - this generic template shall not
 * be used.
 *
 * @tparam T User defined component class (CRTP pattern).
 * @tparam Us User selected customization.
 */
template <typename T = void, typename... Us>
class Component {
public:
    // static_assert(false, "Unknown Component<...> template parameter.");
};

/**
 * @brief Abstract base class, a common interface for instantiations of Component<T != void, ...>.
 */
template <>
class Component<void> {
public:
    virtual ~Component() = default;

    /**
     * @brief Return the underlying component type name.
     *
     * @return Type name reference.
     */
    const std::string& type() const { return type_; }

    /**
     * @brief Return component identifier.
     *
     * @return Return identifier reference.
     */
    const std::string& id() const { return id_; }

protected:
    Component(const std::string& type, std::string&& id, Config&& config) : type_{type}, id_{std::move(id)}, config_{std::move(config)} {}

    /**
     * @brief Return config parameter of the given type and key.
     * @exception ConfigEntryNotFound If no config entry exists for the given key.
     * @exception ConfigEntryCastError If parameter value can not be casted to the given type (dependent on the actual entry type and value, @see
     * ConfigEntryCastError).
     *
     * @tparam T Config parameter type.
     * @param key Config parameter key.
     * @return Parameter reference.
     */
    template <typename T>
    const T& config(const std::string& key) const {
        const auto it = config_.find(key);
        if (config_.cend() == it) {
            throw ConfigEntryNotFound(type_, id_, key);
        }

        return (*it)->value<T>();
    }

private:
    const std::string& type_;
    const std::string id_;

    const Config config_;
};

template <typename T>
class Factory;

/**
 * @brief Specialization allowing initialization of component instance identifier and config.
 *
 * @tparam T User component type (CRTP pattern).
 */
template <typename T>
class Component<T> : public Component<> {
public:
    virtual ~Component() = default;

protected:
    friend class Factory<T>;

    Component() : Component<>(diff::Demangler::of<T>(), std::move(initializer_.first), std::move(initializer_.second)) {}

    /**
     * @brief For the framework use only. Register the component (as dependency/dependencies declared with as<...>) and side dependencies (as declared
     * by side<...>) in the given DependencyRegistry.
     *
     * @param dependencyRegistry Registry to register the component in.
     */
    void registerAs(DependencyRegistry& dependencyRegistry) {}

    /**
     *  @brief For the framework use only.
     */
    std::set<std::string> sideDependencyIdentifiers_;

private:
    /**
     * @brief For the framework use only. Temporal storage of identifier and config for a newly created component.
     */
    static std::pair<std::string /* id */, Config> initializer_;
};

template <typename T>
std::pair<std::string /* id */, Config> Component<T>::initializer_;

/**
 * @brief Used for component customization. Putting as<T> (where T is an abstract interface) as a template argument of the Component<...> template
 * results in:
 *  - User defined component derives from the type T and shall implement the given interface,
 *  - Framework recognizes that inheritance and for each instantiated component will register it as a dependency of type T under the id of the
 * component instance.
 *
 * @tparam T Dependency type.
 */
template <typename T>
struct as {
    // TODO assert if not abstract
};

/**
 * @brief Specialization resolving user customization applied with as<...> and registering the component as the given dependency.
 *
 * @tparam T User component type (CRTP pattern).
 * @tparam U Dependency type for the component to be registered as.
 * @tparam Vs Other user applied customizations.
 */
template <typename T, typename U, typename... Vs>
class Component<T, as<U>, Vs...> : public U, public Component<T, Vs...> {
public:
    virtual ~Component() = default;

protected:
    friend class Factory<T>;

    Component() = default;

    /**
     * @brief @see Component<T>::registerAs
     */
    void registerAs(DependencyRegistry& dependencyRegistry) {
        if (!std::is_base_of<Component<T, Vs...>, as<U>>::value) {   // avoid registering multiple times as the same dependency
            dependencyRegistry.add<U>(Component<T>::id(), static_cast<U&>(*this));
        }
        Component<T, Vs...>::registerAs(dependencyRegistry);
    }
};

/**
 * @brief Used for component customization. Putting side<T> (where T is an abstract interface) as a template argument of the Component<...> template
 * results in:
 *  - User defined component can expose side-dependencies of type T. It shall implement a protected member function of the following signature:
 *      void side(SideDependencies<T> &sideDependencies);
 *    which populates sideDependencies object with references to side-dependency objects, with assigned side-identifiers for those references. Any
 * number of side-dependencies is allowed, including zero and it is instance-specific (different instances may have differnt number of
 * side-dependencies).
 * - User defined component owns the instances of side-dependencies. Their lifetime shall match the lifetime of the component instance itsself.
 *
 * @tparam T Side-dependency type.
 */
template <typename T>
struct side {};

/**
 * @brief Collection of named references to side dependencies to be registered in DependencyRegistry. Each side dependency shall have a unique
 * side-id (unique across the whole component, not only for the given side-dependency type). The resulting side-dependency identifier in the
 * DependencyRegistry is the component identifier concatenated with and underscore character '_' and side-identifier.
 *
 * @tparam T Dependencies type.
 */
template <typename T>
using SideDependencies = std::map<std::string /* side-id */, std::reference_wrapper<T>>;
/**
 * @brief Specialization resolving user customization applied with side<...> and registering the component side dependencies of the given type.
 *
 * @tparam T User component type (CRTP pattern).
 * @tparam U Dependency type of the component side dependencies to be registered.
 * @tparam Vs Other user applied customizations.
 */
template <typename T, typename U, typename... Vs>
class Component<T, side<U>, Vs...> : public Component<T, Vs...> {
public:
    virtual ~Component() = default;

protected:
    friend class Factory<T>;

    Component() = default;

    /**
     * @brief @see Component<T>::registerAs
     */
    void registerAs(DependencyRegistry& dependencyRegistry) {
        struct SideDependenciesExtractor : public T {
            // static_assert TODO https://stackoverflow.com/questions/36692345/type-trait-check-if-class-have-specific-function-maybe-inherit

            static void extract(T& t, SideDependencies<U>& sideDependencies) {
                (t.*(static_cast<void (T::*)(SideDependencies<U>&)>(&T::side)))(sideDependencies);
            }
        };

        if (!std::is_base_of<Component<T, Vs...>, side<U>>::value) {   // avoid registering the same side dependencies multiple times
            SideDependencies<U> sideDependencies;
            SideDependenciesExtractor::extract(static_cast<T&>(*this), sideDependencies);

            for (const auto& kv : sideDependencies) {
                if (kv.first.empty()) {
                    throw -0.5f;
                }

                std::set<std::string>::const_iterator it;
                bool emplaced;
                std::tie(it, emplaced) = Component<T>::sideDependencyIdentifiers_.emplace(Component<T>::id() + "_"s + kv.first);

                if (!emplaced) {
                    throw 9999;
                }

                dependencyRegistry.add<U>(*it, kv.second);
            }
        }   // destruct sideDependencies object

        Component<T, Vs...>::registerAs(dependencyRegistry);
    }
};

}   // namespace diff
